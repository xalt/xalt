AC_INIT(Makefile.in)
AC_CONFIG_HEADERS(src/__build__/xalt_header.h:src/xalt_header.h.in)
AC_PROG_CC
AC_PREFIX_DEFAULT(/usr/local)
AC_CONFIG_SUBDIRS(src)

AC_SUBST(XALT_INTERFACE_VERSION)
XALT_INTERFACE_VERSION=4


AC_SUBST(TRANSMISSION)
AC_ARG_WITH(transmission,
    AC_HELP_STRING([--with-transmission=ans],[transmission style (file,syslog,file_separate_dirs) [[file]]]),
    TRANSMISSION="$withval"
    AC_MSG_RESULT([TRANSMISSION=$with_transmission])
    AC_DEFINE_UNQUOTED(TRANSMISSION, "$with_transmission")dnl
    ,
    withval="file"
    AC_MSG_RESULT([TRANSMISSION=$withval])
    TRANSMISSION="$withval"
    AC_DEFINE_UNQUOTED(TRANSMISSION, "$withval"))dnl

AC_SUBST(XALT_FUNCTION_TRACKING)
AC_ARG_WITH(functionTracking,
    AC_HELP_STRING([--with-functionTracking=ans],[Track functions from modules [[yes]]]),
    XALT_FUNCTION_TRACKING="$withval"
    AC_MSG_RESULT([XALT_FUNCTION_TRACKING=$with_functionTracking])
    AC_DEFINE_UNQUOTED(XALT_FUNCTION_TRACKING, "$with_functionTracking")dnl
    ,
    withval="yes"
    AC_MSG_RESULT([XALT_FUNCTION_TRACKING=$withval])
    XALT_FUNCTION_TRACKING="$withval"
    AC_DEFINE_UNQUOTED(XALT_FUNCTION_TRACKING, "$withval"))dnl

AC_SUBST(XALT_FILE_PREFIX)
AC_ARG_WITH(xaltFilePrefix,
    AC_HELP_STRING([--with-xaltFilePrefix=ans],[Prefix where the json files are written [[$HOME]]]),
    XALT_FILE_PREFIX="$withval"
    AC_MSG_RESULT([XALT_FILE_PREFIX=$with_xaltFilePrefix])
    AC_DEFINE_UNQUOTED(XALT_FILE_PREFIX, "$with_xaltFilePrefix")dnl
    ,
    withval="USE_HOME"
    AC_MSG_RESULT([XALT_FILE_PREFIX=$withval])
    XALT_FILE_PREFIX="$withval"
    AC_DEFINE_UNQUOTED(XALT_FILE_PREFIX, "$withval"))dnl

AC_SUBST(XALT_GPU_TRACKING)
AC_ARG_WITH(trackGPU,
    AC_HELP_STRING([--with-trackGPU=ans],[Track GPU executables, [[no]]]),
    XALT_GPU_TRACKING="$withval"
    AC_MSG_RESULT([XALT_GPU_TRACKING=$with_trackGPU])
    AC_DEFINE_UNQUOTED(XALT_GPU_TRACKING, "$with_trackGPU")dnl
    ,
    withval="no"
    AC_MSG_RESULT([XALT_GPU_TRACKING=$withval])
    XALT_GPU_TRACKING="$withval"
    AC_DEFINE_UNQUOTED(XALT_GPU_TRACKING, "$withval"))dnl

AC_SUBST(XALT_MPI_TRACKING)
AC_ARG_WITH(trackMPI,
    AC_HELP_STRING([--with-trackMPI=ans],[Track MPI executables, [[yes]]]),
    XALT_MPI_TRACKING="$withval"
    AC_MSG_RESULT([XALT_MPI_TRACKING=$with_trackMPI])
    AC_DEFINE_UNQUOTED(XALT_MPI_TRACKING, "$with_trackMPI")dnl
    ,
    withval="yes"
    AC_MSG_RESULT([XALT_MPI_TRACKING=$withval])
    XALT_MPI_TRACKING="$withval"
    AC_DEFINE_UNQUOTED(XALT_MPI_TRACKING, "$withval"))dnl

