#ifndef XALT_PKG_PARSER_H
#define XALT_PKG_PARSER_H

typedef enum { PKGS=1, KEEP=2, SKIP=3} xalt_parser;

int keep_pkg(const char * path);
void pkg_parser_cleanup();

#endif //XALT_PKG_PARSER_H
