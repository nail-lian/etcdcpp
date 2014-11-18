#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(etcdcpp, 0.0.1, wangyongliang.wyl@gmail.com)
AC_CONFIG_SRCDIR([config.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability foreign subdir-objects])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_PROG_RANLIB
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h mach/mach.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/param.h sys/socket.h sys/statvfs.h sys/time.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime dup2 getcwd gethostname localtime_r memset mkdir realpath rmdir setenv socket sqrt stime strcasecmp strchr strdup strerror sysinfo uname utime])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 cli/Makefile
                 ])

AC_OUTPUT