AC_SUBST(XALT_SPSR_TRACKING)
AC_ARG_WITH(trackSPSR,
    AC_HELP_STRING([--with-trackSPSR=ans],[Track SPSR executables, [[yes]]]),
    XALT_SPSR_TRACKING="$withval"
    AC_MSG_RESULT([XALT_SPSR_TRACKING=$with_trackSPSR])
    AC_DEFINE_UNQUOTED(XALT_SPSR_TRACKING, "$with_trackSPSR")dnl
    ,
    withval="yes"
    AC_MSG_RESULT([XALT_SPSR_TRACKING=$withval])
    XALT_SPSR_TRACKING="$withval"
    AC_DEFINE_UNQUOTED(XALT_SPSR_TRACKING, "$withval"))dnl

AC_SUBST(XALT_SCALAR_TRACKING)
AC_ARG_WITH(trackScalarPrgms,
    AC_HELP_STRING([--with-trackScalarPrgms=ans],[Track non-mpi, non-spsr executables, [[yes]]]),
    XALT_SCALAR_TRACKING="$withval"
    AC_MSG_RESULT([XALT_SCALAR_TRACKING=$with_trackScalarPrgms])
    AC_DEFINE_UNQUOTED(XALT_SCALAR_TRACKING, "$with_trackScalarPrgms")dnl
    ,
    withval="yes"
    AC_MSG_RESULT([XALT_SCALAR_TRACKING=$withval])
    XALT_SCALAR_TRACKING="$withval"
    AC_DEFINE_UNQUOTED(XALT_SCALAR_TRACKING, "$withval"))dnl

AC_SUBST(COMPUTE_SHA1SUM)
AC_ARG_WITH(computeSHA1,
    AC_HELP_STRING([--with-computeSHA1=ans],[compute SHA1 sum on libraries, [[no]]]),
    COMPUTE_SHA1SUM="$withval"
    AC_MSG_RESULT([COMPUTE_SHA1SUM=$with_computeSHA1])
    AC_DEFINE_UNQUOTED(COMPUTE_SHA1SUM, "$with_computeSHA1")dnl
    ,
    withval="no"
    AC_MSG_RESULT([COMPUTE_SHA1SUM=$withval])
    COMPUTE_SHA1SUM="$withval"
    AC_DEFINE_UNQUOTED(COMPUTE_SHA1SUM, "$withval"))dnl


AC_SUBST(ETC_DIR)
AC_ARG_WITH(etcDir,
    AC_HELP_STRING([--with-etcDir=ans],[Directory where xalt_db.conf and reverseMapD can be found [[.]]]),
    ETC_DIR="$withval"
    AC_MSG_RESULT([ETC_DIR=$with_etcDir])
    AC_DEFINE_UNQUOTED(ETC_DIR, "$with_etcDir")dnl
    ,
    withval="."
    AC_MSG_RESULT([ETC_DIR=$withval])
    ETC_DIR="$withval"
    AC_DEFINE_UNQUOTED(ETC_DIR, "$withval"))dnl

AC_SUBST(XALT_CONFIG_PY)
AC_ARG_WITH(config,
    AC_HELP_STRING([--with-config=ans],[A python file defining the accept, ignore, hostname pattern lists [[NO DEFAULT]]]),
    XALT_CONFIG_PY="$withval"
    AC_MSG_RESULT([XALT_CONFIG_PY=$with_config])
    AC_DEFINE_UNQUOTED(XALT_CONFIG_PY, "$with_config")dnl
    ,
    withval="no"
    AC_MSG_RESULT([XALT_CONFIG_PY=$withval])
    XALT_CONFIG_PY="$withval"
    AC_DEFINE_UNQUOTED(XALT_CONFIG_PY, "$withval"))dnl

AC_SUBST(MYSQLDB)
AC_ARG_WITH(MySQL,
    AC_HELP_STRING([--with-MySQL=ans],[Require the MySQL-python module [[yes]]]),
    MYSQLDB="$withval"
    AC_MSG_RESULT([MYSQLDB=$with_MySQL])
    AC_DEFINE_UNQUOTED(MYSQLDB, "$with_MySQL")dnl
    ,
    withval="yes"
    AC_MSG_RESULT([MYSQLDB=$withval])
    MYSQLDB="$withval"
    AC_DEFINE_UNQUOTED(MYSQLDB, "$withval"))dnl

