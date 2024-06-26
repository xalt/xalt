#ifndef  DEBUG_MACROS
#define  DEBUG_MACROS

#define STRINGIFY_base(x) #x
#define STRINGIFY(x)      STRINGIFY_base(x)

#define HERE_fp(fp) do { if (fp) {fprintf((fp),"%s:%s\n",__FILE__,STRINGIFY(__LINE__)); fflush((fp));} } while(0)
#define HERE        HERE_fp(stderr)

#define ARGS_FIRST(...)       ARGS_FIRST0(__VA_ARGS__, 0)
#define ARGS_FIRST0(_0, ...)  _0

#define ARGS_REST(...)       ARGS_REST0(__VA_ARGS__, 0)
#define ARGS_REST0(_0, ...)  __VA_ARGS__

#define MAIN_DEBUG(tracing, fp, ...) MAIN_DEBUG_base(tracing, fp, ARGS_FIRST(__VA_ARGS__) "%.0d", ARGS_REST(__VA_ARGS__))

#define MAIN_DEBUG_base(tracing, fp, ...) if (tracing && fp) do { fprintf((fp), __VA_ARGS__  ); fflush(fp); } while (0)


#endif //DEBUG_MACROS

