#ifndef RUN_SUBMISSION_H
#define RUN_SUBMISSION_H

#include "Options.h"
#include "xalt_types.h"

void buildEnvT(Options& options, char* env[], Table& envT);
void filterEnvT(char* env[], Table& envT);
void buildUserT(Options& options, Table& envT, Table& userT, DTable& userDT);
void compute_sha1(std::string& fn, std::string& sha1);
void extractXALTRecord(std::string& exec, std::string& watermark, Table& recordT);
void parseProcMaps(pid_t pid, std::vector<Libpair>& libA, double& t_maps, double& t_sha1);
void pkgRecordTransmit(Options& options, const char* transmission);
void run_direct2db(const char* confFn, std::string& usr_cmdline, std::string& hash_id, 
                   Table& rmapT, Table& envT, Table& userT,
                   Table& recordT, std::vector<Libpair>& lddA);
void translate(Table& envT, Table& userT, DTable& userDT);



#endif //RUN_SUBMISSION_H
