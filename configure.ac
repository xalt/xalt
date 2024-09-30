AC_INIT
AC_CONFIG_SRCDIR([Makefile.in])
AC_CONFIG_HEADERS(src/__build__/xalt_header.h:src/util/xalt_header.h.in)
AC_PROG_CC([[gcc cl cc]])
AC_PREFIX_DEFAULT(/usr/local)
dnl AC_CONFIG_SUBDIRS(libuuid/src)

AC_SUBST(XALT_INTERFACE_VERSION)
XALT_INTERFACE_VERSION=5
AH_TOP(
[
/* Remember that AC_DEFINE() end up in xalt_header.h.in
 * AC_SUBST() end up in xalt_config.h.in
 */
])

VALID_YN ()
{
  ANS=$1
  MSG=$2
  case $ANS in
    yes|no)
      ;;
    *)
      echo $MSG
      exit 1
      ;;
  esac
}

ERR_MSG=""
FOUND_ERROR=

AC_SUBST(TRANSMISSION)
AC_ARG_WITH(transmission,
    AS_HELP_STRING([--with-transmission=ans],[transmission style (file,syslog,file_separate_dirs,curl) [[file]]]),
    TRANSMISSION="$withval"
    AC_MSG_RESULT([TRANSMISSION=$with_transmission])
    AC_DEFINE_UNQUOTED(TRANSMISSION, "$with_transmission",[transmission style (file,syslog,file_separate_dirs,curl)])dnl
    ,
    withval="file"
    AC_MSG_RESULT([TRANSMISSION=$withval])
    TRANSMISSION="$withval"
    AC_DEFINE_UNQUOTED(TRANSMISSION, "$withval",[transmission style (file,syslog,file_separate_dirs,curl)]))dnl

AC_SUBST(XALT_LOGGING_URL)
AC_ARG_WITH(xaltLoggingUrl,
    AS_HELP_STRING([--with-xaltLoggingUrl=ans],[Logging URL for transmission [[]]]),
    XALT_LOGGING_URL="$withval"
    AC_MSG_RESULT([XALT_LOGGING_URL=$with_xaltLoggingUrl])
    AC_DEFINE_UNQUOTED(XALT_LOGGING_URL, "$with_xaltLoggingUrl",[Logging URL for transmission])dnl
    ,
    withval="USE_ENV"
    AC_MSG_RESULT([XALT_LOGGING_URL=$withval])
    XALT_LOGGING_URL="$withval"
    AC_DEFINE_UNQUOTED(XALT_LOGGING_URL, "$withval",[Logging URL for transmission]))dnl

AC_SUBST(XALT_FUNCTION_TRACKING)
AC_ARG_WITH(functionTracking,
    AS_HELP_STRING([--with-functionTracking=ans],[Track functions from modules [[yes]]]),
    XALT_FUNCTION_TRACKING="$withval"
    XALT_FUNCTION_TRACKING=`echo $XALT_FUNCTION_TRACKING | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $XALT_FUNCTION_TRACKING "--with-functionTracking=ans: ans must be yes or no"
    AC_MSG_RESULT([XALT_FUNCTION_TRACKING=$with_functionTracking])
    AC_DEFINE_UNQUOTED(XALT_FUNCTION_TRACKING, "$with_functionTracking",[Track functions from modules])dnl
    ,
    withval="yes"
    AC_MSG_RESULT([XALT_FUNCTION_TRACKING=$withval])
    XALT_FUNCTION_TRACKING="$withval"
    AC_DEFINE_UNQUOTED(XALT_FUNCTION_TRACKING, "$withval",[Track functions from modules]))dnl

AC_SUBST(XALT_FILE_PREFIX)
AC_ARG_WITH(xaltFilePrefix,
    AS_HELP_STRING([--with-xaltFilePrefix=ans],[Prefix where the json files are written [[$HOME]]]),
    XALT_FILE_PREFIX="$withval"
    AC_MSG_RESULT([XALT_FILE_PREFIX=$with_xaltFilePrefix])
    AC_DEFINE_UNQUOTED(XALT_FILE_PREFIX, "$with_xaltFilePrefix",[Prefix where the json files are written])dnl
    ,
    withval="USE_HOME"
    AC_MSG_RESULT([XALT_FILE_PREFIX=$withval])
    XALT_FILE_PREFIX="$withval"
    AC_DEFINE_UNQUOTED(XALT_FILE_PREFIX, "$withval",[Prefix where the json files are written]))dnl

AC_SUBST(XALT_GPU_TRACKING_STYLE)
AC_ARG_WITH(trackGPU,
    AS_HELP_STRING([--with-trackGPU=ans],[Track GPU executables {yes(nvml),no,dcgm,nvml}, [[no]]]),
    XALT_GPU_TRACKING_STYLE="$withval"
    XALT_GPU_TRACKING_STYLE=`echo $XALT_GPU_TRACKING_STYLE | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    if test $XALT_GPU_TRACKING_STYLE = yes; then
       XALT_GPU_TRACKING_STYLE=nvml
    fi
    AC_MSG_RESULT([XALT_GPU_TRACKING_STYLE=$XALT_GPU_TRACKING_STYLE])
    AC_DEFINE_UNQUOTED(XALT_GPU_TRACKING_STYLE, "$with_trackGPU",[Track GPU executables])dnl
    ,
    withval="no"
    AC_MSG_RESULT([XALT_GPU_TRACKING_STYLE=$withval])
    XALT_GPU_TRACKING_STYLE="$withval"
    AC_DEFINE_UNQUOTED(XALT_GPU_TRACKING_STYLE, "$withval",[Track GPU executables]))dnl

