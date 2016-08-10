INCLUDES=$(LIBSDL_CFLAGS) $(LIBMX_CFLAGS) $(all_includes)

lib_LTLIBRARIES=libmxui.la

libmxui_includedir=$(includedir)/mx/mxui
libmxui_include_HEADERS=mxwidget.h mxwidget_type.h mxdimension.h mxserver.h mxwindow.h wcode.h mxreg.h mxmsgbox.h keyfilter.h  menu.h  mxe.h mxplugin.h

libmxui_la_SOURCES=mxwidget.cpp  mxwidget_type.cpp   mxdimension.cpp keyFilter.cpp mxserver.cpp mxwindow.cpp mxreg.cpp mxmsgbox.cpp menu.cpp mxplugin.cpp

libmxui_la_CXXFLAGS= -I/System/Library/Frameworks/Python.Framework/Headers -I${prefix}/include/mx -O2 -g -ansi -I../mxui $(LIBSDL_CFLAGS) $(LIBMX_CFLAGS)  $(BOOST_PATH)
libmxui_la_LIBADD=$(LIBSDL_LIBS)  $(LIBMX_LIBS) -lmx -lmxl -lmasc

include_dir=mx/mxui

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ../mxui.pc
