/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */
/* acconfig.h
   This file is in the public domain.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.
   No software package will use all of them; autoheader copies the ones
   your configure.in uses into your configuration header file templates.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */


/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define if using ARCHITECTURE  */
#define ARCHITECTURE "arm"

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define if type char is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
/* #undef __CHAR_UNSIGNED__ */
#endif

/* Define the chooser routine */
/* #undef CHOOSER_ROUTINE */

/* Define the clear program */
#define CLEAR "/usr/bin/clear"

/* Define if the closedir function returns void instead of int.  */
/* #undef CLOSEDIR_VOID */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define for DECALPHA  */
/* #undef DECALPHA */

/* Define for DGUX with <sys/dg_sys_info.h>.  */
/* #undef DGUX */

/* Define if you have <dirent.h>.  */
/* #undef DIRENT */

/* Define if you want to disable force_localhost default  */
#define FORCE_LOCALHOST "1"

/* Define locale directory */
/* #undef LOCALEDIR */

/* Define if you want to disable lpd_bounce default  */
/* #undef LPD_BOUNCE */

/* Define to 1 if NLS is requested.  */
/* #undef ENABLE_NLS */


/* Define to the type of elements in the array set by `getgroups'.
   Usually this is either `int' or `gid_t'.  */
/* #undef GETGROUPS_T */

/* Define if the `getloadavg' function needs to be run setuid or setgid.  */
/* #undef GETLOADAVG_PRIVILEGED */

/* Define if the `getpgrp' function takes no argument.  */
/* #undef GETPGRP_VOID */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if you have a broken TIOCEXCL (IRIX 5) */
/* #undef HAVE_BROKEN_TIOCEXCL */

/* Define as 1 if you have catgets and don't want to use GNU gettext.  */
/* #undef HAVE_CATGETS */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if <errno.h> */
/* #undef HAVE_ERRNO */

/* Define if <errno.h> contains a declaration for extern int errno */
/* #undef HAVE_DECL_ERRNO */

/* Define if flock definition available */
#define HAVE_FLOCK_DEF 1

/* Define if you have gethostname() defined */
#define HAVE_GETHOSTNAME_DEF 1

/* Define if your system has its own `getloadavg' function.  */
/* #undef HAVE_GETLOADAVG */

/* Define if you have the getmntent function.  */
/* #undef HAVE_GETMNTENT */

/* Define if you have to use getpgrp(0) to get process group */
#define HAVE_GETPGRP_0 

/* Define as 1 if you have gettext and don't want to use GNU gettext.  */
/* #undef HAVE_GETTEXT */

/* Define as 1 if you have Heimdal Kerberos */
/* #undef HAVE_HEIMDAL */

/* Define if you have innetgr() defined in include file */
#define HAVE_INNETGR_DEF 1

/* Define if you have Kerberos 4 krb_sendauth defined in include file */
/* #undef HAVE_KRB_AUTH_DEF */

/* Define if you have Kerberos 5 xfree and variants */
/* #undef HAVE_KRB5_XFREE */
/* #undef HAVE_KRB_XFREE */

/* Define if your locale.h file contains LC_MESSAGES.  */
#define HAVE_LC_MESSAGES 1

/* Define if the `long double' type works.  */
#define HAVE_LONG_DOUBLE 1

/* Define if you support file names longer than 14 characters.  */
/* #undef HAVE_LONG_FILE_NAMES */

/* Define if the `long long' type works.  */
#define HAVE_LONG_LONG 1

/* Define if there is lseek proto.  */
#define HAVE_LSEEK_PROTO 1

/* Define if you have a working `mmap' system call.  */
/* #undef HAVE_MMAP */

/* Define if you have a openlog prototype.  */
#define HAVE_OPENLOG_DEF 1

/* Define if you have a outch prototype.  */
/* #undef HAVE_OUTCH_DEF */

/* Define if quad_t is present on the system  */
#define HAVE_QUAD_T 1

/* Define if _res defined */
/* #undef HAVE_RES */

/* Define if system calls automatically restart after interruption
   by a signal.  */
/* #undef HAVE_RESTARTABLE_SYSCALLS */

/* Define if you have to use setpgrp(0,0) to set process group */
/* #undef HAVE_SETPGRP_0 */

/* Define if you do have setproctitle() defined */
/* #undef HAVE_SETPROCTITLE_DEF */

/* Define if your struct stat has st_blksize.  */
/* #undef HAVE_ST_BLKSIZE */

