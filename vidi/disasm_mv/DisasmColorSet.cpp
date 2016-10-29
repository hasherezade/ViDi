#include "DisasmColorSet.h"

void DisasmColorSet::init()
{
    vHdrColor = QColor("LightGrey");
    vHdrColor.setAlpha(50);

    branchingColor = QColor("darkGrey");
    branchingColor.setAlpha(30);

    retColor = QColor("DodgerBlue");
    nopColor = QColor("grey");
    int3Color = QColor("magenta");
    invalidColor = QColor("red");
    importColor = QColor("cyan");
    delayImpColor = QColor("pink");
    conditionalColor = QColor("yellow");
    internalCallColor = QColor("orange");
    internalCallColor.setAlpha(200);
    basicTextColor = QColor("black");
    basicBgColor = QColor("white");
    clickableColor = QColor("pink");
    clickableColor.setAlpha(100);
    referingColor = QColor("red");
    referingColor.setAlpha(100);
    referedColor = QColor("goldenrod");
    referedColor.setAlpha(70);
}
