/* #define DEBUG */
#ifdef DEBUG
#define DEBUGPRINT(_fmt, ...)  fprintf(stderr, "[file %s, line %d]: " _fmt, __FILE__, __LINE__, __VA_ARGS__)
#else
#define DEBUGPRINT(_fmt, ...)
#endif // DEBUG
