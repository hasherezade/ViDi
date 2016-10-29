#pragma once

#include <QtGui>
#include <bearparser.h>

#include "Executables.h"
#include "LoadProgressBar.h"

class Controller : public QObject
{
    Q_OBJECT

signals:
    void loadingFailed(QString );
    void loadingFinished();
    void exeListChanged();
    void exeSelected(ExeHandler*);
    void loadingProgress(int);

    void offsetSelected(offset_t, Executable::addr_type);
    void offsetNotFound(offset_t, Executable::addr_type);

public:
    Controller() : m_ExeSelected(NULL) {}
    ExeHandler* selectedExe() { return  m_ExeSelected; }
    bool parse(QString &fileName, LoadProgressBar *progressWatcher);

    CodeBlock* getBlockAtOffset(offset_t target, const Executable::addr_type aType = Executable::RAW);

    Executables m_exes;

public slots:
    void removeExe(ExeHandler* exe);

    void selectExe(ExeHandler* exe)
    {
        if (exe == m_ExeSelected) return;
        m_ExeSelected = exe;
        emit exeSelected(exe);
    }

    void openTags();
    void saveTags();

private slots:
    void onLoadingFailed(QString fileName)
    {
        QMessageBox::warning(NULL,"Error!", "Cannot load the file:" + fileName);
    }

    void onLoaderThreadFinished()
    {
        delete QObject::sender();
        if (m_exes.size() == 0) return;

        const size_t lastExe = m_exes.size() - 1;
        ExeHandler *lastHndl = m_exes.at(lastExe);
        selectExe(lastHndl);
    }

protected:
    ExeHandler* m_ExeSelected;
};
