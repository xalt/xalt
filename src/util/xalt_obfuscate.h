#ifndef XALT_OBFUSCATE
#define XALT_OBFUSCATE

#define PASTE2(a,b)    PaStE2(a,b)
#define PaStE2(a,b)    a##b
#define PASTE3(a,b,c)  PaStE3(a,b,c)
#define PaStE3(a,b,c)  a##b##c

#ifndef HIDE
#define HIDE _xalt_1_5
#endif

#define UNDERSCORE         _
#define MY_NAME            PASTE3(UNDERSCORE,STATE,HIDE)

// Functions

#define __fini                        PASTE2(__XALT__fini,                      MY_NAME)
#define __init                        PASTE2(__XALT__init,                      MY_NAME)
#define myfini                        PASTE2(__XALT_myfini,                     MY_NAME)
#define myinit                        PASTE2(__XALT_myinit,                     MY_NAME)
#define wrapper_for_myfini            PASTE2(__XALT_wrapper_for_myfini,         MY_NAME)

#define buildEnvT                     PASTE2(__XALT_buildEnvT,                  HIDE)
#define buildUserT                    PASTE2(__XALT_buildUserT,                 HIDE)
#define buildXALTRecordT              PASTE2(__XALT_buildXALTRecordT,           HIDE)
#define build_proc                    PASTE2(__XALT_build_proc,                 HIDE)
#define build_resultDir               PASTE2(__XALT_build_resultDir,            HIDE)
#define build_resultFn                PASTE2(__XALT_build_resultFn,             HIDE)
#define build_uuid                    PASTE2(__XALT_build_uuid,                 HIDE)
#define capture                       PASTE2(__XALT_capture,                    HIDE)
#define compute_sha1                  PASTE2(__XALT_compute_sha1,               HIDE)
#define compute_sha1_cleanup          PASTE2(__XALT_compute_sha1_cleanup,       HIDE)
#define compress_string               PASTE2(__XALT_compress_string,            HIDE)
#define crcInit                       PASTE2(__XALT_crcInit,                    HIDE)
#define crcFast                       PASTE2(__XALT_crcFast,                    HIDE)
#define crcSlow                       PASTE2(__XALT_crcSlow,                    HIDE)
#define create_xalt_tmpdir_str        PASTE2(__XALT_create_xalt_tmpdir_str,     HIDE)
#define env_parser_cleanup            PASTE2(__XALT_crcSlow,                    HIDE)
#define epoch                         PASTE2(__XALT_epoch,                      HIDE)
#define extractXALTRecordString       PASTE2(__XALT_extractXALTRecordString,    HIDE)
#define filterEnvT                    PASTE2(__XALT_filterEnvT,                 HIDE)
#define free_S2D                      PASTE2(__XALT_free_S2D,                   HIDE)
#define free_S2S                      PASTE2(__XALT_free_S2S,                   HIDE)
#define free_SET                      PASTE2(__XALT_free_SET,                   HIDE)
#define free_proc                     PASTE2(__XALT_free_proc,                  HIDE)
#define have_libc_getentropy_func     PASTE2(__XALT_have_libc_getentropy_func,  HIDE)
#define hostname_parser               PASTE2(__XALT_hostname_parser,            HIDE)
#define hostname_parser_cleanup       PASTE2(__XALT_hostname_parser_cleanup,    HIDE)
#define init_proc                     PASTE2(__XALT_init_proc,                  HIDE)
#define isDirectory                   PASTE2(__XALT_isDirectory,                HIDE)
#define is_directory                  PASTE2(__XALT_is_directory,               HIDE)
#define insert_key_SET                PASTE2(__XALT_insert_key_SET,             HIDE)
#define insert_key_double             PASTE2(__XALT_insert_key_double,          HIDE)
#define insert_key_string             PASTE2(__XALT_insert_key_string,          HIDE)
#define json_add_S2D                  PASTE2(__XALT_json_add_S2D,               HIDE)
#define json_add_S2S                  PASTE2(__XALT_json_add_S2S,               HIDE)
#define json_add_SET                  PASTE2(__XALT_json_add_SET,               HIDE)
#define json_add_array                PASTE2(__XALT_json_add_array,             HIDE)
#define json_add_char_str             PASTE2(__XALT_json_add_char_str,          HIDE)
#define json_add_double               PASTE2(__XALT_json_add_double,            HIDE)
#define json_add_int                  PASTE2(__XALT_json_add_int,               HIDE)
#define json_add_json_str             PASTE2(__XALT_json_add_json_str,          HIDE)
#define json_add_libT                 PASTE2(__XALT_json_add_libT,              HIDE)
#define json_add_ptA                  PASTE2(__XALT_json_add_ptA,               HIDE)
#define json_add_utarray              PASTE2(__XALT_json_add_utarray,           HIDE)
#define json_fini                     PASTE2(__XALT_json_fini,                  HIDE)
#define json_init                     PASTE2(__XALT_json_init,                  HIDE)
#define keep_env_name                 PASTE2(__XALT_keep_env_name,              HIDE)
#define keep_path                     PASTE2(__XALT_keep_path,                  HIDE)
#define mkpath                        PASTE2(__XALT_mkpath,                     HIDE)
#define my_free                       PASTE2(__XALT_my_free,                    HIDE)
#define my_hostname_parser            PASTE2(__XALT_my_hostname_parser,         HIDE)
#define my_hostname_parser_cleanup    PASTE2(__XALT_my_hostname_parser_cleanup, HIDE)
#define parseProcMaps                 PASTE2(__XALT_parseProcMaps,              HIDE)
#define path_parser_cleanup           PASTE2(__XALT_path_parser_cleanup,        HIDE)
#define pkgRecordTransmit             PASTE2(__XALT_pkgRecordTransmit,          HIDE)
#define proc_cmdline                  PASTE2(__XALT_proc_cmdline,               HIDE)
#define processTreeFree               PASTE2(__XALT_processTreeFree,            HIDE)
#define queueType                     PASTE2(__XALT_queueType,                  HIDE)
#define remove_xalt_tmpdir            PASTE2(__XALT_remove_xalt_tmpdir,         HIDE)
#define run_submission                PASTE2(__XALT_run_submission,             HIDE)
#define set_end_record                PASTE2(__XALT_set_end_record,             HIDE)
#define track_executable              PASTE2(__XALT_track_executable,           HIDE)
#define track_executable_cleanup      PASTE2(__XALT_track_executable_cleanup,   HIDE)
#define translate                     PASTE2(__XALT_translate,                  HIDE)
#define transmit                      PASTE2(__XALT_transmit,                   HIDE)
#define walkProcessTree               PASTE2(__XALT_walkProcessTree,            HIDE)
#define xalt_dir                      PASTE2(__XALT_dir,                        HIDE)
#define xalt_fgets_alloc              PASTE2(__XALT_fgets_alloc,                HIDE)
#define xalt_file_transmission_method PASTE2(__XALT_file_transmission_method,   HIDE)
#define xalt_quotestring              PASTE2(__XALT_quotestring,                HIDE)
#define xalt_quotestring_free         PASTE2(__XALT_quotestring_free,           HIDE)
#define xalt_syshost                  PASTE2(__XALT_syshost,                    HIDE)
#define xalt_unquotestring            PASTE2(__XALT_unquotestring,              HIDE)
#define xalt_vendor_note              PASTE2(__XALT_vendor_note,                HIDE)
#define xmalloc                       PASTE2(__XALT_xmalloc,                    HIDE)

#define xalt_parserA                  PASTE2(__XALT_parserA,                    HIDE)
#define jumpArgA                      PASTE2(__XALT_jumpArgA,                   HIDE)



#endif //XALT_OBFUSCATE