/* Define if your struct stat has st_blocks.  */
/* #undef HAVE_ST_BLOCKS */

/* Define to 1 if stpcpy function is available.  */
#define HAVE_STPCPY 1

/* Define if you have strcasecmp definition  */
#define HAVE_STRCASECMP_DEF 1

/* Define if you have the strcoll function and it is properly defined.  */
/* #undef HAVE_STRCOLL */

/* Define if your struct stat has st_rdev.  */
/* #undef HAVE_ST_RDEV */

/* Define if you have the strftime function.  */
/* #undef HAVE_STRFTIME */

/* Define if we have a BSD-ish struct exec in <a.out.h> */
/* #undef HAVE_STRUCT_EXEC */

/* Define if we have a SOLARIS struct linger in <socket.h> */
/* #undef HAVE_STRUCT_LINGER */

/* Define if sys_errlist has the error strings. */
#define HAVE_SYS_ERRLIST 1

/* Define if sys_errlist is defined.  */
#define HAVE_DECL_SYS_ERRLIST 1

/* Define if syslog() defined */
#define HAVE_SYSLOG_DEF 1

/* Define if sys_nerr has number of err strings. */
#define HAVE_SYS_NERR 1

/* Define if sys_nerr has definition.  */
#define HAVE_SYS_NERR 1
/* #undef HAVE_DECL_SYS_NERR */

/* Define if sys_siglist has the signal strings. */
#define HAVE_SYS_SIGLIST 1

/* Define if _sys_siglist has the signal strings. */
#define HAVE__SYS_SIGLIST 1

/* Define if sys_siglist is defined in unistd.h include file */
#define HAVE_SYS_SIGLIST_DEF 1

/* Define if sys_siglist has the signal strings. */
#define HAVE__SYS_SIGLIST_DEF 1

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if tgetent() has definition  */
/* #undef HAVE_TGETENT_DEF */

/* Define if tgetstr() has definition  */
/* #undef HAVE_TGETSTR_DEF */

/* Define if your struct tm has tm_zone.  */
/* #undef HAVE_TM_ZONE */

/* Define if tputs() has definition  */
/* #undef HAVE_TPUTS_DEF */

/* Define if you don't have tm_zone but do have the external array
   tzname.  */
/* #undef HAVE_TZNAME */

/* Use the "union wait" union to get process status from wait3/waitpid */
#define HAVE_UNION_WAIT 1

/* Define if you have <unistd.h>.  */
#define HAVE_UNISTD_H 1

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
/* #undef HAVE_UTIME_NULL */

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define if you have the wait3 system call.  */
#define HAVE_WAIT3 1

/* HP/UX */
/* #undef HPUX */

/* HP/UX source */
/* #undef _HPUX_SOURCE */

/* Define if the struct in6_addr is defined */
#define IN6_ADDR 1

/* Define if the LINUX struct in_addr6 is defined */
/* #undef IN_ADDR6 */

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define if int is 16 bits instead of 32.  */
/* #undef INT_16_BITS */

/* IRIX */
/* #undef IRIX */

/* KERBEROS */
/* #undef KERBEROS */

/* MIT_KERBEROS4 */
/* #undef MIT_KERBEROS4 */

/* Define if long int is 64 bits.  */
/* #undef LONG_64_BITS */

/* Define if major, minor, and makedev are declared in <mkdev.h>.  */
/* #undef MAJOR_IN_MKDEV */

/* Define if major, minor, and makedev are declared in <sysmacros.h>.  */
/* #undef MAJOR_IN_SYSMACROS */

/* Define if on MINIX.  */
/* #undef _MINIX */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef mode_t */


/* Define if you don't have <dirent.h>, but have <ndir.h>.  */
/* #undef NDIR */

/* Define if you have <memory.h>, and <string.h> doesn't declare the
   mem* functions.  */
/* #undef NEED_MEMORY_H */

/* Define if your struct nlist has an n_un member.  */
/* #undef NLIST_NAME_UNION */

/* Define if you have <nlist.h>.  */
/* #undef NLIST_STRUCT */

/* Define if your C compiler doesn't accept -c and -o together.  */
/* #undef NO_MINUS_C_MINUS_O */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define the order routine */
/* #undef ORDER_ROUTINE */

/* make autoconf happy */
#define OSNAME "linux-gnu"
#define OSVERSION 26356

/* Define to the name of the distribution.  */
#define PACKAGE "LPRng"

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define if the system does not provide POSIX.1 features except
   with this defined.  */
