#ifdef __linux__
#include "system/linux.c"
#else
#ifdef __solaris__
#include "system/solaris.c"
#else
#ifdef __hpux__
#include "system/hpux.c"
#else
#include "system/generic.c"
#endif
#endif
#endif
