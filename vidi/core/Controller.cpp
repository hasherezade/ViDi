#include "Controller.h"
#include "ExeHandlerLoader.h"

CodeBlock* Controller::getBlockAtOffset(offset_t target, const Executable::addr_type aType)
{
    if (target == INVALID_ADDR) return NULL;

     ExeHandler *hndl = selectedExe();
     if (!hndl) return NULL;

     Tracer *dis = hndl->getTracer();
     if (!dis) return NULL;

    CodeBlock* block = dis->blockAt(target, aType);

    if (!block && target != INVALID_ADDR) {
        hndl->resolveOffset(target, aType);
        block = dis->blockAt(target, aType);
    }
    return block;
}

void Controller::openTags()
{
    ExeHandler* exeHndl = m_ExeSelected;
    if (exeHndl == NULL) {
        QMessageBox::warning(NULL, "Error!", "No exe selected!");
        return;
    }
    QString infoStr ="Import tags";
    QFileInfo inputInfo(exeHndl->getFileName());

    QString fileName = QFileDialog::getOpenFileName(
        NULL,
        infoStr,
        inputInfo.absoluteDir().path(),
        "Config file (*.txt);;Config file (*.cfg);;All files (*.*)"
    );
    if (fileName.length() == 0) return;

    size_t counter = exeHndl->loadFunctionNames(fileName);
    if (counter == 0) {
        QMessageBox::warning(NULL, "Error!", "Cannot import!");
        return;
    } else {
        QString ending =  (counter > 1) ? "s":" ";
        QMessageBox::information(NULL, "Done!", "Imported: " + QString::number(counter) + " tag" + ending);
    }
}

void Controller::saveTags()
{
    ExeHandler* exeHndl = m_ExeSelected;
    if (exeHndl == NULL) {
        QMessageBox::warning(NULL, "Error!", "No exe selected!");
        return;
    }
    /*
    if (exeHndl->hasReplacements() == false) {
        QMessageBox::warning(NULL, "Cannot save!", "The file have NO replacements defined!");
        return;
    }*/
    QString infoStr ="Save tags as...";
    QFileInfo inputInfo(exeHndl->getFileName());

    QString fileName = QFileDialog::getSaveFileName(
        NULL,
        infoStr,
        inputInfo.absoluteDir().path(),
        "Config file (*.txt);;Config file (*.cfg);;All files (*.*)"
    );
    if (fileName.length() == 0) return;

    size_t counter = exeHndl->saveFunctionNames(fileName);
    if (counter == 0) {
        QMessageBox::warning(NULL, "Error!", "Cannot export!");
    } else {
        QString ending =  (counter > 1) ? "s":" ";
        QMessageBox::information(NULL, "Done!", "Exported: " + QString::number(counter) + " tags" + ending);
    }
}

void Controller::removeExe(ExeHandler* exe)
{
    selectExe(NULL);
    this->m_exes.removeExe(exe);
    delete exe;
    exe = NULL;
}

bool Controller::parse(QString &fileName, LoadProgressBar *progressWatcher)
{
    if (fileName == "") return false;

    QString link = QFile::symLinkTarget(fileName);
    if (link.length() > 0) fileName = link;

    bufsize_t maxMapSize = FILE_MAXSIZE;
    try {
        FileView fileView(fileName, maxMapSize);
        ExeFactory::exe_type exeType = ExeFactory::findMatching(&fileView);
        if (exeType == ExeFactory::NONE) {
            QMessageBox::warning(NULL,"Cannot parse!", "Cannot parse the file: \n"+fileName+"\n\nType not supported.");
            return false;
        }

        ExeHandlerLoader *loader = new ExeHandlerLoader(fileName);
        QObject::connect(loader, SIGNAL( loaded(ExeHandler*) ), &m_exes, SLOT( addExe(ExeHandler*) ) );
        QObject::connect(loader, SIGNAL( loadingFailed(QString ) ), this, SLOT( onLoadingFailed(QString ) ) );
        QObject::connect(loader, SIGNAL(finished()), this, SLOT( onLoaderThreadFinished() ) );

        if (progressWatcher) {
            connect(loader, SIGNAL(loadingProgress(int)), progressWatcher, SLOT(setValue(int)));
        }
        //printf("Thread started...\n");
        loader->start();

    } catch (CustomException &e) {
        QMessageBox::warning(NULL, "ERROR", e.getInfo());
        return false;
    }
    return true;
}