/* #undef _POSIX_1_SOURCE */

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */


/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define if a setuid compromise is possible */
/* #undef SETUID_CHECK */

/* Define if the setvbuf function takes the buffering type as its second
   argument and the buffer pointer as the third, as on System V
   before release 3.  */
/* #undef SETVBUF_REVERSED */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define on SUN Solaris */
/* #undef SOLARIS */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if the `S_IS*' macros in <sys/stat.h> do not work properly.  */
/* #undef STAT_MACROS_BROKEN */

/* Define if you have the ANSI C header files.  */
/* #undef STDC_HEADERS */

/* Define if you have struct stat with st_mtime_nsec */
/* #undef ST_MTIMENSEC */
/* Define if you have struct stat with st_mtimespec.ts_nsec */
/* #undef ST_MTIMESPEC_TV_NSEC */

/* Define on SunOS */
/* #undef SUNOS */

/* Define on System V Release 4.  */
/* #undef SVR4 */

/* Define if you don't have <dirent.h>, but have <sys/dir.h>.  */
/* #undef SYSDIR */

/* Define if you don't have <dirent.h>, but have <sys/ndir.h>.  */
/* #undef SYSNDIR */

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define for Encore UMAX.  */
/* #undef UMAX */

/* Define for Encore UMAX 4.3 that has <inq_status/cpustats.h>
   instead of <sys/cpustats.h>.  */
/* #undef UMAX4_3 */

/* Define if you want a UNIX socket used */
/* #undef UNIXSOCKET */

/* Location of the UNIX socket */
/* #undef UNIXSOCKETPATH */

/* Define if we the user specifies an include file */
/* #undef USER_INCLUDE */

/* Define if we are to use the old sgttyb structure and <sgtty.h>. */
/* #undef USE_SGTTYB */

/* Use the statfs structure to find out free space */
/* #undef USE_STATFS */

/* Define how to get file system space */
#define USE_STATFS_TYPE STATVFS

/* Use the statvfs structure to find out free space */
/* #undef USE_STATVFS */

/* Define how to change serial line */
#define USE_STTY TERMIOS

/* Use the statfs() func with f_bfree member to find out free space */
/* #undef USE_SVR3_STATFS */

/* Define if we are to use the termio structure and TCGETA. */
/* #undef USE_TERMIO */

/* Define if we are to use the termios structure and TCGETS. */
#define USE_TERMIOS 1

/* Define if we are to use the HP TERMIOX calls */
/* #undef USE_TERMIOX */

/* Use the statfs() func with fs_data member to find out free space */
/* #undef USE_ULTRIX_STATFS */

/* Define if you do not have <strings.h>, index, bzero, etc..  */
/* #undef USG */

/* Define to the version of the distribution.  */
#define VERSION "3.8.10"

/* Define vfork as fork if vfork does not work.  */
/* #undef vfork */

/* Define if the closedir function returns void instead of int.  */
/* #undef VOID_CLOSEDIR */

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if lex declares yytext as a char * by default, not a char[].  */
/* #undef YYTEXT_POINTER */


/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Define if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix). */
#define HAVE_ALLOCA_H 1

/* Define if you have the <aouthdr.h> header file. */
/* #undef HAVE_AOUTHDR_H */

/* Define if you have the <argz.h> header file. */
#define HAVE_ARGZ_H 1

/* Define if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define if you have the <arpa/nameser.h> header file. */
#define HAVE_ARPA_NAMESER_H 1

/* Define if you have the <ar.h> header file. */
#define HAVE_AR_H 1

/* Define if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define if you have the <a_out.h> header file. */
/* #undef HAVE_A_OUT_H */

/* Define if you have the `cfsetispeed' function. */
#define HAVE_CFSETISPEED 1

/* Define if you have the <compat.h> header file. */
/* #undef HAVE_COMPAT_H */

/* Define if you have the <com_err.h> header file. */
/* #undef HAVE_COM_ERR_H */

/* Define if you have the <ctypes.h> header file. */
/* #undef HAVE_CTYPES_H */

/* Define if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define if you have the `dcgettext' function. */
/* #undef HAVE_DCGETTEXT */

/* Define if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define if you have the <exechdr.h> header file. */
/* #undef HAVE_EXECHDR_H */

/* Define if you have the `fcntl' function. */
#define HAVE_FCNTL 1

/* Define if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define if you have the <filehdr.h> header file. */
/* #undef HAVE_FILEHDR_H */