AC_SUBST(SYSTEM_PATH)
AC_ARG_WITH(systemPath,
    AC_HELP_STRING([--with-systemPath=[ans]],[]),
    SYSTEM_PATH="$withval"
    AC_MSG_RESULT([SYSTEM_PATH=$with_systemPath])
    AC_DEFINE_UNQUOTED(SYSTEM_PATH, "$with_systemPath")dnl
    ,
    withval="/usr/bin:/bin"
    AC_MSG_RESULT([SYSTEM_PATH=$withval])
    SYSTEM_PATH="$withval"
    AC_DEFINE_UNQUOTED(SYSTEM_PATH, "$withval"))dnl

AC_SUBST(SYSLOG_MSG_SZ)
AC_ARG_WITH(syslogMsgSz,
    AC_HELP_STRING([--with-syslogMsgSz=ans],[]),
    SYSLOG_MSG_SZ="$withval"
    AC_MSG_RESULT([SYSLOG_MSG_SZ=$with_syslogMsgSz])
    AC_DEFINE_UNQUOTED(SYSLOG_MSG_SZ, "$with_syslogMsgSz")dnl
    ,
    withval="512"
    AC_MSG_RESULT([SYSLOG_MSG_SZ=$withval])
    SYSLOG_MSG_SZ="$withval"
    AC_DEFINE_UNQUOTED(SYSLOG_MSG_SZ, "$withval"))dnl

AC_SUBST(CXX_LD_LIBRARY_PATH)
AC_ARG_WITH(cxxLDLibraryPath,
    AC_HELP_STRING([--with-cxxLDLibraryPath=ans],[LD_LIBRARY_PATH for c++ programs[[]]]),
    CXX_LD_LIBRARY_PATH="$withval"
    AC_MSG_RESULT([CXX_LD_LIBRARY_PATH=$with_cxxLDLibraryPath])
    AC_DEFINE_UNQUOTED(CXX_LD_LIBRARY_PATH, "$with_cxxLDLibraryPath")dnl
    ,
    withval=""
    AC_MSG_RESULT([CXX_LD_LIBRARY_PATH=$withval])
    CXX_LD_LIBRARY_PATH="$withval"
    AC_DEFINE_UNQUOTED(CXX_LD_LIBRARY_PATH, "$withval"))dnl

AC_SUBST(SYSHOST_CONFIG)
AC_ARG_WITH(syshostConfig,
    AC_HELP_STRING([--with-syshostConfig=ans],[How to determine syshost[[no default]]]),
    SYSHOST_CONFIG="$withval"
    AC_MSG_RESULT([SYSHOST_CONFIG=$with_syshostConfig])
    AC_DEFINE_UNQUOTED(SYSHOST_CONFIG, "$with_syshostConfig")dnl
    ,
    withval="no"
    AC_MSG_RESULT([SYSHOST_CONFIG=$withval])
    SYSHOST_CONFIG="$withval"
    AC_DEFINE_UNQUOTED(SYSHOST_CONFIG, "$withval"))dnl

AC_SUBST(MY_HOSTNAME_PARSER)
AC_ARG_WITH(hostnameParser,
    AC_HELP_STRING([--with-hostnameParser=ans],[Replace built-in lex based hostname parser c_file:file.c or library:file_64.a or library:file_64.a:file_32.a [[no]]]),
    MY_HOSTNAME_PARSER="$withval"
    AC_MSG_RESULT([MY_HOSTNAME_PARSER=$with_hostnameParser])
    AC_DEFINE_UNQUOTED(MY_HOSTNAME_PARSER, "$with_hostnameParser")dnl
    ,
    withval="no"
    AC_MSG_RESULT([MY_HOSTNAME_PARSER=$withval])
    MY_HOSTNAME_PARSER="$withval"
    AC_DEFINE_UNQUOTED(MY_HOSTNAME_PARSER, "$withval"))dnl

AC_SUBST(STATIC_LIBS)
AC_ARG_WITH(staticLibs,
    AC_HELP_STRING([--with-staticLibs=ans],[Link with static libraries (currently only libcrypto) [[no]]]),
    STATIC_LIBS="$withval"
    AC_MSG_RESULT([STATIC_LIBS=$with_staticLibs])
    AC_DEFINE_UNQUOTED(STATIC_LIBS, "$with_staticLibs")dnl
    ,
    withval="no"
    AC_MSG_RESULT([STATIC_LIBS=$withval])
    STATIC_LIBS="$withval"
    AC_DEFINE_UNQUOTED(STATIC_LIBS, "$withval"))dnl

