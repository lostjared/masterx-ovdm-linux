bin_PROGRAMS=masterx
masterx_SOURCES=masterx.cpp welcome.cpp console.cpp mxdb.cpp chatdim.cpp


#MACOS_XVersion=-mmacosx-version-min=10.5 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk

AM_CPPFLAGS= -Wno-narrowing -std=c++98 $(MACOS_XVersion)  -I. -O2 -g $(LIBSDL_CFLAGS) -I${prefix}/include/mx  $(LIBMX_CFLAGS) -I../mxui $(PY_CFLAGS)  $(BOOST_PATH)
AM_LDFLAGS= 
masterx_LDADD= $(MACOS_XVersion)  -L. $(LIBSDL_LIBS) -L../mxui/.libs -lmx ../mxui/.libs/libmxui.a -L/usr/local/lib -lmasc  -lmxl $(LIBMX_LIBS)  $(PY_LIBS) $(BOOST_LIB)




