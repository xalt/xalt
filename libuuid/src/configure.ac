AC_INIT(Makefile.in)
AC_CONFIG_HEADER(uuid_config.h.in:uuid_config.h)
AC_PROG_CC
AC_CHECK_HEADERS([linux/compiler.h linux/blkpg.h linux/major.h], [], [], [
#ifdef HAVE_LINUX_COMPILER_H
#include <linux/compiler.h>
#endif
])
AC_CHECK_HEADERS([ \
	asm/io.h \
	err.h \
	errno.h \
	fcntl.h \
	getopt.h \
	inttypes.h \
	lastlog.h \
	libutil.h \
	linux/btrfs.h \
	linux/cdrom.h \
	linux/falloc.h \
	linux/watchdog.h \
	linux/fd.h \
	linux/raw.h \
	linux/tiocl.h \
	linux/version.h \
	linux/securebits.h \
	linux/net_namespace.h \
	locale.h \
	mntent.h \
	net/if.h \
	net/if_dl.h \
	netinet/in.h \
	paths.h \
	pty.h \
	security/pam_appl.h \
	stdint.h \
	stdio_ext.h \
	stdlib.h \
	endian.h \
	byteswap.h \
	sys/endian.h \
	sys/disk.h \
	sys/disklabel.h \
	sys/file.h \
	sys/io.h \
	sys/ioccom.h \
	sys/ioctl.h \
	sys/mkdev.h \
	sys/mount.h \
	sys/param.h \
	sys/prctl.h \
	sys/resource.h \
	sys/signalfd.h \
	sys/socket.h \
	sys/sockio.h \
	sys/stat.h \
	sys/sysmacros.h \
	sys/swap.h \
	sys/syscall.h \
	sys/time.h \
	sys/timex.h \
	sys/ttydefaults.h \
	sys/types.h \
	sys/ucred.h \
	sys/un.h \
	unistd.h \
	shadow.h \
	utmp.h \
	utmpx.h \
])
AC_CHECK_MEMBER([struct sockaddr.sa_len],
		AC_DEFINE_UNQUOTED([HAVE_SA_LEN], [1], [Define if struct sockaddr contains sa_len]), [],
	[#include <sys/types.h>
	 #include <sys/socket.h>])

SOCKET_LIBS=
AC_SEARCH_LIBS([socket], [socket],
	[AS_IF([test x"$ac_cv_search_socket" != x"none required"],
		[SOCKET_LIBS="$SOCKET_LIBS -lsocket"])
])
AC_SUBST([SOCKET_LIBS])
