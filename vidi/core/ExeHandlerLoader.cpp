#include "ExeHandlerLoader.h"

#define MAX_TRACE_DEPTH 3
#define MAX_TRACE_UNSOLVED 1000
#define MIN_TRACE_RESOLVED 10

bool ExeHandlerLoader::trace(ExeHandler &exeHndl)
{
    exeHndl.makeTracer();
    Tracer * tracer = exeHndl.getTracer();
    Executable* exe = exeHndl.getExe();

    if (!exe || !tracer) {
        return false;
    }
    connect(tracer, SIGNAL(loadingProgress(int)), this, SLOT(onTracerLoadingProgress(int)));
    
    tracer->traceEntrySection();
    updateProgress(5);
    
    QSet<offset_t> prologs;
    size_t prologsCount = tracer->findAllPrologs(prologs);
    std::cout << "Found prologs: " << std::dec << prologsCount << "\n";
    
    QMap<offset_t, QString> entrypoints;
    const size_t entryPointsCount = exe->getAllEntryPoints(entrypoints, Executable::RAW);
    updateProgress(1);
    std::cout << "Found entryPoints: " << std::dec << entryPointsCount << "\n";
    
    size_t totalFunctions = prologsCount + entryPointsCount;
    
    int added = 0;
    QSet<offset_t>::const_iterator pItr;
    for (pItr = prologs.constBegin(); pItr != prologs.constEnd(); ++pItr) {
        offset_t prologOffset = *pItr;
        
        offset_t prologOffsetRVA = exe->convertAddr(prologOffset, Executable::RAW, Executable::RVA);
        QString name = "func_" + QString::number(prologOffsetRVA, 16).toUpper();
        
        tracer->defineFunction(prologOffset, Executable::RAW, name);
        tracer->resolveOffset(prologOffset, Executable::RAW);
        
        added++;
        if (added >= (0.1 * totalFunctions)) {
            updateProgress(1);
            added = 0;
        }
    }
    QMap<offset_t,QString>::const_iterator itr;
    for (itr = entrypoints.constBegin(); itr != entrypoints.constEnd(); ++itr) {
        const offset_t epRaw = itr.key();
        const QString name = itr.value();
        tracer->defineFunction(epRaw, Executable::RAW, name);
        tracer->resolveOffset(epRaw, Executable::RAW);
        
        added++;
        if (double(added) >= (0.1 * totalFunctions)) {
            updateProgress(1);
            added = 0;
        }
    }
    
    tracer->resolveUnsolved(MAX_TRACE_DEPTH, MAX_TRACE_UNSOLVED);
    updateProgress(5);
    return true;   
}

bool ExeHandlerLoader::parse(QString &fileName)
{
    m_fileName = fileName;

    bool isLoaded = false;
    ExeHandler *exeHndl = NULL;
    try {
        printf("Emited progress: %d\n", 0);
        //onst bufsize_t MINBUF = 0x200;
        AbstractByteBuffer *buf = new FileView(fileName);
        if (buf == NULL) {
            return false;
        }
        updateProgress(10);
        ExeFactory::exe_type exeType = ExeFactory::findMatching(buf);
        if (exeType == ExeFactory::NONE) {
            delete buf;
            return false;
        }

        Executable *exe = ExeFactory::build(buf, exeType);
        updateProgress(5);

        exeHndl = new ExeHandler(buf, exe);
        updateProgress(10);

        if (!exeHndl) {
            return false;
        }
        exeHndl->setFileName(fileName);
        isLoaded = true;
        trace(*exeHndl);
        
        emit loaded(exeHndl);

    } catch (CustomException &e) { }

    return isLoaded;
}
//----

