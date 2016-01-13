AC_INIT(Makefile.in)
AC_PREFIX_DEFAULT(/usr/local)

AC_SUBST(TRANSMISSION)
AC_ARG_WITH(transmission,
    AC_HELP_STRING([--with-transmission=ans],[transmission style (file,syslog,directdb) [[file]]]),
    TRANSMISSION="$withval"
    AC_MSG_RESULT([TRANSMISSION=$with_transmission])
    AC_DEFINE_UNQUOTED(TRANSMISSION, "$with_transmission")dnl
    ,
    withval="file"
    AC_MSG_RESULT([TRANSMISSION=$withval])
    TRANSMISSION="$withval"
    AC_DEFINE_UNQUOTED(TRANSMISSION, "$withval"))dnl

AC_SUBST(SITE)
AC_ARG_WITH(site,
    AC_HELP_STRING([--with-site=ans],[Site name [[default]]]),
    SITE="$withval"
    AC_MSG_RESULT([SITE=$with_site])
    AC_DEFINE_UNQUOTED(SITE, "$with_site")dnl
    ,
    withval="default"
    AC_MSG_RESULT([SITE=$withval])
    SITE="$withval"
    AC_DEFINE_UNQUOTED(SITE, "$withval"))dnl

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

AC_SUBST(ACCEPT_LIST)
AC_ARG_WITH(acceptList,
    AC_HELP_STRING([--with-acceptList=ans],[Filename that contains the list of acceptable paths [[accept.default.txt]]]),
    ACCEPT_LIST="$withval"
    AC_MSG_RESULT([ACCEPT_LIST=$with_acceptList])
    AC_DEFINE_UNQUOTED(ACCEPT_LIST, "$with_acceptList")dnl
    ,
    withval="$ac_confdir/accept.default.txt"
    AC_MSG_RESULT([ACCEPT_LIST=$withval])
    ACCEPT_LIST="$withval"
    AC_DEFINE_UNQUOTED(ACCEPT_LIST, "$withval"))dnl

AC_SUBST(IGNORE_LIST)
AC_ARG_WITH(ignoreList,
    AC_HELP_STRING([--with-ignoreList=ans],[Directory where xalt_db.conf and reverseMapD can be found [[.]]]),
    IGNORE_LIST="$withval"
    AC_MSG_RESULT([IGNORE_LIST=$with_ignoreList])
    AC_DEFINE_UNQUOTED(IGNORE_LIST, "$with_ignoreList")dnl
    ,
    withval="$ac_confdir/ignore.default.txt"
    AC_MSG_RESULT([IGNORE_LIST=$withval])
    IGNORE_LIST="$withval"
    AC_DEFINE_UNQUOTED(IGNORE_LIST, "$withval"))dnl

AC_SUBST(HOSTNAME_LIST)
AC_ARG_WITH(hostnameList,
    AC_HELP_STRING([--with-hostnameList=ans],[Directory where xalt_db.conf and reverseMapD can be found [[.]]]),
    HOSTNAME_LIST="$withval"
    AC_MSG_RESULT([HOSTNAME_LIST=$with_hostnameList])
    AC_DEFINE_UNQUOTED(HOSTNAME_LIST, "$with_hostnameList")dnl
    ,
    withval="$ac_confdir/hostname.default.txt"
    AC_MSG_RESULT([HOSTNAME_LIST=$withval])
    HOSTNAME_LIST="$withval"
    AC_DEFINE_UNQUOTED(HOSTNAME_LIST, "$withval"))dnl


AC_SUBST(SYS_LD_LIB_PATH)
SYS_LD_LIB_PATH=$LD_LIBRARY_PATH


AC_SUBST(PATH_TO_AS)
AC_SUBST(PATH_TO_BASENAME)
AC_SUBST(PATH_TO_CAT)
AC_SUBST(PATH_TO_DATE)
AC_SUBST(PATH_TO_DIRNAME)
AC_SUBST(PATH_TO_FILE)
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

AC_SUBST(BUILD_PSTREE)
AC_SUBST(USE_CONTRIB_ARGPARSE)

AC_PATH_PROG(PATH_TO_AS,as, [])
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
AC_PATH_PROG(PATH_TO_OBJDUMP, objdump, "")
AC_PATH_PROG(PATH_TO_OTOOL, otool, "")
AC_PATH_PROG(PATH_TO_PYTHON, python, "")
AC_PATH_PROG(PATH_TO_READLINK, readlink, [])
AC_PATH_PROG(PATH_TO_RM, rm, [])
AC_PATH_PROG(PATH_TO_SHA1SUM, sha1sum, "")
AC_PATH_PROG(PATH_TO_UUIDGEN,uuidgen, [])

AC_SUBST(HAVE_PYMOD_PSUTIL)
AC_SUBST(HAVE_PYMOD_MYSQLDB)
AC_PYTHON_MODULE([psutil])
AC_PYTHON_MODULE([MySQLdb])

if test "$HAVE_PYMOD_MYSQLDB" = no; then
   echo "unable to install XALT without the python module: MySQLdb"
   echo "To install it do: pip install MySQL-python"
   exit 1
fi


if test "$HAVE_PYMOD_PSUTIL" = no; then
   echo "unable to install XALT without the python module: psutil"
   echo "To install it do: pip install psutil"
   exit 1
fi
BUILD_PSTREE=no


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

$PATH_TO_PYTHON $ac_confdir/src/build_xalt_regex.py --accept $ACCEPT_LIST --ignore $IGNORE_LIST --hostname $HOSTNAME_LIST --input $ac_confdir/src/xalt_regex.template --output $ac_confdir/src/xalt_regex.h


AC_OUTPUT(makefile:Makefile.in)
