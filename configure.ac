AC_INIT(Makefile.in)
AC_PREFIX_DEFAULT(/usr/local)
AC_SUBST(PATH_TO_AS)
AC_SUBST(PATH_TO_UUIDGEN)
AC_SUBST(PATH_TO_PSTREE)
AC_SUBST(BUILD_PSTREE)

AC_PATH_PROG(PATH_TO_AS,as, [])
AC_PATH_PROG(PATH_TO_UUIDGEN,uuidgen, [])
AC_PATH_PROG(pstreeprog, pstree, [])
PATH_TO_PSTREE=$pstree_prog

BUILD_PSTREE=no
if test x$pstree_prog = x; then
  BUILD_PSTREE=yes
else
  versionStr=$(pstree_prog | head -1)
  name=`expr "$versionStr" : '.*(\(PSmisc\))'`
  if test x$name = "xPSmisc"; then
    major_version=`expr "$versionStr" : '.*(PSmisc) \([0-9]\+\)\.'`
    minor_version=`expr "$versionStr" : '.*(PSmisc) \+[0-9]\+\.\([0-9]\+\)'
    version=`expr $major_version \* 100000 + $minor_version`

    contrib_version=2200021

    if test $version < $contrib_version; then
      BUILD_PSTREE=yes
    fi
  fi
fi
  


AC_OUTPUT(makefile:Makefile.in)