AC_SUBST(MODE)
AC_ARG_WITH(mode,
    AS_HELP_STRING([--with-mode=ans],[Override executable install mode (755 or 750 or 700) [[user_default]]]),
    MODE="$withval"
    MODE=`echo $MODE | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    AC_MSG_RESULT([MODE=$with_mode])
    AC_DEFINE_UNQUOTED(MODE, "$with_mode",[Override executable install mode])dnl
    ,
    withval="user_default"
    AC_MSG_RESULT([MODE=$withval],[Override executable install mode])
    MODE="$withval"
    AC_DEFINE_UNQUOTED(MODE, "$withval"))dnl

AC_SUBST(XALT_GPU_TRACKING)
if test $XALT_GPU_TRACKING_STYLE = nvml -o $XALT_GPU_TRACKING_STYLE = dcgm ; then
   XALT_GPU_TRACKING=yes
else
   XALT_GPU_TRACKING=no
fi	


AC_SUBST(XALT_MPI_TRACKING)
AC_ARG_WITH(trackMPI,
    AS_HELP_STRING([--with-trackMPI=ans],[Track MPI executables, [[yes]]]),
    XALT_MPI_TRACKING="$withval"
    XALT_MPI_TRACKING=`echo $XALT_MPI_TRACKING | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $XALT_MPI_TRACKING "--with-trackMPI=ans: ans must be yes or no"
    AC_MSG_RESULT([XALT_MPI_TRACKING=$with_trackMPI])
    AC_DEFINE_UNQUOTED(XALT_MPI_TRACKING, "$with_trackMPI",[Track MPI executables])dnl
    ,
    withval="yes"
    AC_MSG_RESULT([XALT_MPI_TRACKING=$withval])
    XALT_MPI_TRACKING="$withval"
    AC_DEFINE_UNQUOTED(XALT_MPI_TRACKING, "$withval",[Track MPI executables]))dnl

AC_SUBST(XALT_SCALAR_TRACKING)
AC_ARG_WITH(trackScalarPrgms,
    AS_HELP_STRING([--with-trackScalarPrgms=ans],[Track non-mpi executables[[yes]]]),
    XALT_SCALAR_TRACKING="$withval"
    XALT_SCALAR_TRACKING=`echo $XALT_SCALAR_TRACKING | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $XALT_SCALAR_TRACKING "--with-trackScalarPrgms=ans: ans must be yes or no"
    AC_MSG_RESULT([XALT_SCALAR_TRACKING=$with_trackScalarPrgms])
    AC_DEFINE_UNQUOTED(XALT_SCALAR_TRACKING, "$with_trackScalarPrgms",[Track non-mpi executables])dnl
    ,
    withval="yes"
    AC_MSG_RESULT([XALT_SCALAR_TRACKING=$withval])
    XALT_SCALAR_TRACKING="$withval"
    AC_DEFINE_UNQUOTED(XALT_SCALAR_TRACKING, "$withval",[Track non-mpi executables]))dnl

AC_SUBST(XALT_SUPPORT_CURL)
AC_ARG_WITH(supportCURL,
    AS_HELP_STRING([--with-supportCURL],[Support CURL transmission style [[yes]]]),
    XALT_SUPPORT_CURL="$withval"
    XALT_SUPPORT_CURL=`echo $XALT_SUPPORT_CURL | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $XALT_SUPPORT_CURL "--with-supportCURL=ans: ans must be yes or no"
    AC_MSG_RESULT([XALT_SUPPORT_CURL=$with_supportCURL])
    AC_DEFINE_UNQUOTED(XALT_SUPPORT_CURL, "$with_supportCURL",[Track non-mpi executables])dnl
    ,
    withval="yes"
    AC_MSG_RESULT([XALT_SUPPORT_CURL=$withval])
    XALT_SUPPORT_CURL="$withval"
    AC_DEFINE_UNQUOTED(XALT_SUPPORT_CURL, "$withval",[Track non-mpi executables]))dnl


