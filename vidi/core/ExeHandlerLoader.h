#pragma once

#include <QtWidgets>
#include <QFile>
#include <bearparser.h>
#include <stdexcept>

#include "FileLoader.h"
#include "ExeHandler.h"
 
class ExeHandlerLoader : public FileLoader
{
    Q_OBJECT
signals:
    void loaded(ExeHandler *exeHndl);

private slots:
    void onTracerLoadingProgress(int progress) 
    {
        updateProgress(progress*0.1);
    }

public:
    ExeHandlerLoader(QString fileName) 
        : FileLoader(fileName),
        m_fileName(fileName), m_loadingProgress(0)
    {
    }

    virtual bool parse(QString &fileName);

protected:
    virtual bool trace(ExeHandler& exeHndl);
    
    void updateProgress(int increment)
    {
        m_loadingProgress += increment;
        m_loadingProgress %= 100;
        emit loadingProgress(m_loadingProgress);
    }

    int m_loadingProgress;
    QString m_fileName;
};
