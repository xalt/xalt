#ifndef XALT_HOSTNAME_PARSER_H
#define XALT_HOSTNAME_PARSER_H

#include "xalt_obfuscate.h"

int hostname_parser(const char * my_hostname);
void hostname_parser_cleanup();

int my_hostname_parser(const char * my_hostname);
void my_hostname_parser_cleanup();

#endif //XALT_HOSTNAME_PARSER_H
