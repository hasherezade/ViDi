#pragma once

#include <bearparser.h>

#define INVALID_INDEX (-1)

namespace minidis {

class AddrConverter {
public:
    virtual offset_t convertAddr(offset_t off, Executable::addr_type inType, Executable::addr_type outType) const = 0;
    virtual Executable::addr_type detectAddrType(offset_t off, Executable::addr_type hintType) const = 0;
};

}; /* namespace pe_bear */
