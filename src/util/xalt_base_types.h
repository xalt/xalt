#ifndef XALT_BASE_TYPES_H
#define XALT_BASE_TYPES_H

#define HERE do { fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); fflush(stderr); } while(0)

#define DEBUG(fp,format, ...)    if (xalt_tracing) do { fprintf((fp),format __VA_OPT__(,)  __VA_ARGS__); fflush(fp); } while (0)
#define DEBUG0(fp,s)             if (xalt_tracing) do { fprintf((fp),s);                     fflush(fp); } while (0)
#define DEBUG1(fp,s,x1)          if (xalt_tracing) do { fprintf((fp),s,(x1));                fflush(fp); } while (0)
#define DEBUG2(fp,s,x1,x2)       if (xalt_tracing) do { fprintf((fp),s,(x1),(x2));           fflush(fp); } while (0)
#define DEBUG3(fp,s,x1,x2,x3)    if (xalt_tracing) do { fprintf((fp),s,(x1),(x2),(x3));      fflush(fp); } while (0)
#define DEBUG4(fp,s,x1,x2,x3,x4) if (xalt_tracing) do { fprintf((fp),s,(x1),(x2),(x3),(x4)); fflush(fp); } while (0)


#endif //XALT_BASE_TYPES_H
