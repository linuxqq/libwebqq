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
AC_PROG_AR


LT_INIT
AC_PROG_LIBTOOL

AC_DEFUN([AX_BOOST_PYTHON],
    [AC_REQUIRE([AX_PYTHON])dnl
     AC_CACHE_CHECK(whether the Boost::Python library is available,
                            ac_cv_boost_python,
                            [AC_LANG_SAVE
                             AC_LANG_CPLUSPLUS
                             CPPFLAGS_SAVE="$CPPFLAGS"
                             if test x$PYTHON_INCLUDE_DIR != x; then
                                CPPFLAGS="-I$PYTHON_INCLUDE_DIR $CPPFLAGS"
                             fi
                             AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                             #include <boost/python/module.hpp>
                             using namespace boost::python;
                             BOOST_PYTHON_MODULE(test) { throw "Boost::Python test."; }]],
                             			   [[return 0;]])],
                             ac_cv_boost_python=yes, ac_cv_boost_python=no)
                             AC_LANG_RESTORE
                             CPPFLAGS="$CPPFLAGS_SAVE"
                     ])
                     
if test "$ac_cv_boost_python" = "yes"; then
  AC_DEFINE(HAVE_BOOST_PYTHON,,[define if the Boost::Python library is available])
  ax_python_lib=boost_python
  AC_ARG_WITH([boost-python],AS_HELP_STRING([--with-boost-python],[specify yes/no or the boost python library or suffix to use]),
  [if test "x$with_boost_python" != "xno"; then
     ax_python_lib=$with_boost_python
     ax_boost_python_lib=boost_python-$with_boost_python
   fi])
  BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
  for ax_lib in `ls $BOOSTLIBDIR/libboost_python*.so* $BOOSTLIBDIR/libboost_python*.dylib* $BOOSTLIBDIR/libboost_python*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_python.*\)\.so.*$;\1;' -e 's;^lib\(boost_python.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_python.*\)\.a.*$;\1;' ` $ax_python_lib $ax_boost_python_lib boost_python; do
    AC_CHECK_LIB($ax_lib, exit, [BOOST_PYTHON_LIB=$ax_lib break])
  done
  AC_SUBST(BOOST_PYTHON_LIB)
fi
])

AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([zlib], [compress])
AC_CHECK_LIB([boost_python],[base::encode])
dnl AC_CHECK_PROG(SWIGPROG, swig, swig, MISSING)
dnl if test $SWIGPROG = "MISSING"; then
dnl   AC_MSG_WARN([Could not locate the swig command so NO wrappers can be built.
dnl   If it exists, add it to your PATH otherwise install the swig package
dnl   (or see http://www.swig.org for download details)])
dnl   exit -1
dnl fi

AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdint.h\
                                       stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h zlib.h boost/shared_ptr.hpp boost/function.hpp])




dnl PKG_CHECK_MODULES(SQLITE3, [sqlite3 > 3.3], [
dnl            AC_DEFINE(USE_SQLITE3, 1, [Use sqlite3])
dnl            AC_SUBST(SQLITE3_CFLAGS)
dnl            AC_SUBST(SQLITE3_LIBS)] ,
dnl            [
dnl            echo ""
dnl            echo "configure: error: "
dnl            echo "libsqlite3 development headers not found."
dnl            echo ""
dnl            exit -1])
            
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

#Set build CXXFLAGS

AC_ARG_ENABLE([use-event-queue],
	[AS_HELP_STRING([--enable-use-event-queue],[Use event queue for communication between interface and back end daemon])], enable_event_queue="$enableval" , enable_event_queue="yes" )
if test "$enable_event_queue" == "yes" ; then
	CXXFLAGS="$CXXFLAGS -DUSE_EVENT_QUEUE" 
fi

AC_CONFIG_FILES([Makefile
                jsoncpp/Makefile
                jsoncpp/src/Makefile
                src/Makefile
	            wrappers/Makefile
                wrappers/boost/Makefile
                test/Makefile
         ])
AC_OUTPUT


echo ============================== OPTIONS ==============================
echo Build CXXFLAGS: $CXXFLAGS
echo =====================================================================