/* Define if you have the `flock' function. */
#define HAVE_FLOCK 1

/* Define if you have the `fork' function. */
#define HAVE_FORK 1

/* Define if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define if you have the `getdtablesize' function. */
#define HAVE_GETDTABLESIZE 1

/* Define if you have the `gethostbyname2' function. */
#define HAVE_GETHOSTBYNAME2 1

/* Define if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define if you have the `getrlimit' function. */
#define HAVE_GETRLIMIT 1

/* Define if you have the <grp.h> header file. */
#define HAVE_GRP_H 1

/* Define if you have the `inet_aton' function. */
#define HAVE_INET_ATON 1

/* Define if you have the `inet_ntop' function. */
#define HAVE_INET_NTOP 1

/* Define if you have the `inet_pton' function. */
#define HAVE_INET_PTON 1

/* Define if you have the `initgroups' function. */
#define HAVE_INITGROUPS 1

/* Define if you have the `innetgr' function. */
#define HAVE_INNETGR 1

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the <kerberosIV/krb.h> header file. */
/* #undef HAVE_KERBEROSIV_KRB_H */

/* Define if you have the `killpg' function. */
#define HAVE_KILLPG 1

/* Define if you have the `krb5_free_data_contents' function. */
/* #undef HAVE_KRB5_FREE_DATA_CONTENTS */

/* Define if you have the <krb5.h> header file. */
/* #undef HAVE_KRB5_H */

/* Define if you have the <krb.h> header file. */
/* #undef HAVE_KRB_H */

/* Define if you have the `i' library (-li). */
/* #undef HAVE_LIBI */

/* Define if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define if you have the `lockf' function. */
#define HAVE_LOCKF 1

/* Define if you have the <machine/vmparam.h> header file. */
/* #undef HAVE_MACHINE_VMPARAM_H */

/* Define if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define if you have the `mktemp' function. */
#define HAVE_MKTEMP 1

/* Define if you have a working `mmap' system call. */
/* #undef HAVE_MMAP */

/* Define if you have the `munmap' function. */
#define HAVE_MUNMAP 1

/* Define if you have the <ndir.h> header file. */
/* #undef HAVE_NDIR_H */

/* Define if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <nlist.h> header file. */
/* #undef HAVE_NLIST_H */

/* Define if you have the <nl_types.h> header file. */
#define HAVE_NL_TYPES_H 1

/* Define if you have the `openlog' function. */
#define HAVE_OPENLOG 1

/* Define if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define if you have the `rand' function. */
#define HAVE_RAND 1

/* Define if you have the `random' function. */
#define HAVE_RANDOM 1

/* Define if you have the <reloc.h> header file. */
/* #undef HAVE_RELOC_H */

/* Define if you have the <resolv.h> header file. */
#define HAVE_RESOLV_H 1

/* Define if you have the <select.h> header file. */
/* #undef HAVE_SELECT_H */

/* Define if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define if you have the `seteuid' function. */
#define HAVE_SETEUID 1

/* Define if you have the `setgroups' function. */
#define HAVE_SETGROUPS 1

/* Define if you have the <setjmp.h> header file. */
#define HAVE_SETJMP_H 1

/* Define if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define if you have the `setpgid' function. */
#define HAVE_SETPGID 1

/* Define if you have the `setproctitle' function. */
/* #undef HAVE_SETPROCTITLE */

/* Define if you have the `setresuid' function. */
#define HAVE_SETRESUID 1

/* Define if you have the `setreuid' function. */
#define HAVE_SETREUID 1

/* Define if you have the `setruid' function. */
/* #undef HAVE_SETRUID */

/* Define if you have the `setsid' function. */
#define HAVE_SETSID 1

/* Define if you have the <sgs.h> header file. */
/* #undef HAVE_SGS_H */

/* Define if you have the <sgtty.h> header file. */
#define HAVE_SGTTY_H 1

/* Define if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define if you have the `siglongjmp' function. */
#define HAVE_SIGLONGJMP 1

/* Define if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define if you have the `sigprocmask' function. */
#define HAVE_SIGPROCMASK 1

/* Define if you have the `socketpair' function. */
#define HAVE_SOCKETPAIR 1

/* Define if you have the <stab.h> header file. */
#define HAVE_STAB_H 1

/* Define if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have the `stpcpy' function. */
#define HAVE_STPCPY 1

/* Define if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define if you have the `sysconf' function. */
#define HAVE_SYSCONF 1

