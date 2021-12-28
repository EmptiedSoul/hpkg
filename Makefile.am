AUTOMAKE_OPTIONS = subdir-objects foreign
ACLOCAL_AMFLAGS = -I m4
EXTRA_CFLAGS=-Wno-unused-result -Wno-write-strings -pipe # Add optimizations here!

var_hardman_moduledir =$(localstatedir)/hardman/modules
my_libdir =$(libdir)

bin_PROGRAMS = chpkg/chpkg-info 
dist_bin_SCRIPTS = shpkg/hpkg shpkg/hpkg-remove shpkg/hpkg-info shpkg/hpkg-explode packing/hpkg-make hardman/hardman hardman/hparts
dist_var_hardman_module_SCRIPTS = hardman/default-plugins/default-install \
			   hardman/default-plugins/default-download \
			   hardman/default-plugins/default-lowlevel \
			   hardman/default-plugins/default-resolve \
			   hardman/default-plugins/default-update-db
libexec_PROGRAMS = hardman/daria/daria hardman/resolver/hsfd-resolv
dist_my_lib_SCRIPTS = shpkg/lib/hpkglib.sh

chpkg_chpkg_info_SOURCES = chpkg/chpkg-info.c
chpkg_chpkg_info_LDADD   = chpkg/libhard/libhard.la

chpkg_chpkg_info_CFLAGS  = -DPROGNAME=\"chpkg-info\" -I$(top_srcdir)/chpkg/libhard $(EXTRA_CFLAGS)

lib_LTLIBRARIES = chpkg/libhard/libhard.la libhpm/libhpm.la

noinst_HEADERS = chpkg/hpkglib.h chpkg/chpkg-info-help.h hardman/resolver/hsfd-resolv.h
include_HEADERS = chpkg/libhard/libhard.h libhpm/libhpm.h

chpkg_libhard_libhard_la_SOURCES = chpkg/libhard/decrypt.c \
		     chpkg/libhard/is_encrypted.c \
		     chpkg/libhard/split_pkg.c \
		     chpkg/libhard/validate.c
libhpm_libhpm_la_SOURCES  	= libhpm/free_cmd_result.c \
		     libhpm/free_package_info.c \
		     libhpm/get_file_list.c \
		     libhpm/get_package_info.c \
		     libhpm/package_is_installed.c \
		     libhpm/run_hardman_command.c
chpkg_libhard_libhard_la_LIBADD = -lhrd
libhpm_libhpm_la_LIBADD		= -lhrd
chpkg_libhard_libhard_la_CFLAGS = $(EXTRA_CFLAGS)
libhpm_libhpm_la_CFLAGS		= $(EXTRA_CFLAGS)

hardman_daria_daria_SOURCES 	= hardman/daria/daria.cpp
hardman_daria_daria_LDADD       = -laria2 -lhrd
hardman_daria_daria_CXXFLAGS	= $(EXTRA_CFLAGS)

hardman_resolver_hsfd_resolv_SOURCES	= hardman/resolver/package.c \
		     		hardman/resolver/query.c \
		     		hardman/resolver/resolv.c
hardman_resolver_hsfd_resolv_CFLAGS	= $(EXTRA_CFLAGS)



shpkg/lib/hpkglib.sh: shpkg/lib/hpkglib.sh.in
	@echo "  VER      shpkg/lib/hpkglib.sh"
	@sed shpkg/lib/hpkglib.sh.in -e "s/@VERSION@/$(VERSION)/" > shpkg/lib/hpkglib.sh

CLEANFILES=shpkg/lib/hpkglib.sh

SUBDIRS = po

EXTRA_DIST = m4/ChangeLog shpkg/lib/hpkglib.sh.in