AC_SUBST(PRELOAD_ONLY)
AC_ARG_WITH(preloadOnly,
    AC_HELP_STRING([--with-preloadOnly=ans],[Only use XALT in preload only mode[[yes]]]),
    PRELOAD_ONLY="$withval"
    AC_MSG_RESULT([PRELOAD_ONLY=$with_preloadOnly])
    AC_DEFINE_UNQUOTED(PRELOAD_ONLY, "$with_preloadOnly")dnl
    ,
    withval="yes"
    AC_MSG_RESULT([PRELOAD_ONLY=$withval])
    PRELOAD_ONLY="$withval"
    AC_DEFINE_UNQUOTED(PRELOAD_ONLY, "$withval"))dnl

AC_SUBST(BAD_INSTALL)
AC_ARG_WITH(IrefuseToInstallXALTCorrectly,
    AC_HELP_STRING([--with-IrefuseToInstallXALTCorrectly=ans],[Install XALT incorrectly [[no]]]),
    BAD_INSTALL="$withval"
    AC_MSG_RESULT([BAD_INSTALL=$with_IrefuseToInstallXALTCorrectly])
    AC_DEFINE_UNQUOTED(BAD_INSTALL, "$with_IrefuseToInstallXALTCorrectly")dnl
    ,
    withval="no"
    AC_MSG_RESULT([BAD_INSTALL=$withval])
    BAD_INSTALL="$withval"
    AC_DEFINE_UNQUOTED(BAD_INSTALL, "$withval"))dnl

AC_SUBST(HAVE_32BIT)

AX_TEST_COMPILE_FLAG([-m32],[HAVE_32BIT=yes],[HAVE_32BIT=no])
AC_LANG_PUSH(C++)
AX_TEST_COMPILE_FLAG([-std=c++11],[HAVE_CXX11=yes],[HAVE_CXX11=no])
AC_LANG_POP([C++])

AC_SUBST(PATH_TO_AS)
AC_SUBST(PATH_TO_BASENAME)
AC_SUBST(PATH_TO_CAT)
AC_SUBST(PATH_TO_DATE)
AC_SUBST(PATH_TO_DIRNAME)
AC_SUBST(PATH_TO_FILE)
AC_SUBST(PATH_TO_GIT)
AC_SUBST(PATH_TO_GREP)
AC_SUBST(PATH_TO_HEAD)
AC_SUBST(PATH_TO_LDD)
AC_SUBST(PATH_TO_LOGGER)
AC_SUBST(PATH_TO_MKDIR)
AC_SUBST(PATH_TO_OBJDUMP)
AC_SUBST(PATH_TO_PSTREE)
AC_SUBST(PATH_TO_READLINK)
AC_SUBST(PATH_TO_RM)
AC_SUBST(PATH_TO_SHA1SUM)
AC_SUBST(PATH_TO_UUIDGEN)

AC_SUBST(USE_CONTRIB_ARGPARSE)

AC_PATH_PROG(PATH_TO_GIT,git, [])
AC_PATH_PROG(PATH_TO_AS,as, [])
AC_PATH_PROG(PATH_TO_FLEX,flex, [])
AC_PATH_PROG(PATH_TO_BASENAME, basename, [])
AC_PATH_PROG(PATH_TO_CAT, cat, [])
AC_PATH_PROG(PATH_TO_DATE, date, [])
AC_PATH_PROG(PATH_TO_DIRNAME, dirname, [])
AC_PATH_PROG(PATH_TO_FILE, file, "")
AC_PATH_PROG(PATH_TO_GIT, git, "")
AC_PATH_PROG(PATH_TO_GREP, grep, [])
AC_PATH_PROG(PATH_TO_HEAD, head, [])
AC_PATH_PROG(PATH_TO_LDD, ldd, [])
AC_PATH_PROG(PATH_TO_LOGGER, logger, [])
AC_PATH_PROG(PATH_TO_MKDIR,mkdir, [])
AC_PATH_PROG(PATH_TO_MKTEMP, mktemp, [])
AC_PATH_PROG(PATH_TO_OBJDUMP, objdump, "")
AC_PATH_PROG(PATH_TO_OTOOL, otool, "")
AC_PATH_PROG(PATH_TO_PYTHON, python, "")
AC_PATH_PROG(PATH_TO_READLINK, readlink, [])
AC_PATH_PROG(PATH_TO_RM, rm, [])
AC_PATH_PROG(PATH_TO_SHA1SUM, sha1sum, "")
AC_PATH_PROG(PATH_TO_UUIDGEN,uuidgen, [])

