#if defined(__linux__)
# include "system/linux.c"
#elif defined(__sun__)
# include "system/solaris.c"
#elif defined(__hpux__)
# include "system/hpux.c"
#elif defined(__QNX__)
# include "system/qnx.c"
#else
# include "system/generic.c"
#endif
