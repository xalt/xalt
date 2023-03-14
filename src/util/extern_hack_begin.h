#if defined(ALLOCATE)
#  define EXTERN
#  define INIT(x) = x
#else
#  define EXTERN extern
#  define INIT(x)
#endif
