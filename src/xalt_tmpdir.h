#ifdef  XALT_TMPDIR_H
#define XALT_TMPDIR_H

#ifdef __cplusplus
extern "C"
{
#endif

char* create_xalt_tmpdir_str(const char* run_uuid);
char* remove_xalt_tmpdir(const char* run_uuid);

#ifdef __cplusplus
}
#endif

#endif /* XALT_TMPDIR_H */