if test "x${PATH_TO_MKTEMP}" = "x" ; then
  AC_MSG_ERROR([Unable to build XALT without mktemp])
fi

if test "x${PATH_TO_UUIDGEN}" = "x" ; then
  AC_MSG_ERROR([Unable to build XALT without uuidgen])
fi

transmission=`echo $TRANSMISSION | tr A-Z a-z`

found=no
for i in file file_separate_dirs syslog none; do
   if test $transmission = $i ; then
      found=yes
      break
   fi
done

if test found = no; then
   echo
   echo "======================================================="
   echo "XALT 2 does not support transmission style: $TRANSMISSION"
   echo "See the manual for supported methods"
   echo "======================================================="
   echo
   rm -f makefile
   exit 1
fi

AC_SUBST(HAVE_PYMOD_MYSQLDB)
AC_PYTHON_MODULE([MySQLdb])
AC_CHECK_HEADER(uuid/uuid.h,
                [AC_DEFINE([HAVE_UUID_UUID_H], 1,
                   [Define to 1 if you have <uuid/uuid.h>.])],
                [AC_MSG_ERROR([Unable to build XALT without uuid/uuid.h])])
AC_CHECK_HEADER(openssl/sha.h,
                [AC_DEFINE([HAVE_OPENSSL_SHA_H], 1,
                   [Define to 1 if you have <openssl/sha.h>.])],
                [AC_MSG_ERROR([Unable to build XALT without openssl/sha.h])])

AC_SUBST(HAVE_WORKING_LIBUUID)
HAVE_WORKING_LIBUUID=no

if test $ac_cv_header_uuid_uuid_h = yes; then
  AC_SEARCH_LIBS([uuid_generate],[uuid], [AC_DEFINE(HAVE_LIBUUID) HAVE_WORKING_LIBUUID=yes], [])
fi

if test -z "$PATH_TO_FLEX" ; then
   echo
   echo "======================================================="
   echo "Unable to install XALT without flex"
   echo "See the manual for details"
   echo "======================================================="
   rm -f makefile
   exit 1
fi


if test $HAVE_WORKING_LIBUUID = no -a ! -f /proc/sys/kernel/random/uuid; then
   echo
   echo "======================================================="
   echo "Unable to install XALT without uuid and"
   echo "without /proc/sys/kernel/random/uuid"
   echo "See the manual for details"
   echo "======================================================="
   rm -f makefile
   exit 1
fi

if test $HAVE_32BIT = yes -a $HAVE_WORKING_LIBUUID = yes; then
   CFLAGS_OLD=$CFLAGS
   CFLAGS="-m32 $CFLAGS"
   AC_SEARCH_LIBS([uuid_clear],[uuid], [HAVE_WORKING_LIBUUID_32=yes], [HAVE_WORKING_LIBUUID_32=no])
   CFLAGS=$CFLAGS_OLD
   if test $HAVE_WORKING_LIBUUID_32 = no; then
      echo
      echo "======================================================="
      echo "Unable to install XALT without a 32 version of the"
      echo "uuid library"
      echo ""
      echo "Your system has the 64 bit version of libuuid but not"
      echo "the 32 bit version."
      echo "======================================================="
      echo
      rm -f makefile
      exit 1
   fi
fi  
  
AC_SUBST(HAVE_DCGM)
HAVE_DCGM=no
if test $XALT_GPU_TRACKING = yes; then
  AC_CHECK_HEADER(dcgm_agent.h,
                  [AC_DEFINE([HAVE_DCGM_AGENT_H], 1,
                     [Define to 1 if you have DCGM.])],
                  [AC_MSG_ERROR([Unable to include GPU tracking without DCGM])])
  if test "$STATIC_LIBS" = yes; then
    AC_SEARCH_LIBS([dcgmInit], [dcgm_stub], [AC_DEFINE(USE_DCGM, [1]) HAVE_DCGM=yes], [AC_MSG_ERROR([Unable to include GPU tracking without DCGM])], [-ldl])
  else
    AC_SEARCH_LIBS([dcgmInit], [dcgm], [AC_DEFINE(USE_DCGM, [1]) HAVE_DCGM=yes], [AC_MSG_ERROR([Unable to include GPU tracking without DCGM])])
  fi