AC_SUBST(XALT_CMDLINE_RECORD)
AC_ARG_WITH(cmdlineRecord,
    AS_HELP_STRING([--with-cmdlineRecord=ans],[Record the program's execution command line[[yes]]]),
    XALT_CMDLINE_RECORD="$withval"
    XALT_CMDLINE_RECORD=`echo $XALT_CMDLINE_RECORD | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $XALT_CMDLINE_RECORD "--with-cmdlineRecord=ans: ans must be yes or no"
    AC_MSG_RESULT([XALT_CMDLINE_RECORD=$with_cmdlineRecord])
    AC_DEFINE_UNQUOTED(XALT_CMDLINE_RECORD, "$with_cmdlineRecord",[Record the program's execution command line])dnl
    ,
    withval="yes"
    AC_MSG_RESULT([XALT_CMDLINE_RECORD=$withval])
    XALT_CMDLINE_RECORD="$withval"
    AC_DEFINE_UNQUOTED(XALT_CMDLINE_RECORD, "$withval",[Record the program's execution command line]))dnl

AC_SUBST(COMPUTE_SHA1SUM)
AC_ARG_WITH(computeSHA1,
    AS_HELP_STRING([--with-computeSHA1=ans],[compute SHA1 sums on libraries, [[no]]]),
    COMPUTE_SHA1SUM="$withval"
    COMPUTE_SHA1SUM=`echo $COMPUTE_SHA1SUM | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $COMPUTE_SHA1SUM "--with-computeSHA1=ans: ans must be yes or no"
    AC_MSG_RESULT([COMPUTE_SHA1SUM=$with_computeSHA1])
    AC_DEFINE_UNQUOTED(COMPUTE_SHA1SUM, "$with_computeSHA1",,[Compute SHAs sums on libraries])dnl
    ,
    withval="no"
    AC_MSG_RESULT([COMPUTE_SHA1SUM=$withval])
    COMPUTE_SHA1SUM="$withval"
    AC_DEFINE_UNQUOTED(COMPUTE_SHA1SUM, "$withval",[Compute SHAs sums on libraries]))dnl

AC_SUBST(ETC_DIR)
AC_ARG_WITH(etcDir,
    AS_HELP_STRING([--with-etcDir=ans],[Directory where xalt_db.conf and reverseMapD can be found [[.]]]),
    ETC_DIR="$withval"
    AC_MSG_RESULT([ETC_DIR=$with_etcDir])
    AC_DEFINE_UNQUOTED(ETC_DIR, "$with_etcDir",[Directory where xalt_db.conf and reverseMapD can be found])dnl
    ,
    withval="."
    AC_MSG_RESULT([ETC_DIR=$withval])
    ETC_DIR="$withval"
    AC_DEFINE_UNQUOTED(ETC_DIR, "$withval",[Directory where xalt_db.conf and reverseMapD can be found]))dnl

AC_SUBST(XALT_CONFIG_PY)
AC_ARG_WITH(config,
    AS_HELP_STRING([--with-config=ans],[A python file defining the accept, ignore, hostname pattern lists [[NO DEFAULT]]]),
    XALT_CONFIG_PY="$withval"
    AC_MSG_RESULT([XALT_CONFIG_PY=$with_config])
    AC_DEFINE_UNQUOTED(XALT_CONFIG_PY, "$with_config",[A python file defining the site configuration])dnl
    ,
    withval="no"
    AC_MSG_RESULT([XALT_CONFIG_PY=$withval])
    XALT_CONFIG_PY="$withval"
    AC_DEFINE_UNQUOTED(XALT_CONFIG_PY, "$withval",[A python file defining the site configuration]))dnl

AC_SUBST(MYSQLDB)
AC_ARG_WITH(MySQL,
    AS_HELP_STRING([--with-MySQL=ans],[Require the mysqlclient/MySQL-python module [[yes]]]),
    MYSQLDB="$withval"
    MYSQLDB=`echo $MYSQLDB | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $MYSQLDB "--with-MySQL=ans: ans must be yes or no"
    AC_MSG_RESULT([MYSQLDB=$with_MySQL])
    AC_DEFINE_UNQUOTED(MYSQLDB, "$with_MySQL",[Require the mysqlclient/MySQL-python module])dnl
    ,
    withval="yes"
    AC_MSG_RESULT([MYSQLDB=$withval])
    MYSQLDB="$withval"
    AC_DEFINE_UNQUOTED(MYSQLDB, "$withval",[Require the mysqlclient/MySQL-python module]))dnl

AC_SUBST(SYSTEM_PATH)
AC_ARG_WITH(systemPath,
    AS_HELP_STRING([--with-systemPath=[ans]],[]),
    SYSTEM_PATH="$withval"
    AC_MSG_RESULT([SYSTEM_PATH=$with_systemPath])
    AC_DEFINE_UNQUOTED(SYSTEM_PATH, "$with_systemPath",[The system path that XALT will use (typically /usr/bin:/bin)])dnl
    ,
    withval="/usr/bin:/bin"
    AC_MSG_RESULT([SYSTEM_PATH=$withval])
    SYSTEM_PATH="$withval"
    AC_DEFINE_UNQUOTED(SYSTEM_PATH, "$withval",[The system path that XALT will use (typically /usr/bin:/bin)]))dnl

AC_SUBST(SYSLOG_MSG_SZ)
AC_ARG_WITH(syslogMsgSz,
    AS_HELP_STRING([--with-syslogMsgSz=ans],[]),
    SYSLOG_MSG_SZ="$withval"
    AC_MSG_RESULT([SYSLOG_MSG_SZ=$with_syslogMsgSz])dnl
    ,
    withval="512"
    AC_MSG_RESULT([SYSLOG_MSG_SZ=$withval])
    SYSLOG_MSG_SZ="$withval")dnl

AC_SUBST(SYSHOST_CONFIG)
AC_ARG_WITH(syshostConfig,
    AS_HELP_STRING([--with-syshostConfig=ans],[How to determine syshost[[no default]]]),
    SYSHOST_CONFIG="$withval"
    AC_MSG_RESULT([SYSHOST_CONFIG=$with_syshostConfig])
    AC_DEFINE_UNQUOTED(SYSHOST_CONFIG, "$with_syshostConfig",[How to determine syshost])dnl
    ,
    withval="no"
    AC_MSG_RESULT([SYSHOST_CONFIG=$withval])
    SYSHOST_CONFIG="$withval"
    AC_DEFINE_UNQUOTED(SYSHOST_CONFIG, "$withval",[How to determine syshost]))dnl

AC_SUBST(MY_HOSTNAME_PARSER)
AC_ARG_WITH(hostnameParser,
    AS_HELP_STRING([--with-hostnameParser=ans],[Replace built-in lex based hostname parser c_file:file.c or library:file_64.a or library:file_64.a:file_32.a [[no]]]),
    MY_HOSTNAME_PARSER="$withval"
    AC_MSG_RESULT([MY_HOSTNAME_PARSER=$with_hostnameParser])
    AC_DEFINE_UNQUOTED(MY_HOSTNAME_PARSER, "$with_hostnameParser",[Replace built-in lex based hostname parser])dnl
    ,
    withval="no"
    AC_MSG_RESULT([MY_HOSTNAME_PARSER=$withval])
    MY_HOSTNAME_PARSER="$withval"
    AC_DEFINE_UNQUOTED(MY_HOSTNAME_PARSER, "$withval",[Replace built-in lex based hostname parser]))dnl

AC_SUBST(STATIC_LIBS)
AC_ARG_WITH(staticLibs,
    AS_HELP_STRING([--with-staticLibs=ans],[Link with static libraries (currently only dcgm) [[no]]]),
    STATIC_LIBS="$withval"
    STATIC_LIBS=`echo $STATIC_LIBS | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $STATIC_LIBS "--with-staticLibs=ans: ans must be yes or no"
    AC_MSG_RESULT([STATIC_LIBS=$with_staticLibs])
    AC_DEFINE_UNQUOTED(STATIC_LIBS, "$with_staticLibs",[Link with static libraries (currently only dcgm)])dnl
    ,
    withval="no"
    AC_MSG_RESULT([STATIC_LIBS=$withval])
    STATIC_LIBS="$withval"
    AC_DEFINE_UNQUOTED(STATIC_LIBS, "$withval",[Link with static libraries (currently only dcgm)]))dnl

AC_SUBST(PRELOAD_ONLY)
AC_ARG_WITH(preloadOnly,
    AS_HELP_STRING([--with-preloadOnly=ans],[Only use XALT in preload only mode[[yes]]]),
    PRELOAD_ONLY="$withval"
    PRELOAD_ONLY=`echo $PRELOAD_ONLY | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $PRELOAD_ONLY "--with-preloadOnly=ans: ans must be yes or no"
    AC_MSG_RESULT([PRELOAD_ONLY=$with_preloadOnly])
    AC_DEFINE_UNQUOTED(PRELOAD_ONLY, "$with_preloadOnly",[Only use XALT in preload only mode])dnl
    ,
    withval="yes"
    AC_MSG_RESULT([PRELOAD_ONLY=$withval])
    PRELOAD_ONLY="$withval"
    AC_DEFINE_UNQUOTED(PRELOAD_ONLY, "$withval",[Only use XALT in preload only mode]))dnl

AC_SUBST(SITE_CONTROLLED_PREFIX)
AC_ARG_WITH(siteControlledPrefix,
    AS_HELP_STRING([--with-siteControlledPrefix=ans],[A site controlled prefix, [[no]]]),
    SITE_CONTROLLED_PREFIX="$withval"
    SITE_CONTROLLED_PREFIX=`echo $SITE_CONTROLLED_PREFIX | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $SITE_CONTROLLED_PREFIX "--with-siteControlledPrefix=ans: ans must be yes or no"
    AC_MSG_RESULT([SITE_CONTROLLED_PREFIX=$with_siteControlledPrefix])
    AC_DEFINE_UNQUOTED(SITE_CONTROLLED_PREFIX, "$with_siteControlledPrefix",[A site controlled prefix.])dnl
    ,
    withval="no"
    AC_MSG_RESULT([SITE_CONTROLLED_PREFIX=$withval])
    SITE_CONTROLLED_PREFIX="$withval"
    AC_DEFINE_UNQUOTED(SITE_CONTROLLED_PREFIX, "$withval",[A site that insist to install XALT incorrectly]))dnl

AC_SUBST(BIT32)
AC_ARG_WITH(32bit,
    AS_HELP_STRING([--with-32bit=ans],[Allow for 32 bits executables[[yes]]]),
    BIT32="$withval"
    BIT32=`echo $BIT32 | tr '@<:@:upper:@:>@' '@<:@:lower:@:>@'`
    VALID_YN $BIT32 "--with-32bit=ans: ans must be yes or no"
    AC_MSG_RESULT([BIT32=$with_32bit])
    AC_DEFINE_UNQUOTED(BIT32, "$with_32bit",[Build 32bit libraries as well])dnl
    ,
    withval="yes"
    AC_MSG_RESULT([BIT32=$withval])
    BIT32="$withval"
    AC_DEFINE_UNQUOTED(BIT32, "$withval",[Build 32bit libraries as well]))dnl

AC_SUBST(XALT_TMPDIR)
AC_ARG_WITH(tmpdir,
    AS_HELP_STRING([--with-tmpdir=ans],[The tmp directory to use with package records [[/dev/shm]]]),
    XALT_TMPDIR="$withval"
    AC_MSG_RESULT([XALT_TMPDIR=$with-tmpdir])
    AC_DEFINE_UNQUOTED(XALT_TMPDIR, "$with-tmpdir",[Allow xalt to write to /dev/shm])dnl
    ,
    withval="/dev/shm"
    AC_MSG_RESULT([XALT_TMPDIR=$withval])
    XALT_TMPDIR="$withval"
    AC_DEFINE_UNQUOTED(XALT_TMPDIR, "$withval"),[Allow xalt to write to /dev/shm])dnl

AC_SUBST(XALT_PRIME_NUMBER)
AC_ARG_WITH(primeNumber,
    AS_HELP_STRING([--with-primeNumber=ans],[The prime number of directories when using XALT_FILE_PREFIX, [[997]]]),
    XALT_PRIME_NUMBER="$withval"
    AC_MSG_RESULT([XALT_PRIME_NUMBER=$with-primeNumber])dnl
    ,
    withval="997"
    AC_MSG_RESULT([XALT_PRIME_NUMBER=$withval])
    XALT_PRIME_NUMBER="$withval")dnl

AC_SUBST(XALT_SIGNAL_HANDLER)
AC_ARG_WITH(signalHandler,
    AS_HELP_STRING([--with-signalHandler=ans],[Have XALT capture executions that fail with a signal: [[no]]]),
    XALT_SIGNAL_HANDLER="$withval"
    AC_MSG_RESULT([XALT_SIGNAL_HANDLER=$with-signalHandler])
    AC_DEFINE_UNQUOTED(XALT_SIGNAL_HANDLER, "$with-signalHandler",[Allow XALT to interact with Signals])dnl
    ,
    withval="no"
    AC_MSG_RESULT([XALT_SIGNAL_HANDLER=$withval])
    XALT_SIGNAL_HANDLER="$withval"
    AC_DEFINE_UNQUOTED(XALT_SIGNAL_HANDLER, "$withval",[Allow XALT to interact with Signals]))dnl

AC_SUBST(XALT_PRIME_FMT)
AC_SUBST(HAVE_32BIT)

if test "$BIT32" = yes ; then
  AX_TEST_COMPILE_FLAG([-m32],[HAVE_32BIT=yes],[HAVE_32BIT=no])
else
  HAVE_32BIT=no
fi

AC_LANG_PUSH(C++)
AX_TEST_COMPILE_FLAG([-std=c++11],[HAVE_CXX11=yes],[HAVE_CXX11=no])
AC_LANG_POP([C++])

AC_SUBST(PATH_TO_GIT)
AC_SUBST(PATH_TO_FLEX)
AC_SUBST(PATH_TO_LOGGER)
AC_SUBST(PATH_TO_PSTREE)

AC_SUBST(PATH_TO_UUIDGEN)

AC_SUBST(USE_CONTRIB_ARGPARSE)

if test $MODE != "user_default" ; then
  mode=0$MODE
  EXEC_BITS=$(( 0111 & $mode ));
  if test $EXEC_BITS = 0; then
     MSG="Error: MODE must have at least one executable bit set (Not MODE=$MODE)|Please try setting mode to 755 or 750 or 700 or similar."
     ERR_MSG="$ERR_MSG||$MSG"
     echo
     echo $MSG
     echo
     rm -f makefile
     exit 1
  fi
fi


AC_PATH_PROG(PATH_TO_GIT,git, [])
AC_PATH_PROG(PATH_TO_FLEX,flex, [])
AC_PATH_PROG(PATH_TO_LOGGER,logger, [])
AC_PATH_PROG(PATH_TO_MKTEMP, mktemp, [])
AC_PATH_PROGS(PATH_TO_PYTHON, [python3] [python] [python2], [])
AC_MSG_RESULT([PATH_TO_PYTHON: $PATH_TO_PYTHON])
if test "x$PATH_TO_PYTHON" = "x"; then
   MSG="Error: XALT requires python3, python2 or python to run."
   ERR_MSG="$ERR_MSG||$MSG"
   rm -f makefile
   exit 1
fi

AC_SUBST(XALT_LD_LIBRARY_PATH)

AC_SUBST(XALT_INSTALL_OS)
XALT_INSTALL_OS=$($ac_confdir/sh_src/xalt_print_os.in)
AC_DEFINE_UNQUOTED(XALT_INSTALL_OS,"$XALT_INSTALL_OS",[Name of the OS that installed XALT])


if test "x${PATH_TO_MKTEMP}" = "x" ; then
  MSG="Unable to build XALT without mktemp"
  ERR_MSG="$ERR_MSG||$MSG"
  FOUND_ERROR=yes
  AC_MSG_NOTICE([$MSG])
fi


transmission=`echo $TRANSMISSION | tr A-Z a-z`

found=no
for i in file file_separate_dirs syslog logger none; do
   if test $transmission = $i ; then
      found=yes
      break
   fi
done

if test found = no; then
   MSG="XALT does not support transmission style: $TRANSMISSION|See the xalt.readthedocs.io for supported methods"
   ERR_MSG="$ERR_MSG||$MSG"
   rm -f makefile
   exit 1
fi

XALT_UUID_V7=no
AC_SUBST(XALT_UUID_V7)
AC_CHECK_LIB([c], [getentropy], [XALT_UUID_V7=yes; AC_DEFINE([HAVE_FUNC_GETENTROPY],1,
                   [Define to 1 if you have getentropy in libc])],[])

AC_SUBST(HAVE_PYMOD_MYSQLDB)
AX_PYTHON_MODULE([$PATH_TO_PYTHON],[MySQLdb])

AC_CHECK_HEADER(uuid/uuid.h,
                [AC_DEFINE([HAVE_UUID_UUID_H], 1,
                   [Define to 1 if you have <uuid/uuid.h>.])],
                [FOUND_ERROR=yes;
		 MSG="Unable to build XALT without uuid/uuid.h"
		 ERR_MSG="$ERR_MSG||$MSG"
		 AC_MSG_NOTICE([$MSG])])

if test $XALT_SUPPORT_CURL = yes ; then
  AC_CHECK_HEADER(curl/curl.h,
                  [AC_DEFINE([HAVE_CURL_CURL_H], 1,
                     [Define to 1 if you have <curl/curl.h>.])],
                  [FOUND_ERROR=yes;
		  MSG="Configure with --supportCURL=no if you don't want support for curl transmission style"
  		  ERR_MSG="$ERR_MSG||$MSG"
		  AC_MSG_NOTICE([$MSG])])
fi

AC_CHECK_HEADER(openssl/sha.h,
                [AC_DEFINE([HAVE_OPENSSL_SHA_H], 1,
                   [Define to 1 if you have <openssl/sha.h>.])],
                [FOUND_ERROR=yes;
		MSG="Unable to build XALT without openssl/sha.h"
                ERR_MSG="$ERR_MSG||$MSG"
		AC_MSG_NOTICE([$MSG])])

AC_CHECK_HEADER(gelf.h, [],
                [FOUND_ERROR=yes;
		MSG="Unable to build XALT without gelf.h. Please install the libelf development package"
                ERR_MSG="$ERR_MSG||$MSG"
		AC_MSG_NOTICE([$MSG])])
AC_CHECK_HEADER(syslog.h, [],
                [FOUND_ERROR=yes;
		MSG="Unable to build XALT without syslog.h. Please install the glibc headers package"
                ERR_MSG="$ERR_MSG||$MSG"
		AC_MSG_NOTICE([$MSG])])

AC_SUBST(HAVE_WORKING_LIBUUID)
HAVE_WORKING_LIBUUID=no
if test $ac_cv_header_uuid_uuid_h = yes; then
  AC_SEARCH_LIBS([uuid_generate],[uuid], [AC_DEFINE(HAVE_LIBUUID,1, [libuuid exist]) HAVE_WORKING_LIBUUID=yes], [])
fi

if test $HAVE_WORKING_LIBUUID = no -a ! -f /proc/sys/kernel/random/uuid; then
   FOUND_ERROR=yes;
   MSG="Unable to install XALT without uuid and without /proc/sys/kernel/random/uuid|See the manual for details"
   ERR_MSG="$ERR_MSG||$MSG"
   rm -f makefile
fi


if test -z "$PATH_TO_FLEX" ; then
   FOUND_ERROR=yes;
   MSG="Unable to install XALT without flex|See the xalt.readthedocs.io for details"
   ERR_MSG="$ERR_MSG||$MSG"
   rm -f makefile
fi

MY_TMPDIR=`mktemp -d`
LOCATE_SO=$MY_TMPDIR/locate_shared_library

gcc -o $LOCATE_SO $ac_confdir/proj_mgmt/locate_shared_library.c -ldl

AC_SUBST(GPU_STR)
GPU_STR=
AC_SUBST(HAVE_NVML)
HAVE_NVML=no
AC_SUBST(HAVE_DCGM)
HAVE_DCGM=no
if test $XALT_GPU_TRACKING_STYLE = "dcgm" ; then
  AC_CHECK_HEADER(dcgm_agent.h,
                  [AC_DEFINE([HAVE_DCGM_AGENT_H], 1,
                     [Define to 1 if you have DCGM.])],
                  [FOUND_ERROR=yes; 
		  MSG="Unable to include GPU tracking without DCGM"
		  ERR_MSG="$ERR_MSG||$MSG"
		  AC_MSG_NOTICE([$MSG])])
  if test "$STATIC_LIBS" = yes; then
    AC_SEARCH_LIBS([dcgmInit], [dcgm_stub], [AC_DEFINE(USE_DCGM, [1],[Using DCGM]) HAVE_DCGM=yes], [FOUND_ERROR=yes;
                                                                                                   MSG="Unable to include GPU tracking without DCGM"
                                                                                                   ERR_MSG="$ERR_MSG||$MSG"												   
    			                                                                           AC_MSG_NOTICE([$MSG])], [-ldl])
  else
    AC_SEARCH_LIBS([dcgmInit], [dcgm], [AC_DEFINE(USE_DCGM, [1],[Using DCGM]) HAVE_DCGM=yes], [FOUND_ERROR=yes;
                                                                                              MSG="Unable to include GPU tracking without DCGM"
                                                                                              ERR_MSG="$ERR_MSG||$MSG"												   
                                                                                              AC_MSG_NOTICE([$MSG])])
  fi
  for i in ' ' .9 .8 .7 .6 .5 .4 .3 .2 .1; do
    GPU_STR=`$LOCATE_SO libdcgm.so$i`
    if test "$?" -eq 0; then
      break;
    fi
  done
elif test $XALT_GPU_TRACKING_STYLE = "nvml" ; then
  AC_CHECK_HEADER(nvml.h,
                  [AC_DEFINE([HAVE_NVML_H], 1,
                     [Define to 1 if you have NVML.])],
                  [FOUND_ERROR=yes;
		  MSG="Unable to include GPU tracking without NVML"
		  ERR_MSG="$ERR_MSG||$MSG"
 		  AC_MSG_NOTICE([$MSG])])
  AC_CHECK_HEADER(dlfcn.h,
                  [AC_DEFINE([HAVE_DCFCN_H], 1,
                     [Define to 1 if you have dlfcn.h.])],
                  [FOUND_ERROR=yes;
		  MSG="Unable to include GPU tracking without dlfcn.h"
		  ERR_MSG="$ERR_MSG||$MSG"
		  AC_MSG_NOTICE([$MSG])])
  AC_SEARCH_LIBS([dlopen], [dl dld],
                 [AC_DEFINE([HAVE_DLOPEN], [1],[Using dlopen library])
                  AC_DEFINE([USE_NVML], [1],[Using NVML library])
                  HAVE_NVML=yes],
                 [FOUND_ERROR=yes;
		 MSG="Unable to find dlopen"
		 ERR_MSG="$ERR_MSG||$MSG"
		 AC_MSG_NOTICE([$MSG])])

  for i in ' ' .9 .8 .7 .6 .5 .4 .3 .2 .1; do
    GPU_STR=`$LOCATE_SO libnvidia-ml.so$i`
    if test "$?" -eq 0; then
      break;
    fi
  done
fi

AC_SUBST(PKGV)
AC_SUBST(XALT_DEFAULT_DIR)
AC_SUBST(VERSION)
AC_SUBST(XALT_GIT_VERSION)

VERSION=$(cat $ac_confdir/.version | sed -e 's/-devel//g')
PKGV=$prefix/xalt/$VERSION
XALT_DEFAULT_DIR=$prefix/xalt/xalt

if test $SITE_CONTROLLED_PREFIX != yes; then
   if echo $prefix | grep -i xalt/$VERSION > /dev/null ; then
      echo
      echo "===================================================="
      echo "A site can include the version in the prefix, but"
      echo "must be aware of the issues with this change. If"
      echo "your site have static binaries build with older"
      echo "version of XALT, they will silently NOT be recorded."
      echo 
      echo "Also sites must define the "XALT_DIR" environment"
      echo "variable correctly."
      echo
      echo "If your site wishes to install XALT and control "
      echo "the prefix then add: "
      echo
      echo "   --with-siteControlledPrefix=yes"
      echo
      echo "to your configure line."
      echo "===================================================="
      echo
      rm -f makefile
      exit 1
   fi
   
else
  PKGV=$prefix
  XALT_DEFAULT_DIR=$prefix
fi

XALT_LD_LIBRARY_PATH=$XALT_DEFAULT_DIR/lib64

echo "VERSION=$VERSION"
if test -n "$PATH_TO_GIT" -a -e $srcdir/.git ; then
  xaltV=`(cd $srcdir; $PATH_TO_GIT describe)`
else
  xaltV="@git@"
fi
XALT_GIT_VERSION=$xaltV
AC_MSG_RESULT([XALT_GIT_VERSION=$XALT_GIT_VERSION])


if test "$HAVE_CXX11" = no; then
   echo
   echo "======================================================="
   echo "Unable to install XALT without g++ supporting -std=c++11"
   echo "See the xalt.readthedocs.io for details"
   echo "======================================================="
   rm -f makefile
   exit 1
fi

if test "$XALT_CONFIG_PY" = no; then
   echo
   echo "======================================================="
   echo "Unable to install XALT without setting --with-config=..."
   echo "See the xalt.readthedocs.io for details"
   echo "======================================================="
   rm -f makefile
   exit 1   
fi

if ! test -f $XALT_CONFIG_PY; then
   echo
   echo "======================================================="
   echo "Unable to find the file: $XALT_CONFIG_PY"
   echo "quitting!"
   echo "======================================================="
   rm -f makefile
   exit 1   
fi

if test "$MYSQLDB" = yes; then
   if test "$HAVE_PYMOD_MYSQLDB" = no; then
      echo
      echo "======================================================="
      echo "Unable to install XALT without the python module: MySQLdb"
      echo "Install a system package (debian based linux: python3-mysqldb) "
      echo "To install for python3 do: pip3 install mysqlclient"
      echo "======================================================="
      rm -f makefile
      exit 1
   fi
fi

if test "$SYSHOST_CONFIG" = no; then
   echo
   echo "======================================================="
   echo "Unable to install XALT without setting --with-syshostConfig=..."
   echo "Your choices are hardcode, nth_name, read_file, mapping,"
   echo "strip_nodename_numbers and env_var."
   echo "See the xalt.readthedocs.io for details."
   echo "======================================================="
   rm -f makefile
   exit 1
fi


printf "Use contrib argparse"
USE_CONTRIB_ARGPARSE=no
versionStr=$($PATH_TO_PYTHON --version 2>&1 | head -1)
major_version=`expr "$versionStr" : 'Python \+\([[0-9]]\+\)\.'`
minor_version=`expr "$versionStr" : 'Python \+[[0-9]]\+\.\([[0-9]]\+\)'`
version=`expr $major_version \* 100000 + $minor_version`
contrib_version=200007
if test $version -lt $contrib_version ; then
   USE_CONTRIB_ARGPARSE=yes
fi
echo "... $USE_CONTRIB_ARGPARSE"

if ! test -d src/__build__; then
   mkdir -p src/__build__
fi

if test "$USE_CONTRIB_ARGPARSE" = yes; then
   if test -z "$PYTHONPATH"; then
      export PYTHONPATH=$ac_confdir/contrib/argparse
   else
      export PYTHONPATH=$ac_confdir/contrib/argparse:$PYTHONPATH
   fi
fi

AC_SEARCH_LIBS([SHA1], [crypto], [],
		       [FOUND_ERROR=yes; 
		       MSG="Unable to build XALT without libcrypto"
		       ERR_MSG="$ERR_MSG||$MSG"
                       AC_MSG_NOTICE([$MSG])])

if test $XALT_SUPPORT_CURL = yes; then
  AC_SEARCH_LIBS([curl_easy_setopt], [curl], [], [FOUND_ERROR=yes;
                                                 MSG="Unable to build XALT without libcurl"
                                                 ERR_MSG="$ERR_MSG||$MSG"
                                                 AC_MSG_NOTICE([$MSG])])
fi

AC_SUBST(CRYPTO_STR)
CRYPTO_STR=`$LOCATE_SO libcrypto.so`

AC_SUBST(CURL_STR)
CURL_STR=
if test $XALT_SUPPORT_CURL = yes; then
  for i in ' ' .9 .8 .7 .6 .5 .4 .3 .2 .1; do
    CURL_STR=`$LOCATE_SO libcurl.so$i`
    if test "$?" -eq 0; then
      break;
    fi
  done
fi

AC_SUBST(UUID_STR)
for i in ' ' .9 .8 .7 .6 .5 .4 .3 .2 .1; do
  UUID_STR=`$LOCATE_SO libuuid.so$i`
  if test "$?" -eq 0; then
    break;
  fi
done


$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/xalt_prime_check.py $XALT_PRIME_NUMBER
if test "$?" != 0; then
   echo
   echo "======================================================="
   echo "Unable to install XALT when --with-primeNumber=<ans>"
   echo "is not a prime number.  It also has to be bigger than 3."
   echo "See the xalt.readthedocs.io for details."
   echo "======================================================="
   rm -f makefile
   exit 1
fi
   
XALT_PRIME_FMT=$($PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/xalt_prime_fmt.py $XALT_PRIME_NUMBER)
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi

$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_xalt_interval_table.py --confFn $XALT_CONFIG_PY --input $ac_confdir/src/tmpl/xalt_interval.template --output src/__build__/xalt_interval.h 
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi

$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_xalt_regex.py --confFn $XALT_CONFIG_PY --xalt_cfg $ac_confdir/src/tmpl/xalt_config.py --input $ac_confdir/src/tmpl/xalt_regex.template --default_dir $XALT_DEFAULT_DIR --output src/__build__/xalt_regex.h
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	


$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_syshost_routine.py --input $SYSHOST_CONFIG --output src/__build__/xalt_syshost.c 
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_parser_routine.py --confFn $XALT_CONFIG_PY --xalt_cfg $ac_confdir/src/tmpl/xalt_config.py --input $ac_confdir/src/tmpl/xalt_env_parser.template --output src/__build__/xalt_env_parser.lex 
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_parser_routine.py --confFn $XALT_CONFIG_PY --xalt_cfg $ac_confdir/src/tmpl/xalt_config.py --input $ac_confdir/src/tmpl/xalt_pre_ingest_filter.template --output src/__build__/xalt_pre_ingest_filter.lex
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_parser_routine.py --confFn $XALT_CONFIG_PY --xalt_cfg $ac_confdir/src/tmpl/xalt_config.py --input $ac_confdir/src/tmpl/xalt_path_parser.template --default_dir $XALT_DEFAULT_DIR --output src/__build__/xalt_path_parser.lex
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_parser_routine.py --confFn $XALT_CONFIG_PY --xalt_cfg $ac_confdir/src/tmpl/xalt_config.py --input $ac_confdir/src/tmpl/xalt_track_executable_parser.template --default_dir $XALT_DEFAULT_DIR --output src/__build__/xalt_track_executable_parser.lex
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_parser_routine.py --confFn $XALT_CONFIG_PY --xalt_cfg $ac_confdir/src/tmpl/xalt_config.py --input $ac_confdir/src/tmpl/xalt_hostname_parser.template --output src/__build__/xalt_hostname_parser.lex 
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_python_filter_routine.py --confFn $XALT_CONFIG_PY --xalt_cfg $ac_confdir/src/tmpl/xalt_config.py --input $ac_confdir/src/tmpl/xalt_python_pkg_filter.template --output src/__build__/xalt_python_pkg_filter.py
if test "$?" != 0; then
   rm -f makefile			
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/proj_mgmt/py_build_tools/build_parser_routine.py --confFn $XALT_CONFIG_PY --xalt_cfg $ac_confdir/src/tmpl/xalt_config.py --input $ac_confdir/src/tmpl/xalt_pkg_filter.template --output src/__build__/xalt_pkg_filter.lex
if test "$?" != 0; then
   rm -f makefile			
   exit 1
fi	


#########################################################################
## check to see if the following programs can be found from SYSTEM_PATH:
#
ORIG_SEP=$IFS
IFS=':'
NOT_FOUND=""
for cmd in as basename bc cat date dirname file grep head ldd logger mkdir objdump readlink rm sha1sum sed; do
   found=""
   for dir in $SYSTEM_PATH; do
     if test -f $dir/$cmd && test -x $dir/$cmd; then
       found=1
       break
     fi
   done
   if test -z "$found" ; then
     NOT_FOUND="$cmd $NOT_FOUND"
   fi
done
   
IFS=$ORIG_SEP

if test -n "$NOT_FOUND"; then
  FOUND_ERROR=yes
  MSG="Unable to build XALT without SYSTEM_PATH=$SYSTEM_PATH including $NOT_FOUND"
  ERR_MSG="$ERR_MSG||$MSG"
  AC_MSG_NOTICE([$MSG])
fi  


if test "$FOUND_ERROR" = yes; then
  echo
  echo One or more errors found:
  echo
  echo Error Messages:
  echo "------------------------------------------------------------"
  echo $ERR_MSG | tr '|' '\n' | while read item; do
       echo $item
  done   
  echo "------------------------------------------------------------"
  
  rm -f makefile
  exit 1;
fi      

rm -rf $MY_TMPDIR

AC_CONFIG_FILES([makefile:Makefile.in                                       \
          src/__build__/xalt_config.h:src/util/xalt_config.h.in      \
          src/makefile:src/Makefile.in                               \
          src/libxalt/module.mk:src/libxalt/Module.mk.in             \
          src/linker/module.mk:src/linker/Module.mk.in               \
          src/util/module.mk:src/util/Module.mk.in])
AC_OUTPUT

AX_SUMMARIZE_CONFIG
