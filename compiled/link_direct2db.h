#ifndef LINK_DIRECT2DB_H
#define LINK_DIRECT2DB_H

#include "xalt_types.h"

void link_direct2db(const char* confFn, Vstring& linklineA, Table& resultT,
                    std::vector<Libpair>& libA, Set& funcSet, Table& rmapT);

#endif //LINK_DIRECT2DB_H
