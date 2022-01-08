AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
EXTRA_CFLAGS= -Wno-unused-result -Wno-write-strings -pipe # Add optimizations here!

var_hardman_moduledir =$(localstatedir)/hardman/modules
my_libdir =$(libdir)

bin_PROGRAMS = 

if CHPKG_INFO_BUILD
bin_PROGRAMS += chpkg/chpkg-info 
endif
if CHPKG_EXPLODE_BUILD
bin_PROGRAMS += chpkg/chpkg-explode
endif

dist_bin_SCRIPTS =

if SHPKG_BUILD
dist_bin_SCRIPTS += shpkg/hpkg
endif
if SHPKG_REMV_BUILD
dist_bin_SCRIPTS += shpkg/hpkg-remove 
endif
if SHPKG_INFO_BUILD
dist_bin_SCRIPTS += shpkg/hpkg-info
endif
if SHPKG_EXPL_BUILD
dist_bin_SCRIPTS += shpkg/hpkg-explode 
endif
if HMAKE_BUILD
dist_bin_SCRIPTS += packing/hpkg-make
endif
if HARDMAN_BUILD
dist_bin_SCRIPTS += hardman/hardman hardman/hparts
endif

dist_var_hardman_module_SCRIPTS =

if DEFIN_BUILD
dist_var_hardman_module_SCRIPTS += hardman/default-plugins/default-install 
endif
if DEFDL_BUILD
dist_var_hardman_module_SCRIPTS += hardman/default-plugins/default-download 
endif 
if DEFLL_BUILD
dist_var_hardman_module_SCRIPTS += hardman/default-plugins/default-lowlevel 
endif
if DEFRS_BUILD
dist_var_hardman_module_SCRIPTS += hardman/default-plugins/default-resolve 
endif
if DEFUD_BUILD
dist_var_hardman_module_SCRIPTS += hardman/default-plugins/default-update-db
endif

libexec_PROGRAMS = 

if DARIA_BUILD
libexec_PROGRAMS += hardman/daria/daria 
endif
if RESLV_BUILD
libexec_PROGRAMS += hardman/resolver/hsfd-resolv
endif

dist_my_lib_SCRIPTS	= shpkg/lib/hpkglib.sh

if LIBHPM_PY_BUILD
pyexec_LTLIBRARIES 	= libhpm/python/libhpm.la
libhpm_python_libhpm_la_SOURCES= libhpm/python/libhpm_bind.c
libhpm_python_libhpm_la_LIBADD  = -lhrd $(PY_LIBS) libhpm/libhpm.la
libhpm_python_libhpm_la_CFLAGS  = $(EXTRA_CFLAGS) $(PY_CFLAGS) -I$(top_srcdir)/libhpm/
libhpm_python_libhpm_la_LDFLAGS = -module -avoid-version
endif

if CHPKG_INFO_BUILD
chpkg_chpkg_info_SOURCES= chpkg/chpkg-info.c
chpkg_chpkg_info_LDADD  = chpkg/libhard/libhard.la
chpkg_chpkg_info_CFLAGS = -DPROGNAME=\"chpkg-info\" -I$(top_srcdir)/chpkg/libhard $(EXTRA_CFLAGS)
endif
if CHPKG_EXPLODE_BUILD
chpkg_chpkg_explode_SOURCES = chpkg/chpkg-explode.c
chpkg_chpkg_explode_LDADD   = chpkg/libhard/libhard.la -l:libarchive.a -lacl -lzstd
chpkg_chpkg_explode_CFLAGS  = -DPROGNAME=\"chpkg-explode\" -I$(top_srcdir)/chpkg/libhard $(EXTRA_CFLAGS)
endif

lib_LTLIBRARIES = 

if LIBHARD_BUILD
lib_LTLIBRARIES += chpkg/libhard/libhard.la
endif
if LIBHPM_BUILD
lib_LTLIBRARIES += libhpm/libhpm.la
endif

noinst_HEADERS = chpkg/hpkglib.h chpkg/chpkg-info-help.h hardman/resolver/hsfd-resolv.h

include_HEADERS = 

if LIBHARD_BUILD
include_HEADERS += chpkg/libhard/libhard.h 
endif
if LIBHPM_BUILD
include_HEADERS += libhpm/libhpm.h
endif

if LIBHARD_BUILD
chpkg_libhard_libhard_la_SOURCES= chpkg/libhard/decrypt.c \
		     chpkg/libhard/split_pkg.c \
		     chpkg/libhard/open_package.c
chpkg_libhard_libhard_la_LIBADD = -lhrd
chpkg_libhard_libhard_la_CFLAGS = $(EXTRA_CFLAGS)
endif

if LIBHPM_BUILD
libhpm_libhpm_la_SOURCES  	= libhpm/free_cmd_result.c \
		     libhpm/free_package_info.c \
		     libhpm/get_file_list.c \
		     libhpm/get_package_info.c \
		     libhpm/package_is_installed.c \
		     libhpm/run_hardman_command.c
libhpm_libhpm_la_LIBADD		= -lhrd
libhpm_libhpm_la_CFLAGS		= $(EXTRA_CFLAGS)
endif

if DARIA_BUILD
hardman_daria_daria_SOURCES 	= hardman/daria/daria.cpp
hardman_daria_daria_LDADD       = -laria2 -lhrd
hardman_daria_daria_CXXFLAGS	= $(EXTRA_CFLAGS)
endif

if RESLV_BUILD
hardman_resolver_hsfd_resolv_SOURCES	= hardman/resolver/package.c \
		     		hardman/resolver/query.c \
		     		hardman/resolver/resolv.c
hardman_resolver_hsfd_resolv_CFLAGS	= $(EXTRA_CFLAGS)
endif


shpkg/lib/hpkglib.sh: shpkg/lib/hpkglib.sh.in
	@echo "  VER      shpkg/lib/hpkglib.sh"
	@sed shpkg/lib/hpkglib.sh.in -e s/VERSION_MACRO/$(VERSION)/ > shpkg/lib/hpkglib.sh

CLEANFILES=shpkg/lib/hpkglib.sh

SUBDIRS = po

EXTRA_DIST = m4/ChangeLog shpkg/lib/hpkglib.sh.in
