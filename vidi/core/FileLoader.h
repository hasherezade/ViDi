#pragma once

#include <QtWidgets>
#include <QFile>
#include <bearparser/core.h>
#include <stdexcept>

class ProgressWatcher : public QObject
{
};

class FileLoader : public QThread
{
    Q_OBJECT
public:
    FileLoader(QString fileName)
        :  m_Buffer(NULL), m_Status(false), m_FileName(fileName) { }

    virtual ~FileLoader()
    {
        printf("~FileLoader: %s\n", m_FileName.toStdString().c_str());
    }

signals:
    void loaded(AbstractByteBuffer *buffer);
    void loadingFailed(QString fileName);
    void loadingProgress(int progress);

protected:
    void run();
    virtual bool parse(QString &fileName);

    QString m_FileName;
    bool m_Status;

private:
    AbstractByteBuffer *m_Buffer;
    QMutex m_arrMutex;
};
