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

AC_SUBST(BROKER_SERVER)
AC_ARG_WITH(broker_server,
    AC_HELP_STRING([--with-broker_server=ans],[server for 'broker' transmission style [[localhost]]]),
    BROKER_SERVER="$withval"
    AC_MSG_RESULT([BROKER_SERVER=$with_broker_server])
    AC_DEFINE_UNQUOTED(BROKER_SERVER, "$with_broker_server")dnl
    ,
    withval="localhost"
    AC_MSG_RESULT([BROKER_SERVER=$withval])
    BROKER_SERVER="$withval"
    AC_DEFINE_UNQUOTED(BROKER_SERVER, "$withval"))dnl

AC_SUBST(BROKER_URI)
AC_ARG_WITH(broker_uri,
    AC_HELP_STRING([--with-broker_uri=ans],[URI for 'broker' transmission style [[logger]]]),
    BROKER_URI="$withval"
    AC_MSG_RESULT([BROKER_URI=$with_broker_uri])
    AC_DEFINE_UNQUOTED(BROKER_URI, "$with_broker_uri")dnl
    ,
    withval="logger"
    AC_MSG_RESULT([BROKER_URI=$withval])
    BROKER_URI="$withval"
    AC_DEFINE_UNQUOTED(BROKER_URI, "$withval"))dnl

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
AC_PYTHON_MODULE([psutil])

if test "$HAVE_PYMOD_PSUTIL" = yes; then
  BUILD_PSTREE=no

else
   AC_PATH_PROG(pstree_prog, pstree, [])

   PATH_TO_PSTREE=$pstree_prog

   printf "Use contrib pstree"
   BUILD_PSTREE=yes
   if test x$pstree_prog != x; then
     versionStr=$($pstree_prog --version 2>&1 | head -1)
     name=`expr "$versionStr" : '.*(\(PSmisc\))'`
     if test x$name = "xPSmisc" ; then
       major_version=`expr "$versionStr" : '.*(PSmisc) \([[0-9]]\+\)\.'`
       minor_version=`expr "$versionStr" : '.*(PSmisc) \+[[0-9]]\+\.\([[0-9]]\+\)'`
       version=`expr $major_version \* 100000 + $minor_version`
       contrib_version=2200020
       if test $version -ge $contrib_version ; then
         BUILD_PSTREE=no
       fi
     fi
   fi
   echo "... $BUILD_PSTREE"

   if test x$BUILD_PSTREE = xyes; then
      PATH_TO_PSTREE=$prefix/libexec/pstree
      AC_CHECK_HEADERS(curses.h)
      if test x$ac_cv_header_curses_h != xyes; then
         echo "Error: must have curses.h before building psmisc"
         exit 1
      fi
   fi
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



AC_OUTPUT(makefile:Makefile.in)
