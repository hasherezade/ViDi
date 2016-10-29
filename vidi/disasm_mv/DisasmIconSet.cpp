#include "DisasmIconSet.h"

#define MIN_FIELD_HEIGHT 20
#define MIN_FIELD_WIDTH 20

void DisasmIconSet::init() {
    /* make icons */
    tracerIcon = makeScaledIcon(":/icons/space.ico", MIN_FIELD_WIDTH,  MIN_FIELD_HEIGHT);
    tracerUpIcon = makeScaledIcon(":/icons/space_up.ico", MIN_FIELD_WIDTH,  MIN_FIELD_HEIGHT);
    tracerDownIcon = makeScaledIcon(":/icons/space_down.ico", MIN_FIELD_WIDTH,  MIN_FIELD_HEIGHT);
    tracerSelf = makeScaledIcon(":/icons/space_this.ico", MIN_FIELD_WIDTH,  MIN_FIELD_HEIGHT);

    callUpIcon = QIcon(":/icons/up.ico");
    callDownIcon = QIcon(":/icons/down.ico");
    callWrongIcon = QIcon(":/icons/wrong_way.ico");
    starIcon = QIcon(":/icons/star.ico");
}

QIcon DisasmIconSet::makeScaledIcon(QString resource, int w, int h)
{
    QPixmap pix(resource);
    pix = pix.scaled(w, h);
    return QIcon(pix);
}
