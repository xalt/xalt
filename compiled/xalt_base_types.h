#ifndef XALT_BASE_TYPES_H
#define XALT_BASE_TYPES_H

#define HERE fprintf(stderr,"%s:%d\n",__FILE__,__LINE__)

#define DEBUG0(fp,s)          if (xalt_tracing) fprintf(fp,s)
#define DEBUG1(fp,s,x1)       if (xalt_tracing) fprintf(fp,s,(x1))
#define DEBUG2(fp,s,x1,x2)    if (xalt_tracing) fprintf(fp,s,(x1),(x2))
#define DEBUG3(fp,s,x1,x2,x3) if (xalt_tracing) fprintf(fp,s,(x1),(x2),(x3))

#endif //XALT_BASE_TYPES_H
