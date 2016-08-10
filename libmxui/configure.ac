AC_INIT(configure.ac)
AC_CANONICAL_SYSTEM


AC_CONFIG_HEADERS(config.h)


AM_INIT_AUTOMAKE(libmxui, 0.1)
AM_PROG_AS
AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl KDE_FIND_PATH(programm-name, variable-name, list of directories,
dnl     if-not-found, test-parameter)
AC_DEFUN(KDE_FIND_PATH,
[
   AC_MSG_CHECKING([for $1])
   if test -n "$$2"; then
        kde_cv_path="$$2";
   else
        kde_cache=`echo $1 | sed 'y%./+-%__p_%'`

        AC_CACHE_VAL(kde_cv_path_$kde_cache,
        [
        kde_cv_path="NONE"
        dirs="$3"
        kde_save_IFS=$IFS
        IFS=':'
        for dir in $PATH; do
          dirs="$dirs $dir"
        done
        IFS=$kde_save_IFS

        for dir in $dirs; do
          if test -x "$dir/$1"; then
            if test -n "$5"
            then
              evalstr="$dir/$1 $5 2>&1 "
              if eval $evalstr; then
                kde_cv_path="$dir/$1"
                break
              fi
            else
                kde_cv_path="$dir/$1"
                break
            fi
          fi
        done

        eval "kde_cv_path_$kde_cache=$kde_cv_path"

        ])

      eval "kde_cv_path=\"`echo '$kde_cv_path_'$kde_cache`\""

   fi

   if test -z "$kde_cv_path" || test "$kde_cv_path" = NONE; then
      AC_MSG_RESULT(not found)
      $4
   else
      AC_MSG_RESULT($kde_cv_path)
      $2=$kde_cv_path

   fi
])

KDE_FIND_PATH(sdl-config, LIBSDL_CONFIG, [${prefix}/bin ${exec_prefix}/bin ], [
  AC_MSG_WARN([Could not find libsdl anywhere, check http://www.sdl.org])
])

KDE_FIND_PATH(pkg-config, PKGCONFIG, [${prefix}/bin ${exec_prefix}/bin], [ AC_MSG_WARN([could not find pkg-config, ensure pkg-config and libmx are installed ]) ])
KDE_FIND_PATH(python2.7-config, PYTHONCONFIG, [${prefix}/bin ${exec_prefix}/bin], [ AC_MSG_WARN([could not find python, ensure python config and development library's are installed ]) ])

if test -n "$LIBSDL_CONFIG"; then

     LIBSDL_LIBS="`$LIBSDL_CONFIG --libs`"
    
     LIBSDL_RPATH=
     for args in $LIBSDL_LIBS; do
          case $args in
            -L*)
               LIBSDL_RPATH="$LIBSDL_RPATH $args"
               ;;
          esac
     done


	if test ${prefix} != "NONE"; then
	mx=${prefix}/lib/pkgconfig/mx.pc
	else
	mx=/usr/local/lib/pkgconfig/mx.pc
	fi

     LIBSDL_RPATH="`echo $LIBSDL_RPATH | sed -e "s/-L/-R/g"`"
     LIBSDL_CFLAGS="`$LIBSDL_CONFIG --cflags`"
     LIBMX_LIBS="`$PKGCONFIG $mx --libs`"
     LIBMX_CFLAGS=" `$PKGCONFIG $mx --cflags` "
     PY_LIBS="`$PYTHONCONFIG --libs`"


     AC_DEFINE_UNQUOTED(HAVE_LIBSDL, 1, [Defines if your system has the LIBSDL library])
fi

AC_ARG_ENABLE(assert,
[   --disable-assert              Disable Assertions  ],no_assert=yes)

AC_ARG_ENABLE(debug_mode,
[   --enable-debug_mode              Enable Debug  ],enable_d=yes)

if test  -n "$no_assert"; then
AC_DEFINE_UNQUOTED(ASSERT_OFF, 1, [assert off])
printf "assert disabled turn off only for stable builds\n"
fi

if  test -n "$enable_d"; then

AC_DEFINE_UNQUOTED(DEBUG_MODE, 1, [debug mode])
printf "Debug Mode Enabled\n"
else

AC_DEFINE_UNQUOTED(DEBUG_MODE, 0, [debug mode])
printf "Debug Mode Disabled\n"
fi

PY_CFLAGS="`$PYTHONCONFIG --cflags`"
if test "${prefix}" != "NONE"; then
path=${prefix}/include/mxl.h
#PY_CFLAGS="  -I${prefix}/include/python2.5 "
else
path=/usr/local/include/mxl.h
fi

if test -f "${path}" ; then
echo "mxl found will use static linkage"
else
echo "error mxl required, svn co svn://lostsidedead.com/mxl"
fi

echo "FLAGS $LIBSDL_CFLAGS  $LIBMX_CFLAGS"
echo "LIBS $LIBSDL_LIBS $LIBMX_LIBS"
echo "PY FLAGS: $PY_CFLAGS"
echo "PY LIBS: $PY_LIBS"


case "${target}" in

*darwin*|*apple*)
BOOST_PATH=""
BOOST_LIB="-L/opt/local/lib /opt/local/lib/libboost_system-mt.a  /opt/local/lib/libboost_python-mt.a /opt/local/lib/libboost_thread-mt.a"
;;
*linux*)
BOOST_PATH=""
BOOST_LIB="-lboost_thread -lboost_python -lboost_system -lGL -lGLU"
;;
esac

echo "boost path $BOOST_PATH libs $BOOST_LIB"

AC_SUBST(BOOST_PATH)
AC_SUBST(BOOST_LIB)
AC_SUBST(LIBSDL_CFLAGS)
AC_SUBST(LIBSDL_LIBS)
AC_SUBST(LIBMX_CFLAGS)
AC_SUBST(LIBMX_LIBS)
AC_SUBST(PY_CFLAGS)
AC_SUBST(PY_LIBS)

AC_OUTPUT(Makefile mxui.pc mxui/Makefile  masterx/Makefile)
