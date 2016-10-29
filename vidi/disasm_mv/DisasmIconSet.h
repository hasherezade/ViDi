#pragma once
#include <QIcon>

class DisasmIconSet
{
public:
    DisasmIconSet() { init(); }
    virtual ~DisasmIconSet() {}

    QIcon tracerIcon, tracerUpIcon, tracerDownIcon, tracerSelf,
        callUpIcon, callDownIcon, callWrongIcon, starIcon;

protected:
    void init();
    QIcon makeScaledIcon(QString resource, int w, int h);
};
