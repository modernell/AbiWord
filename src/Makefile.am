
SUBDIRS = af other text wp

libabiword_cpp = 
opt_libs = 
platform_ldflags = 
resources = 

if TOOLKIT_COCOA
AM_CXXFLAGS = -x objective-c++
main_cpp = $(top_srcdir)/src/wp/main/cocoa/CocoaMain.cpp
endif

if TOOLKIT_GTK
libabiword_cpp += $(top_srcdir)/src/wp/main/gtk/libabiword.cpp
main_cpp = $(top_srcdir)/src/wp/main/gtk/UnixMain.cpp
opt_libs += $(GOFFICE_BUILTIN_LIBS)
endif

if TOOLKIT_WIN
main_cpp = $(top_srcdir)/src/wp/main/win/Win32Main.cpp
opt_libs += Win32Resources.lo
platform_ldflags += -Wl,--gc-sections -Wl,--export-all-symbols -Wl,--enable-auto-import -Wl,--out-implib,abiword.dll.a
resources += Win32Resources.lo
endif

if LINK_OTHER
opt_libs += other/libother.la
endif

lib_LTLIBRARIES = libabiword-2.7.la

libabiword_2_7_la_CPPFLAGS = \
	$(WP_CPPFLAGS) \
	-DABIWORD_DATADIR="\"$(ABIWORD_DATADIR)\""

libabiword_2_7_la_LDFLAGS = \
	$(DEPS_LIBS) \
	--no-undefined \
	-avoid-version \
	-export-dynamic

libabiword_2_7_la_LIBADD = \
	$(opt_libs) \
	af/libaf.la \
	text/libtext.la \
	wp/libwp.la

libabiword_2_7_la_DEPENDENCIES = \
	$(libabiword_2_7_la_LIBADD)

libabiword_2_7_la_SOURCES = \
	$(top_srcdir)/src/wp/main/xp/abi_ver.cpp \
	$(libabiword_cpp)

libabiword_2_7_includedir = $(includedir)/abiword-2.7
libabiword_2_7_include_HEADERS = \
	wp/ap/gtk/abiwidget.h \
	wp/main/gtk/libabiword.h \
	af/xap/gtk/xap_UnixTableWidget.h

# TODO is it possible to track dependencies for that?
WINDRES_CPPFLAGS = -I$(top_srcdir)/src/af/xap/win -I$(top_srcdir)/src/af/util/win -I$(top_srcdir)/src/wp/ap/win
Win32Resources.lo: $(top_srcdir)/src/wp/main/win/Win32Main.rc
	CPPFLAGS="$(WINDRES_CPPFLAGS)" $(top_srcdir)/lt-compile-resource $< $@

bin_PROGRAMS =


if ENABLE_DYNAMIC

bin_PROGRAMS += abiword-dynamic

abiword_dynamic_CPPFLAGS = \
	$(libabiword_2_7_la_CPPFLAGS)

abiword_dynamic_LDFLAGS = \
	$(DEPS_LIBS) \
	--no-undefined \
	-avoid-version \
	-export-dynamic

abiword_dynamic_LDADD = \
	libabiword-2.7.la

abiword_dynamic_SOURCES = \
	$(main_cpp)

endif # ENABLE_DYNAMIC


if ENABLE_STATIC

bin_PROGRAMS += abiword

abiword_CPPFLAGS = \
	$(libabiword_2_7_la_CPPFLAGS)

# --no-undefined doesn't work for objc++, is it needed at all?
abiword_LDFLAGS = \
	$(platform_ldflags) \
	-export-dynamic

abiword_LDADD = \
	.libs/libabiword-2.7.a \
	$(resources) \
	$(DEPS_LIBS)

abiword_DEPENDENCIES = \
	libabiword-2.7.la \
	$(resources)

abiword_SOURCES = \
	$(main_cpp)

endif # ENABLE_STATIC

CLEANFILES = \
	abiword.dll.a

include $(top_srcdir)/objc.mk
