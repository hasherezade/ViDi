#pragma once
#include <QtGui>

//autodeleting on finish
class LoadProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    LoadProgressBar(QWidget *parent)
        : QProgressBar(parent)
    {
        setValue(0);
        setMaximum(100);
    }

    ~LoadProgressBar()
    {
        printf("~LoadProgressBar\n");
    }

public slots:
    void setValue(int value)
    {
        if (value > minimum()) {
            this->setVisible(true);
        }
        QProgressBar::setValue(value);
        if (value >= maximum()) {
            this->setVisible(false);
            delete(this);
        }
    }
};
