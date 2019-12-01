#pragma once
#include <QFile>
#include <bearparser/core.h>
#include <pe/PeTracer.h>
#include <dos/DosTracer.h>

#include "ExeHandlerBase.h"
#include "NavigHistory.h"

using namespace minidis;

class ExeHandler : public ExeHandlerBase
{
Q_OBJECT

public:
    ExeHandler(AbstractByteBuffer *buf, Executable* exe)
        : ExeHandlerBase(buf, exe), m_Tracer(NULL)
    {
        connect(&m_NavigHistory, SIGNAL(stateChanged()), this, SLOT(onHistoryStateChanged()));
    }

    virtual ~ExeHandler()
    {
        delete m_Tracer;
    }

    Tracer* getTracer() const { return m_Tracer; }
    NavigHistory& getNavigHistory() { return m_NavigHistory; }
    
    bool setFunctionName(offset_t offset, Executable::addr_type inType, QString name);
    size_t saveFunctionNames(const QString &fileName);
    size_t loadFunctionNames(const QString &fileName);

    bool resolveOffset(offset_t offset, Executable::addr_type aType);

protected slots:
    void onChildStateChanged() { emit stateChanged(); }
    void onHistoryStateChanged() { emit stateChanged(); }

protected:
    void makeTracer();

    Tracer* m_Tracer;
    NavigHistory m_NavigHistory;

friend class ExeHandlerLoader;
friend class Controller;
};
