#ifndef XALT_REGEX_H
#define XALT_REGEX_H


#define N_ELEMENTS(a) (sizeof(a)/sizeof((a)[0]))

const char* acceptA[] 	= {  };
const char* ignoreA[] 	= { "^/sbin/","^/bin/","^/etc","^/usr","^/opt/apps/intel/","^/opt/apps/gcc/","^/opt/apps/lua","^/opt/apps/lmod/","^/opt/apps/icon/","^/opt/apps/noweb/","^/opt/apps/shell_startup_debug/","^/opt/apps/xalt/","^/opt/apps/intel-[0-9][0-9_]*/mpich/","^/opt/apps/intel-[0-9][0-9_]*/openmpi/","^/opt/VBoxGuestAdditions" };
const char* hostnameA[] = { ".*" };


const int   acceptSz    = N_ELEMENTS(acceptA);
const int   ignoreSz    = N_ELEMENTS(ignoreA);
const int   hostnameSz  = N_ELEMENTS(hostnameA);

#endif

/* Local Variables: */
/* mode: c */
/* End: */
