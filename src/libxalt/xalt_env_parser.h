#ifndef XALT_ENV_PARSER_H
#define XALT_ENV_PARSER_H

#ifdef __cplusplus
extern "C"
{
#endif
  
  int keep_env_name(const char * env_string);
  void env_parser_cleanup();

#ifdef __cplusplus
}
#endif

#endif //XALT_ENV_PARSER_H
