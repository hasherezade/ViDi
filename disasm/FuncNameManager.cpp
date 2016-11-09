#include "FuncNameManager.h"
#include "Tracer.h"

using namespace minidis;

bool FuncNameManager::setFunctionName(offset_t offset, Executable::addr_type inType, QString name)
{
    offset = m_Converter->convertAddr(offset, inType, Executable::RAW);
    if (offset == INVALID_ADDR) return false;

    bool tagExist = false;
    if (this->functionToName.contains(offset)) {
        nameToFunction.remove(functionToName[offset]);
        tagExist = true;
    }
    if (name == "") {
        this->functionToName.remove(offset);
        m_namedOffsetsList.removeOne(offset);
        return true;
    }
    if (nameToFunction.contains(name)) {
        name = name + "@" + QString::number(offset, 16);
    }
    functionToName[offset] = name;
    nameToFunction[name] = offset;
    if (!tagExist) m_namedOffsetsList.append(offset);
    return true;
}

bool FuncNameManager::hasName(offset_t offset, Executable::addr_type inType) const
{
    offset_t raw = m_Converter->convertAddr(offset, inType, Executable::RAW);
    return functionToName.contains(raw);
}

QString FuncNameManager::getFunctionName(offset_t offset, Executable::addr_type inType) const
{
    offset_t raw = m_Converter->convertAddr(offset, inType, Executable::RAW);
    if (raw == INVALID_ADDR) {
        return " <invalid>";
    }
    if (this->functionToName.contains(raw)) {
        return functionToName[raw];
    }
    return QString::number(offset, 16);
}

//----

FuncManager::FuncManager(AddrConverter *converter)
    : m_Converter(converter), m_aType(Executable::RVA)
{
}

Executable::addr_type FuncManager::getAddrType()
{
    return m_aType;
}

bool FuncManager::appendFunction(offset_t offset, Executable::addr_type inType)
{
    offset_t cOffset = m_Converter->convertAddr(offset, inType, m_aType);
    if (cOffset == INVALID_ADDR) {
        printf("Cannot append function : failed convertion at : %llx\n", static_cast<unsigned long long>(offset));
        return false;
    }
    size_t size = m_functions.size();
    m_functions.insert(cOffset);

    if (m_functions.size() == size) { //already exist
        return true;
    }
    this->m_functionsList.append(cOffset);
    return true;
}

size_t FuncNameManager::save(const QString &fileName)
{
    QFile outputFile(fileName);
    if ( !outputFile.open(QIODevice::WriteOnly | QIODevice::Text) ) return 0;

    QTextStream out(&outputFile);
    QMap<offset_t, QString>::iterator itr;
    size_t counter = 0;
    for (itr = functionToName.begin(); itr != functionToName.end(); itr++) {
        //convert every offset into RVA:
        offset_t rva = m_Converter->convertAddr(itr.key(), Executable::RAW, Executable::RVA);
        out << hex << rva;
        out << ",";
        out << itr.value();
        out << '\n';
        counter++;
        
    }
    outputFile.close(); 
    return counter;
}

size_t FuncNameManager::load(const QString &fileName)
{
    size_t loaded = 0;
    QFile inputFile(fileName);

    if ( !inputFile.open(QIODevice::ReadOnly) ) {
        printf("Cannot open file!");
        return 0;
    }
    offset_t offset = 0;
    QString thunkStr = "";
    QString funcName = "";

    QTextStream in(&inputFile);
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        QStringList list = line.split(',');
        if (list.size() < 2) {
            continue; //invalid line, skip it
        }
        QString thunkStr = list[0];
        QString funcName = list[1];
        bool isOk = false;
        offset = thunkStr.toLongLong(&isOk, 16);
        
        if (!isOk) {
            continue; //invalid line, skip it
        }
        //functions are stored by RVAs
        if (setFunctionName(offset, Executable::RVA, funcName)) loaded++;
    }
    inputFile.close();
    return loaded;
}