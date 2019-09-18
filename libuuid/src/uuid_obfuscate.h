#ifndef UUID_OBFUSCATE
#define UUID_OBFUSCATE

#define PASTE2(a,b)    PaStE2(a,b)
#define PaStE2(a,b)    a##b

#ifndef HIDE
#define HIDE _uuid_1_5
#endif

// Functions
#define uuid_clear                  PASTE2(__UUID_uuid_clear,                 HIDE)
#define uuid_compare                PASTE2(__UUID_uuid_compare,               HIDE)
#define uuid_copy                   PASTE2(__UUID_uuid_copy,                  HIDE)
#define uuid_generate               PASTE2(__UUID_uuid_generate,              HIDE)
#define uuid_generate_random        PASTE2(__UUID_uuid_generate_random,       HIDE)
#define uuid_generate_time          PASTE2(__UUID_uuid_generate_time,         HIDE)
#define uuid_generate_time_safe     PASTE2(__UUID_uuid_generate_safe,         HIDE)
#define uuid_generate_md5           PASTE2(__UUID_uuid_generate_md5,          HIDE)
#define uuid_generate_sha1          PASTE2(__UUID_uuid_generate_sha1,         HIDE)
#define uuid_is_null                PASTE2(__UUID_uuid_is_null,               HIDE)
#define uuid_parse                  PASTE2(__UUID_uuid_parse,                 HIDE)
#define uuid_unparse                PASTE2(__UUID_uuid_unparse,               HIDE)
#define uuid_unparse_lower          PASTE2(__UUID_uuid_unparse_lower,         HIDE)
#define uuid_unparse_upper          PASTE2(__UUID_uuid_unparse_upper,         HIDE)
#define uuid_time                   PASTE2(__UUID_uuid_time,                  HIDE)
#define uuid_type                   PASTE2(__UUID_uuid_type,                  HIDE)
#define uuid_variant                PASTE2(__UUID_uuid_variant,               HIDE)
#define uuid_get_template           PASTE2(__UUID_uuid_get_template,          HIDE)
#define uuid_pack                   PASTE2(__UUID_uuid_pack,                  HIDE)
#define uuid_unpack                 PASTE2(__UUID_uuid_unpack,                HIDE)
#define __uuid_generate_random      PASTE2(__UUID___uuid_generate_random,     HIDE)
#define __uuid_generate_time        PASTE2(__UUID___uuid_generate_time,       HIDE)

#define rand_get_number             PASTE2(__UUID_rand_get_number,            HIDE)
#define random_get_fd               PASTE2(__UUID_random_get_fd,              HIDE)
#define random_get_bytes            PASTE2(__UUID_random_get_bytes,           HIDE)
#define random_tell_source          PASTE2(__UUID_random_tell_source,         HIDE)


// Data



#endif /* UUID_OBFUSCATE */
