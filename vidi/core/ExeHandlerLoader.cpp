#include "ExeHandlerLoader.h"

#define MAX_TRACE_DEPTH 3
#define MAX_TRACE_UNSOLVED 200

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

    const offset_t epRaw = exe->getEntryPoint(Executable::RAW);
    tracer->defineFunction(epRaw, Executable::RAW, "START");
    tracer->resolveOffset(epRaw, Executable::RAW);

    tracer->resolveUnsolved(MAX_TRACE_DEPTH, MAX_TRACE_UNSOLVED);
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
        ExeFactory::exe_type exeType = ExeFactory::findMatching(buf);
        if (exeType == ExeFactory::NONE) {
            delete buf;
            return false;
        }

        Executable *exe = ExeFactory::build(buf, exeType);
        //printf("Exe loaded\n");
        updateProgress(10);

        exeHndl = new ExeHandler(buf, exe);
        //printf("ExeHndl loaded\n");
        updateProgress(20);

        if (exeHndl) {
            exeHndl->setFileName(fileName);
            isLoaded = true;
            trace(*exeHndl);
        }

    } catch (CustomException &e) { }

    emit loaded(exeHndl);
    return isLoaded;
}
//----

