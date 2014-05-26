AC_INIT(Makefile.in)
AC_PREFIX_DEFAULT(/usr/local)
AC_SUBST(PATH_TO_AS)
AC_SUBST(PATH_TO_UUIDGEN)
AC_SUBST(PATH_TO_PSTREE)
AC_SUBST(BUILD_PSTREE)
AC_SUBST(USE_CONTRIB_ARGPARSE)

AC_PATH_PROG(PATH_TO_GIT, git, "")
AC_PATH_PROG(PATH_TO_AS,as, [])
AC_PATH_PROG(PATH_TO_UUIDGEN,uuidgen, [])
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
