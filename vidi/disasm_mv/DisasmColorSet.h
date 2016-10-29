#pragma once

#include <QColor>

class DisasmColorSet
{
public:
    DisasmColorSet() { init(); }
    virtual ~DisasmColorSet() {}

    QColor basicTextColor, basicBgColor,
        vHdrColor, branchingColor, retColor, nopColor, int3Color, invalidColor,
        importColor, delayImpColor,
        conditionalColor, internalCallColor, referingColor, referedColor, clickableColor;

protected:
    void init();
};
