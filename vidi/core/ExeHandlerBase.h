#pragma once
#include <QFile>
#include <bearparser.h>

class ExeHandlerBase : public QObject
{
Q_OBJECT

signals:
    void stateChanged();

public:
    ExeHandlerBase(AbstractByteBuffer *buf, Executable* exe)
        : m_Buf(buf), m_Exe(exe)
    {
    }

    virtual ~ExeHandlerBase()
    {
        delete m_Exe,
        delete m_Buf;
    }

    Executable* getExe() { return m_Exe; }
    QString getFileName() { return m_fileName; }
    bool isTruncated() { return m_Buf->isTruncated(); }

protected:
    void setFileName(QString fileName) { this->m_fileName = fileName; emit stateChanged(); }

    QString m_fileName;
    AbstractByteBuffer *m_Buf;
    Executable* m_Exe;

friend class ExeHandlerBaseLoader;
};
