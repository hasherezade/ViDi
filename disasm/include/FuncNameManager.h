#pragma once

#include <vector>
#include <bearparser/core.h>

namespace minidis {

class AddrConverter;
//----------------------------------------------

class FuncNameManager
{
public:
    FuncNameManager(AddrConverter *converter) 
        : m_Converter(converter), m_aType(Executable::RAW) { }
    virtual ~FuncNameManager() {}

    bool setFunctionName(offset_t offset, Executable::addr_type inType, QString name);
    bool hasName(offset_t offset, Executable::addr_type inType) const;
    QString getFunctionName(offset_t offset, Executable::addr_type inType) const;
    QList<offset_t>& getNamedOffsetsList() { return m_namedOffsetsList; }

    size_t save(const QString &fileName);
    size_t load(const QString &fileName);

protected:
    AddrConverter *m_Converter;
    QMap<QString, offset_t> nameToFunction;
    QMap<offset_t, QString> functionToName;
    Executable::addr_type m_aType;
    QList<offset_t> m_namedOffsetsList;
};

class FuncManager
{
public:
    FuncManager(AddrConverter *converter);
    bool appendFunction(offset_t offset, Executable::addr_type inType);

    Executable::addr_type getAddrType();
    QList<offset_t>& list() { return m_functionsList; }

protected:
    AddrConverter *m_Converter;

    QList<offset_t> m_functionsList;
    QSet<offset_t> m_functions;
    Executable::addr_type m_aType;
};

}; //namespace minidis