fi
AC_SUBST(PKGV)
AC_SUBST(VERSION)
AC_SUBST(XALT_GIT_VERSION)

VERSION=$(cat $ac_confdir/.version | sed -e 's/-devel//g')
PKGV=$prefix/xalt/$VERSION

if test $BAD_INSTALL = no; then
   if echo $prefix | grep -i xalt/$VERSION > /dev/null ; then
      echo
      echo "===================================================="
      echo "Do not install XALT with $VERSION as part of the"
      echo "prefix.  It will lead to many many problems with future"
      echo "installs of XALT"
      echo
      echo "Executables built with the current version of XALT"
      echo "will not work with future installs of XALT!!!"
      echo
      echo "If you feel that you know better than the developer"
      echo "of XALT then you can configure XALT with the configure"
      echo "--with-IrefuseToInstallXALTCorrectly=yes and set the prefix"
      echo "to include the version"
      echo 
      echo "If you wanted to install XALT in /apps/xalt/$version"
      echo "set the prefix as:"
      echo
      echo "  ./configure --prefix=/apps"
      echo
      echo "===================================================="
      echo
      rm -f makefile
      exit 1
   fi
fi

echo "VERSION=$VERSION"
if test -n "$PATH_TO_GIT" -a -d $srcdir/.git ; then
  xaltV=`(cd $srcdir; $PATH_TO_GIT describe)`
else
  xaltV="@git@"
fi
XALT_GIT_VERSION=$xaltV

if test "$HAVE_CXX11" = no; then
   echo
   echo "======================================================="
   echo "Unable to install XALT without g++ supporting -std=c++11"
   echo "See the manual for details"
   echo "======================================================="
   rm -f makefile
   exit 1
fi

if test "$XALT_CONFIG_PY" = no; then
   echo
   echo "======================================================="
   echo "Unable to install XALT without setting --with-config=..."
   echo "See the manual for details"
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
      echo "To install it do: pip install MySQL-python"
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
   echo "See the manual for details."
   echo "======================================================="
   rm -f makefile
   exit 1
fi


printf "Use contrib argparse"
USE_CONTRIB_ARGPARSE=no
versionStr=$(python --version 2>&1 | head -1)
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

if test "$STATIC_LIBS" = yes; then
    AC_SEARCH_LIBS([SHA1], [:libcrypto.a], [], [AC_MSG_ERROR([Unable to build XALT without libcrypto.a])], [-ldl -lz])
else
    AC_SEARCH_LIBS([SHA1], [crypto], [], [AC_MSG_ERROR([Unable to build XALT without libcrypto])])
fi

$PATH_TO_PYTHON $ac_confdir/py_build_tools/build_xalt_interval_table.py --confFn $XALT_CONFIG_PY --input $ac_confdir/src/tmpl/xalt_interval.template --output src/__build__/xalt_interval.h
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi

$PATH_TO_PYTHON $ac_confdir/py_build_tools/build_xalt_regex.py --confFn $XALT_CONFIG_PY --input $ac_confdir/src/tmpl/xalt_regex.template --output src/__build__/xalt_regex.h
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/py_build_tools/build_syshost_routine.py --input $SYSHOST_CONFIG --output src/__build__/xalt_syshost.c
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/py_build_tools/build_parser_routine.py --confFn $XALT_CONFIG_PY --pattern env_patterns --input $ac_confdir/src/tmpl/xalt_env_parser.template --output src/__build__/xalt_env_parser.lex
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/py_build_tools/build_parser_routine.py --confFn $XALT_CONFIG_PY --pattern path_patterns --input $ac_confdir/src/tmpl/xalt_path_parser.template --output src/__build__/xalt_path_parser.lex
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	

$PATH_TO_PYTHON $ac_confdir/py_build_tools/build_parser_routine.py --confFn $XALT_CONFIG_PY --pattern hostname_patterns --input $ac_confdir/src/tmpl/xalt_hostname_parser.template --output src/__build__/xalt_hostname_parser.lex
if test "$?" != 0; then
   rm -f makefile
   exit 1
fi	


AC_OUTPUT(makefile:Makefile.in src/__build__/xalt_config.h:src/xalt_config.h.in src/makefile:src/Makefile.in)

AX_SUMMARIZE_CONFIG
