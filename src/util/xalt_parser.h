#ifndef   XALT_PARSER_H
#define   XALT_PARSER_H
#include "xalt_obfuscate.h"
#include "extern_hack_begin.h"

typedef enum { PKGS=1, KEEP=2, SKIP=3, CUSTOM = 4, CONTINUE = 5, 
  JUMP_1 = 6, JUMP_2 = 7,JUMP_3 = 8, JUMP_4 = 9 } xalt_parser;

EXTERN const char * xalt_parserA[] 
#if defined(ALLOCATE)
= {
  "ILLEGAL",
  "PKGS",
  "KEEP",
  "SKIP",
  "CUSTOM",
  "CONTINUE",
  "JUMP_1",
  "JUMP_2",
  "JUMP_3",
  "JUMP_4"
}
#endif
;

EXTERN int jumpArgA[]
#if defined(ALLOCATE)
= {
  0,   /* ILLEGAL  */
  0,   /* PKGS     */
  0,   /* KEEP     */
  0,   /* SKIP     */
  0,   /* CUSTOM   */
  0,   /* CONTINUE */
  2,   /* JUMP_1   */
  3,   /* JUMP_2   */
  4,   /* JUMP_3   */
  5,   /* JUMP_4   */
}
#endif
;




#include "extern_hack_end.h"
#endif // XALT_PARSER_H
