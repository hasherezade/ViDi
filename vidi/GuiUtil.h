#include <QtWidgets>
#include <bearparser.h>

namespace vidi
{
    static QString translateAddrTypeName(const Executable::addr_type addrType)
    {
        switch (addrType) {
            case Executable::RAW : return "Raw";
            case Executable::RVA : return "RVA";
            case Executable::VA : return "VA";
        }
        return "";
    }
};
