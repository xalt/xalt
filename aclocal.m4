dnl AC_PYTHON_MODULE(modname [, fatal ])

AC_DEFUN([AC_PYTHON_MODULE],[
	AC_MSG_CHECKING(python module: $1)
	python -c "import $1" > /dev/null 2>&1
	if test $? -eq 0;
	then
		AC_MSG_RESULT(yes)
		eval AS_TR_CPP(HAVE_PYMOD_$1)=yes
	else
		AC_MSG_RESULT(no)
		eval AS_TR_CPP(HAVE_PYMOD_$1)=no
		#
		if test -n "$2"
		then
			AC_MSG_ERROR(failed to find required module $1)
			exit 1
		fi
	fi
])

dnl @synopsis C_FLAGS_CHECK [compiler flags]                                        
dnl @summary check whether compiler supports given C flags or not                   
AC_DEFUN([C_FLAG_CHECK],                                                            
[dnl                                                                                  
  AC_MSG_CHECKING([if $CC supports $1])
  AC_LANG_PUSH([C])
  ac_saved_cflags="$CFLAGS"                                                       
  CFLAGS="-Werror $1"                                                               
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],                                            
    [AC_MSG_RESULT([yes])],                                                           
    [AC_MSG_ERROR([no])]                                                              
  )                                                                                   
  CFLAGS="$ac_saved_cflags"                                                       
  AC_LANG_POP([C])
])



AC_DEFUN([AX_TEST_COMPILE_FLAG],
[AC_PREREQ(2.64)dnl for _AC_LANG_PREFIX and AS_VAR_IF
AS_VAR_PUSHDEF([CACHEVAR],[ax_cv_check_[]_AC_LANG_ABBREV[]flags_$4_$1])dnl
AC_CACHE_CHECK([whether _AC_LANG compiler can build and run with $1], CACHEVAR, [
  ax_check_save_flags=$[]_AC_LANG_PREFIX[]FLAGS
  _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $4 $1"
  AC_RUN_IFELSE([m4_default([$5],[AC_LANG_PROGRAM()])],
    [AS_VAR_SET(CACHEVAR,[yes])],
    [AS_VAR_SET(CACHEVAR,[no])])
  _AC_LANG_PREFIX[]FLAGS=$ax_check_save_flags])
AS_VAR_IF(CACHEVAR,yes,
  [m4_default([$2], :)],
  [m4_default([$3], :)])
AS_VAR_POPDEF([CACHEVAR])dnl
])dnl AX_CHECK_COMPILE_FLAGS


AC_DEFUN([AX_SUMMARIZE_CONFIG],
[

Using_MYSQL=yes
if test "$MYSQL_CONFIG" = UNKNOWN_mysql_config; then
  Using_MYSQL=no
fi

echo
echo '----------------------------------- SUMMARY ----------------------------------'
echo
echo "Package version......................................" : XALT-$VERSION
echo "Package version (git) ..............................." : $xaltV
echo
echo "Prefix..............................................." : $prefix
echo "Installation Directory..............................." : $PKGV
echo
echo "XALT_FILE_PREFIX....................................." : "$XALT_FILE_PREFIX"
echo "XALT_TRANSMISSION_STYLE.............................." : $TRANSMISSION
echo "XALT_ETC_DIR........................................." : $ETC_DIR
echo "XALT Config.py file.................................." : $XALT_CONFIG_PY
echo "XALT SYSTEM PATH....................................." : $SYSTEM_PATH
echo "XALT SYSLOG Message Size............................." : $SYSLOG_MSG_SZ
echo "XALT SYSHOST CONFIG Style............................" : $SYSHOST_CONFIG
echo "XALT_SCALAR_TRACKING................................." : $XALT_SCALAR_TRACKING
echo "XALT_SPSR_TRACKING..................................." : $XALT_SPSR_TRACKING
echo "XALT_MPI_TRACKING...................................." : $XALT_MPI_TRACKING
echo "XALT 32bit support..................................." : $HAVE_32BIT
echo "XALT Using libuuid..................................." : $HAVE_WORKING_LIBUUID
echo "XALT build with MySQL support........................" : $Using_MYSQL
echo "XALT Backgrounding the start record of a program run." : $ENABLE_BACKGROUNDING
echo "XALT Compute SHA1 sum for libraries.................." : $COMPUTE_SHA1SUM
echo "XALT CXX LD_LIBRARY_PATH............................." : $CXX_LD_LIBRARY_PATH
echo "XALT my_hostname_parser.............................." : $MY_HOSTNAME_PARSER
echo '------------------------------------------------------------------------------'
echo

])
