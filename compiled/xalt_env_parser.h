#ifndef XALT_ENV_PARSER_H
#define XALT_ENV_PARSER_H

extern "C" {

int keep_env_name(const char * env_string);
void env_parser_cleanup();

}
#endif //XALT_ENV_PARSER_H
