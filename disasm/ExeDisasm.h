#pragma once

#ifndef BUILD_WITH_UDIS86
    #include "cdis/ExeDisasm.h"
#else
    #ifdef USE_UDIS86
        #include "udis/ExeDisasm.h"
    #else
        #include "cdis/ExeDisasm.h"
    #endif
#endif