/* Define if you have the `sysinfo' function. */
#define HAVE_SYSINFO 1

/* Define if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define if you have the <sys/dir.h> header file. */
#define HAVE_SYS_DIR_H 1

/* Define if you have the <sys/exechdr.h> header file. */
/* #undef HAVE_SYS_EXECHDR_H */

/* Define if you have the <sys/exec.h> header file. */
/* #undef HAVE_SYS_EXEC_H */

/* Define if you have the <sys/fcntl.h> header file. */
#define HAVE_SYS_FCNTL_H 1

/* Define if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/loader.h> header file. */
/* #undef HAVE_SYS_LOADER_H */

/* Define if you have the <sys/mount.h> header file. */
#define HAVE_SYS_MOUNT_H 1

/* Define if you have the <sys/ndir.h> header file. */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/pstat.h> header file. */
/* #undef HAVE_SYS_PSTAT_H */

/* Define if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/signal.h> header file. */
#define HAVE_SYS_SIGNAL_H 1

/* Define if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/statfs.h> header file. */
#define HAVE_SYS_STATFS_H 1

/* Define if you have the <sys/statvfs.h> header file. */
#define HAVE_SYS_STATVFS_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/syslog.h> header file. */
#define HAVE_SYS_SYSLOG_H 1

/* Define if you have the <sys/systeminfo.h> header file. */
/* #undef HAVE_SYS_SYSTEMINFO_H */

/* Define if you have the <sys/termiox.h> header file. */
/* #undef HAVE_SYS_TERMIOX_H */

/* Define if you have the <sys/termio.h> header file. */
/* #undef HAVE_SYS_TERMIO_H */

/* Define if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/ttold.h> header file. */
/* #undef HAVE_SYS_TTOLD_H */

/* Define if you have the <sys/ttycom.h> header file. */
/* #undef HAVE_SYS_TTYCOM_H */

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* Define if you have the <sys/vfs.h> header file. */
#define HAVE_SYS_VFS_H 1

/* Define if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define if you have the `tcdrain' function. */
#define HAVE_TCDRAIN 1

/* Define if you have the `tcflush' function. */
#define HAVE_TCFLUSH 1

/* Define if you have the `tcsetattr' function. */
#define HAVE_TCSETATTR 1

/* Define if you have the <termcap.h> header file. */
/* #undef HAVE_TERMCAP_H */

/* Define if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define if you have the <termio.h> header file. */
#define HAVE_TERMIO_H 1

/* Define if you have the <term.h> header file. */
/* #undef HAVE_TERM_H */

/* Define if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define if you have the `uname' function. */
#define HAVE_UNAME 1

/* Define if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have the `unsetenv' function. */
#define HAVE_UNSETENV 1

/* Define if you have the <utsname.h> header file. */
/* #undef HAVE_UTSNAME_H */

/* Define if you have the <varargs.h> header file. */
/* #undef HAVE_VARARGS_H */

/* Define if you have the `vfork' function. */
#define HAVE_VFORK 1

/* Define if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define if you have the <vmparam.h> header file. */
/* #undef HAVE_VMPARAM_H */

/* Define if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define if you have the `wait3' function. */
#define HAVE_WAIT3 1

/* Define if you have the `waitpid' function. */
#define HAVE_WAITPID 1

/* Define if `fork' works. */
#define HAVE_WORKING_FORK 1

/* Define if `vfork' works. */
#define HAVE_WORKING_VFORK 1

/* Define if you have the `_res' function. */
/* #undef HAVE__RES */

/* Define if you have the `__argz_count' function. */
#define HAVE___ARGZ_COUNT 1

/* Define if you have the `__argz_next' function. */
#define HAVE___ARGZ_NEXT 1

/* Define if you have the `__argz_stringify' function. */
#define HAVE___ARGZ_STRINGIFY 1

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
        STACK_DIRECTION > 0 => grows toward higher addresses
        STACK_DIRECTION < 0 => grows toward lower addresses
        STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files. */
/* #undef STDC_HEADERS */

/* Define if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* # undef _ALL_SOURCE */
#endif

/* Define if on MINIX. */
/* #undef _MINIX */

/* Define if the system does not provide POSIX.1 features except with this
   defined. */
/* #undef _POSIX_1_SOURCE */

/* Define if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
/* #undef inline */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */

/* Define to empty if the keyword `volatile' does not work. Warning: valid
   code using `volatile' can become incorrect without. Disable with care. */
/* #undef volatile */