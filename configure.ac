AC_PREREQ([2.6])
AC_INIT([LinuxQQ], [0.2], [wxjeacen@gmail.com], [LinuxQQ], [https://github.com/gtkqq/gtkqq])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE


AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_LANG_CPLUSPLUS

AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL


LT_INIT
AC_PROG_LIBTOOL

AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([zlib], [compress])
AC_CHECK_PROG(SWIGPROG, swig, swig, MISSING)
if test $SWIGPROG = "MISSING"; then
  AC_MSG_WARN([Could not locate the swig command so NO wrappers can be built.
  If it exists, add it to your PATH otherwise install the swig package
  (or see http://www.swig.org for download details)])
  exit -1
fi

AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdint.h\
                                       stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h zlib.h boost/shared_ptr.hpp boost/function.hpp])

PKG_CHECK_MODULES(SQLITE3, [sqlite3 > 3.3], [
            AC_DEFINE(USE_SQLITE3, 1, [Use sqlite3])
            AC_SUBST(SQLITE3_CFLAGS)
            AC_SUBST(SQLITE3_LIBS)] ,
            [
            echo ""
            echo "configure: error: "
            echo "libsqlite3 development headers not found."
            echo ""
            exit -1])
            
PKG_CHECK_MODULES(CURL, [libcurl >= 7.20],[
           AC_DEFINE(USE_CURL,1,[Use curl])
           AC_SUBST(CURL_CFLAGS)
           AC_SUBST(CURL_LIBS)],
           [
           echo ""
           echo "configure: error: "
           echo "libcurl development headers not found."
           echo ""
           exit -1]
           )
           
PKG_CHECK_MODULES(PYTHON, [python-2.7 >= 2.7],[
	  AC_DEFINE(USE_PYTHON, 1 ,[Use python])
	  AC_SUBST(PYTHON_CFLAGS)
	  AC_SUBST(PYTHON_LIBS)],
	  [
	  echo ""
	  echo "configure: error: "
	  echo "python development headers not found."
	  echo ""
	  exit -1]
	  )

AM_PATH_PYTHON([2.7])
AC_DEFUN([adl_CHECK_PYTHON], 
[
   AC_CACHE_CHECK([for $am_display_PYTHON includes directory],
    [adl_cv_python_inc],
    [adl_cv_python_inc=`$PYTHON -c "from distutils import sysconfig; print sysconfig.get_python_inc()" 2>/dev/null`])
    AC_SUBST([PYTHONINC], [$adl_cv_python_inc])])

AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero gethostbyname memset mkdir select socket strstr strtol])
AC_CHECK_FUNCS([inet_ntoa])

#Set build CFLAGS

AC_ARG_ENABLE([use-event-queue],
	[AS_HELP_STRING([--enable-use-event-queue],[Use event queue for communication between interface and back end daemon])], enable_event_queue="$enableval" , enable_event_queue="yes" )
if test "$enable_event_queue" == "yes" ; then
	CFLAGS="$CFLAGS -DUSE_EVENT_QUEUE" 
fi

AC_CONFIG_FILES([Makefile
	 jsoncpp/Makefile
	 jsoncpp/src/Makefile
	 wrappers/Makefile
	 wrappers/python/Makefile
         src/Makefile
         test/Makefile
         ])
AC_OUTPUT


echo ============================== OPTIONS ==============================
echo Build CFLAGS: $CFLAGS
echo =====================================================================
