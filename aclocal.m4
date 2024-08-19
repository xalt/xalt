dnl AX_PYTHON_MODULE(path_to_python, modname [, fatal ])

AC_DEFUN([AX_PYTHON_MODULE],[
	AC_MSG_CHECKING(python module: $2)
	$1 -c "import $2" > /dev/null 2>&1
	if test $? -eq 0;
	then
		AC_MSG_RESULT(yes)
		eval AS_TR_CPP(HAVE_PYMOD_$2)=yes
	else
		AC_MSG_RESULT(no)
		eval AS_TR_CPP(HAVE_PYMOD_$2)=no
		#
		if test -n "$3"
		then
			AC_MSG_ERROR(failed to find required module $2)
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

if test -z "$CXX_LD_LIBRARY_PATH" ; then
  CXX_LD_LIBRARY_PATH="<empty>"
fi


echo
echo '----------------------------------- SUMMARY ----------------------------------'
echo
echo "Package version......................................" : XALT-$VERSION
echo "Package version (git) ..............................." : $xaltV
echo
echo "Prefix..............................................." : $prefix
echo "Installation Directory..............................." : $PKGV
echo "XALT Site Controlled Prefix.........................." : $SITE_CONTROLLED_PREFIX
echo
echo "XALT PRELOAD MODE ONLY..............................." : $PRELOAD_ONLY
echo "XALT_FILE_PREFIX....................................." : "$XALT_FILE_PREFIX"
echo "XALT_TRANSMISSION_STYLE.............................." : $TRANSMISSION
echo "XALT_ETC_DIR........................................." : $ETC_DIR
echo "XALT Config.py file.................................." : $XALT_CONFIG_PY
echo "XALT SYSTEM PATH....................................." : $SYSTEM_PATH
echo "XALT EXECUTION CMDLINE Record........................" : $XALT_CMDLINE_RECORD
echo "XALT SYSLOG Message Size............................." : $SYSLOG_MSG_SZ
echo "XALT SYSHOST CONFIG Style............................" : $SYSHOST_CONFIG
echo "XALT_SCALAR_TRACKING................................." : $XALT_SCALAR_TRACKING
echo "XALT_MPI_TRACKING...................................." : $XALT_MPI_TRACKING
echo "XALT_GPU_TRACKING_STYLE.............................." : $XALT_GPU_TRACKING_STYLE
echo "XALT_GPU_TRACKING...................................." : $XALT_GPU_TRACKING
echo "XALT_SUPPORT_CURL...................................." : $XALT_SUPPORT_CURL
echo "XALT UUID V7 Support................................." : $XALT_UUID_V7
echo "XALT 32bit support..................................." : $HAVE_32BIT
echo "XALT tmpdir.........................................." : $XALT_TMPDIR
echo "XALT Using DCGM......................................" : $HAVE_DCGM
echo "XALT Using NVML......................................" : $HAVE_NVML
echo "XALT build with MySQL support........................" : $Using_MYSQL
echo "XALT Compute SHA1 sum for libraries.................." : $COMPUTE_SHA1SUM
echo "XALT CXX LD_LIBRARY_PATH............................." : $CXX_LD_LIBRARY_PATH
echo "XALT_LD_LIBRARY_PATH................................." : $XALT_LD_LIBRARY_PATH
echo "XALT_LOGGING_URL....................................." : $XALT_LOGGING_URL
echo "XALT prime number...................................." : $XALT_PRIME_NUMBER
echo "XALT signal handler.................................." : $XALT_SIGNAL_HANDLER
echo "XALT prime fmt......................................." : $XALT_PRIME_FMT
echo "XALT my_hostname_parser.............................." : $MY_HOSTNAME_PARSER
echo "CURL_STR............................................." : $CURL_STR
echo "CRYPTO_STR..........................................." : $CRYPTO_STR
echo "GPU_STR.............................................." : $GPU_STR
echo "UUID_STR............................................." : $UUID_STR
echo "MODE................................................." : $MODE
echo
echo '------------------------------------------------------------------------------'
echo

])
