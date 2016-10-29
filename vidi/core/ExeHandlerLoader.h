#pragma once

#include <QtGui>
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
    //void loadingProgress(QString fileName, int progress);

private slots:
    void onLoadingProgress(QString fileName, int progress) {
        //printf("#Emited progress: %d\n", progress);
    }

    void onTracerLoadingProgress(int progress) 
    {
        updateProgress(progress * 0.5);
    }

public:
    ExeHandlerLoader(QString fileName) 
        : FileLoader(fileName),
        m_fileName(fileName), m_loadingProgress(0)
    {
    }

    virtual bool parse(QString &fileName);

protected:
    void updateProgress(int increment)
    {
        m_loadingProgress += increment;
        emit loadingProgress(m_loadingProgress);
        //emit loadingProgress(m_fileName, m_loadingProgress);
    }

    int m_loadingProgress;
    QString m_fileName;
};