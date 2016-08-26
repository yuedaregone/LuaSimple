
################
# Feature configuration
################

include config.unix

# Kepler launching model
# Define which launcher to build. If you need to install
# more than one launcher, install one at a time.
# Valid options: cgi, fastcgi, mod2, xavante
LAUNCHER?=xavante

# Optional Kepler packages (a comma-separated list)
# Valid options: lualogging luasql luaexpat luazip md5
OPTIONAL_PACKAGES?=lualogging,md5

# Which database to build LuaSQL for.
# Valid options: mysql, oci8, odbc, postgres, sqlite, sqlite3
LUASQL_DRIVER?=sqlite3

# Set BUILD_LUA to to yes if Lua should be built along with Kepler,
# and Kepler components should use this version. Xavante launchers
# will use the Kepler build of Lua.
# Set INSTALL_LUA to to yes if the Kepler build of Lua should be
# installed. INSTALL_LUA=yes implies BUILD_LUA=yes.
#
# Set both to no if you already have a LuaBinaries-compliant Lua installation.
BUILD_LUA?=yes
INSTALL_LUA?=yes

ifeq ($(INSTALL_LUA),yes)
   override BUILD_LUA=yes
endif

################
# External dependencies
################

READLINE_DIR?=/usr
READLINE_LIBDIR=$(READLINE_DIR)/lib
READLINE_INCLUDEDIR=$(READLINE_DIR)/include

NCURSES_DIR?=/usr
NCURSES_LIBDIR=$(NCURSES_DIR)/lib
NCURSES_INCLUDEDIR=$(NCURSES_DIR)/include

EXPAT_DIR?=/usr
EXPAT_LIBDIR=$(EXPAT_DIR)/lib
EXPAT_INCLUDEDIR=$(EXPAT_DIR)/include

ZZIP_DIR?=/usr
ZZIP_LIBDIR=$(ZZIP_DIR)/lib
ZZIP_INCLUDEDIR=$(ZZIP_DIR)/include

OCI8_DIR?=/home/oracle/OraHome1
OCI8_LIBDIR=$(OCI8_DIR)/lib
OCI8_DEMODIR=$(OCI8_DIR)/rdbms/demo
OCI8_PUBLICDIR=$(OCI8_DIR)/rdbms/public

SQLITE_DIR?=/usr
SQLITE_LIBDIR=$(SQLITE_DIR)/lib
SQLITE_INCLUDEDIR=$(SQLITE_DIR)/include

SQLITE3_DIR?=/usr
SQLITE3_LIBDIR=$(SQLITE3_DIR)/lib
SQLITE3_INCLUDEDIR=$(SQLITE3_DIR)/include

ODBC_DIR?=/usr
ODBC_LIBDIR=$(ODBC_DIR)/lib
ODBC_INCLUDEDIR=$(ODBC_DIR)/include

FASTCGI_DIR?=/usr
FASTCGI_LIBDIR=$(FASTCGI_DIR)/lib
FASTCGI_INCLUDEDIR=$(FASTCGI_DIR)/include

LUABINARIES_DIR?=/usr
LUABINARIES_LIBDIR?=$(LUABINARIES_DIR)/lib
LUABINARIES_INCLUDEDIR?=$(LUABINARIES_DIR)/include

################
# Installation directories
################

PREFIX?=/usr/local
SYSCONFDIR?=$(PREFIX)/etc
INCLUDEDIR?=$(PREFIX)/include
LUA_SUFFIX?=51

KEPLER_INSTALL_DIR?=$(PREFIX)
KEPLER_CONF=$(SYSCONFDIR)/kepler/1.1
KEPLER_INIT=$(KEPLER_CONF)/kepler_init.lua
KEPLER_WEB?=$(KEPLER_INSTALL_DIR)/htdocs
KEPLER_INSTALL_BINDIR=$(KEPLER_INSTALL_DIR)/bin
LUABINARIES_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LUABINARIES_INSTALL_INCLUDEDIR=$(INCLUDEDIR)
LUASOCKET_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
CGILUA_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
CGILUA_INSTALL_CONFDIR=$(KEPLER_CONF)
CGILUA_APPS=$(KEPLER_CONF)/apps
COPAS_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LUAFILESYSTEM_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LAUNCHER_CGI_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LAUNCHER_CGI_INSTALL_BINDIR=$(KEPLER_INSTALL_DIR)/bin
LAUNCHER_FASTCGI_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LAUNCHER_FASTCGI_INSTALL_BINDIR=$(KEPLER_INSTALL_DIR)/bin
LAUNCHER_MOD2_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LAUNCHER_MOD2_INSTALL_APACHE_MODDIR=$(APACHE_MODULES_DIR)
XAVANTE_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
XAVANTE_INSTALL_BINDIR=$(KEPLER_INSTALL_DIR)/bin
XAVANTE_INSTALL_CONFDIR=$(KEPLER_CONF)
XAVANTE_INSTALL_WEBDIR=$(KEPLER_WEB)
RINGS_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LUALOGGING_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LUASQL_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LUAEXPAT_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
LUAZIP_INSTALL_DIR=$(KEPLER_INSTALL_DIR)
MD5_INSTALL_DIR=$(KEPLER_INSTALL_DIR)

################
# Version information
################

KEPLER_VERSION=1.1
LUABINARIES_VERSION=5.1.2
LUASOCKET_VERSION=2.0.1

################
# General compilation flags
################

PLATFORM_CFLAGS=-Wl,-E
PLATFORM_LDFLAGS=-lm -ldl
LIB_FLAG=-shared -fPIC

ifeq ($(BUILD_LUA), yes)
   override LUABINARIES_LIB=lua-$(LUABINARIES_VERSION)/src/liblua$(LUA_SUFFIX).a
   override LUABINARIES_INCLUDEDIR=$(PWD)/lua-$(LUABINARIES_VERSION)/src
endif

LUABINARIES_LIB=$(LUABINARIES_LIBDIR)/liblua$(LUA_SUFFIX).a
LUABINARIES_DEFPATH=-DLUA_ROOT=\\\"$(LUABINARIES_DIR)\\\"
ifeq ($(USE_READLINE), yes)
   LUABINARIES_READLINE_CFLAGS=-DLUA_USE_READLINE -I$(READLINE_INCLUDEDIR)
   LUABINARIES_READLINE_LIBS=-lreadline -lhistory -lncurses -L$(READLINE_LIBDIR) -L$(NCURSES_LIBDIR)
else
   LUABINARIES_READLINE_CFLAGS=
   LUABINARIES_READLINE_LIBS=
endif
LUABINARIES_MYCFLAGS=-DLUA_USE_POSIX -DLUA_USE_DLOPEN $(LUABINARIES_READLINE_CFLAGS)
LUABINARIES_MYLIBS=$(PLATFORM_CFLAGS) -ldl $(LUABINARIES_READLINE_LIBS)
LUASOCKET_LD="$(CC)"
LUASOCKET_FLAGS=-DLUASOCKET_DEBUG -I$(LUABINARIES_INCLUDEDIR)
LUASOCKET_LDFLAGS=$(LIB_FLAG) -O -fPIC
LAUNCHER_CGI_FLAGS=$(PLATFORM_CFLAGS) $(PLATFORM_LDFLAGS) -I$(LUABINARIES_INCLUDEDIR) -DINCLUDE_LUA 
LAUNCHER_FASTCGI_FLAGS=$(PLATFORM_CFLAGS) $(PLATFORM_LDFLAGS) -I$(LUABINARIES_INCLUDEDIR) -DINCLUDE_LUA -I$(FASTCGI_INCLUDEDIR) -L$(FASTCGI_LIBDIR)
LAUNCHER_MOD2_LIBFLAGS=$(LIB_FLAG) $(PLATFORM_CFLAGS) $(PLATFORM_LDFLAGS) -I$(LUABINARIES_INCLUDEDIR) -DINCLUDE_LUA $(APACHE_INCLUDEFLAGS) $(APACHE_LIBFLAGS)
RINGS_CFLAGS=-I$(LUABINARIES_INCLUDEDIR)
ifeq ($(LUASQL_DRIVER), mysql)
   LUASQL_DRIVER_LIBS=$(MYSQL_LIBFLAGS)
   LUASQL_DRIVER_INCS=$(MYSQL_INCLUDEFLAGS)
endif
ifeq ($(LUASQL_DRIVER), oci8)
   LUASQL_DRIVER_LIBS=-L$(OCI8_LIBDIR) -lz -lclntsh
   LUASQL_DRIVER_INCS=-I$(OCI8_DEMODIR) -I$(OCI8_PUBLICDIR)
endif
ifeq ($(LUASQL_DRIVER), postgres)
   LUASQL_DRIVER_LIBS=$(POSTGRES_LIBFLAGS)
   LUASQL_DRIVER_INCS=$(POSTGRES_INCLUDEFLAGS)
endif
ifeq ($(LUASQL_DRIVER), sqlite)
   LUASQL_DRIVER_LIBS=-L$(SQLITE_LIBDIR) -lsqlite
   LUASQL_DRIVER_INCS=-I$(SQLITE_INCLUDEDIR)
endif
ifeq ($(LUASQL_DRIVER), sqlite3)
   LUASQL_DRIVER_LIBS=-L$(SQLITE3_LIBDIR) -lsqlite3
   LUASQL_DRIVER_INCS=-I$(SQLITE3_INCLUDEDIR)
endif
ifeq ($(LUASQL_DRIVER), odbc)
   LUASQL_DRIVER_LIBS= -L$(ODBC_LIBDIR) -lodbc
   LUASQL_DRIVER_INCS= -DUNIXODBC -I$(ODBC_INCLUDEDIR)
endif
LUASQL_LIBFLAGS=$(LIB_FLAG) $(LUASQL_DRIVER_LIBS)
LUASQL_CFLAGS=-I$(LUABINARIES_INCLUDEDIR) $(LUASQL_DRIVER_INCS)
LUAEXPAT_LIBFLAGS=$(LIB_FLAG) -L$(EXPAT_LIBDIR)
LUAEXPAT_CFLAGS=-I$(LUABINARIES_INCLUDEDIR) -I$(EXPAT_INCLUDEDIR)
LUAZIP_LIBFLAGS=$(LIB_FLAG) -L$(ZZIP_LIBDIR)
LUAZIP_CFLAGS=-I$(LUABINARIES_INCLUDEDIR) -I$(ZZIP_INCLUDEDIR)
MD5_FLAGS=-I$(LUABINARIES_INCLUDEDIR)

################
# Fetch locations
################

LUABINARIES_URL=http://www.lua.org/ftp
LUASOCKET_URL=http://www.keplerproject.org/luasocket
CVS_LUAFORGE=-d:pserver:anonymous:@cvs.luaforge.net:/cvsroot

XAVANTE_TAG=v1_3_1
CGILUA_TAG=v5_1_0
LUASQL_TAG=v2_1_0
COPAS_TAG=v1_1_1
LUAFILESYSTEM_TAG=v1_2_1
RINGS_TAG=v1_1_0
LUAEXPAT_TAG=v1_1_0
LUALOGGING_TAG=v1_1_3
LUAZIP_TAG=v1_2_3
MD5_TAG=v1_0_2

################
# Helper applications
################

WGET=wget -c -t0

# These are relative to root of kepler distribution
ifeq ($(BUILD_LUA),yes)
   LUAC=lua-$(LUABINARIES_VERSION)/src/luac$(LUA_SUFFIX)
else
   LUAC=luac$(LUA_SUFFIX)
endif
# bin2c is no longer included with Lua
BIN2C=src/bin2c$(LUA_SUFFIX)

################
# Platform selection
################

PLATFORM_BUILD=build

ifeq ($(shell bash -c 'echo $${OSTYPE%%[0-9]*}'),darwin)
   PLATFORM_BUILD=OSX-build
   ifneq ($(LUABINARIES_READLINE_LIBS),)
      LUABINARIES_READLINE_LIBS=-lreadline -L$(READLINE_LIBDIR)
   endif
endif

platform: $(PLATFORM_BUILD)

################
# Build rules
################

# Core packages plus selected launcher only

ifeq ($(BUILD_LUA),yes)
   CORE_BUILD=LuaBinaries-build 
else
   CORE_BUILD=
endif
ifeq ($(INSTALL_LUA),yes)
   CORE_BUILD=LuaBinaries-build 
   CORE_INSTALL=LuaBinaries-install 
else
   CORE_INSTALL=
endif

CORE_BUILD+= Kepler-build LuaSocket-build CGILua-build \
            Copas-build LuaFileSystem-build \
            Launcher-$(LAUNCHER)-build Rings-build
CORE_INSTALL+= Kepler-install LuaSocket-install CGILua-install \
            Copas-install LuaFileSystem-install \
            Launcher-$(LAUNCHER)-install Rings-install
CORE_CLEAN= Kepler-clean LuaBinaries-clean LuaSocket-clean CGILua-clean \
            Copas-clean LuaFileSystem-clean \
            Launcher-$(LAUNCHER)-clean Rings-clean
CORE_KLEAN= Kepler-klean LuaBinaries-klean LuaSocket-klean CGILua-klean \
            Copas-klean LuaFileSystem-klean \
            Launcher-cgi-klean Launcher-fastcgi-klean \
            Launcher-mod2-klean Launcher-xavante-klean Rings-klean

# Add selected extra packages

ifneq ($(shell echo $(OPTIONAL_PACKAGES) | grep lualogging),)
   EXTRA_BUILD+= LuaLogging-build
   EXTRA_INSTALL+= LuaLogging-install
   EXTRA_CLEAN+= LuaLogging-clean
   EXTRA_KLEAN+= LuaLogging-klean
endif
ifneq ($(shell echo $(OPTIONAL_PACKAGES) | grep luasql),)
   EXTRA_BUILD+= LuaSQL-build
   EXTRA_INSTALL+= LuaSQL-install
   EXTRA_CLEAN+= LuaSQL-clean
   EXTRA_KLEAN+= LuaSQL-klean
endif
ifneq ($(shell echo $(OPTIONAL_PACKAGES) | grep luaexpat),)
   EXTRA_BUILD+= LuaExpat-build
   EXTRA_INSTALL+= LuaExpat-install
   EXTRA_CLEAN+= LuaExpat-clean
   EXTRA_KLEAN+= LuaExpat-klean
endif
ifneq ($(shell echo $(OPTIONAL_PACKAGES) | grep luazip),)
   EXTRA_BUILD+= LuaZip-build
   EXTRA_INSTALL+= LuaZip-install
   EXTRA_CLEAN+= LuaZip-clean
   EXTRA_KLEAN+= LuaZip-klean
endif
ifneq ($(shell echo $(OPTIONAL_PACKAGES) | grep md5),)
   EXTRA_BUILD+= MD5-build
   EXTRA_INSTALL+= MD5-install
   EXTRA_CLEAN+= MD5-clean
   EXTRA_KLEAN+= MD5-klean
endif

.PHONY: build install clean klean dist snapshot Kepler-tarball \
        $(CORE_BUILD) $(CORE_INSTALL) $(CORE_CLEAN) $(CORE_KLEAN) \
        $(EXTRA_BUILD) $(EXTRA_INSTALL) $(EXTRA_CLEAN) $(EXTRA_KLEAN)

build: $(CORE_BUILD) $(EXTRA_BUILD)
install: $(CORE_INSTALL) $(EXTRA_INSTALL)
clean: $(CORE_CLEAN) $(EXTRA_CLEAN)
klean: $(CORE_KLEAN) $(EXTRA_KLEAN)

ALL_CLEAN=LuaBinaries-clean LuaSocket-clean CGILua-clean \
          Copas-clean LuaFileSystem-clean \
          Launcher-cgi-clean Launcher-fastcgi-clean \
          Launcher-mod2-clean Launcher-xavante-clean \
          Rings-clean LuaLogging-clean LuaSQL-clean \
          LuaExpat-clean LuaZip-clean MD5-clean \
          Kepler-clean

ALL_KLEAN=LuaBinaries-klean LuaSocket-klean CGILua-klean \
          Copas-klean LuaFileSystem-klean \
          Launcher-cgi-klean Launcher-fastcgi-klean \
          Launcher-mod2-klean Launcher-xavante-klean \
          Rings-klean LuaLogging-klean LuaSQL-klean \
          LuaExpat-klean LuaZip-klean MD5-klean \
          Kepler-klean

ALL_FETCH=LuaBinaries-fetch LuaSocket-fetch CGILua-fetch \
          Copas-fetch LuaFileSystem-fetch \
          Launcher-cgi-fetch Launcher-fastcgi-fetch \
          Launcher-mod2-fetch Launcher-xavante-fetch \
          Rings-fetch LuaLogging-fetch LuaSQL-fetch \
          LuaExpat-fetch LuaZip-fetch MD5-fetch \
          Kepler-fetch

KEPLER_DIST_DIR=kepler-$(KEPLER_VERSION)

KEPLER_DIST_FILES=Makefile config configure config.unix \
          lua-$(LUABINARIES_VERSION) luasocket-$(LUASOCKET_VERSION) \
          cgilua copas luafilesystem launcher rings \
          lualogging luasql luaexpat luazip md5 xavante src htdocs

dist: $(ALL_KLEAN) $(ALL_FETCH) Kepler-tarball

snapshot: $(ALL_CLEAN) $(ALL_FETCH) Kepler-tarball
	mv $(KEPLER_DIST_DIR).tar.gz $(KEPLER_DIST_DIR)-snapshot-`date +%Y%m%d-%H%M`.tar.gz
	mv $(KEPLER_DIST_DIR).zip $(KEPLER_DIST_DIR)-snapshot-`date +%Y%m%d-%H%M`.zip

Kepler-tarball:
	rm -rf $(KEPLER_DIST_DIR)
	mkdir -p $(KEPLER_DIST_DIR)
	./configure --enable-lua
	cp -RH $(KEPLER_DIST_FILES) $(KEPLER_DIST_DIR) 
	find $(KEPLER_DIST_DIR) -name CVS | xargs rm -rf
	tar -czf $(KEPLER_DIST_DIR).tar.gz $(KEPLER_DIST_DIR)
	zip -rq $(KEPLER_DIST_DIR).zip $(KEPLER_DIST_DIR)/*
	rm -rf $(KEPLER_DIST_DIR)

################
# Portability tweaks
################

OSX-build:
	MACOSX_DEPLOYMENT_TARGET=10.3 $(MAKE) \
	   LUABINARIES_MYCFLAGS="-DLUA_USE_MACOSX $(LUABINARIES_READLINE_CFLAGS)" \
	   LUABINARIES_MYLIBS="$(LUABINARIES_READLINE_LIBS)" \
	   PLATFORM_CFLAGS="-dynamic" \
	   PLATFORM_LDFLAGS="-lm -ldl" \
	   LIB_FLAG="-bundle -undefined dynamic_lookup -all_load" \
	   LUASOCKET_FLAGS="$(LUASOCKET_FLAGS) -DUNIX_HAS_SUN_LEN -fno-common" \
	   build

FreeBSD-build:
	$(MAKE) \
	   LUABINARIES_MYCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN" \
	   LUABINARIES_MYLIBS="-Wl,-E" \
	   PLATFORM_LDFLAGS="-lm" \
	   build

################

LUABINARIES_TESTFILE=lua-$(LUABINARIES_VERSION)/src/lua$(LUA_SUFFIX)

LuaBinaries-fetch: lua-$(LUABINARIES_VERSION)

lua-$(LUABINARIES_VERSION).tar.gz:
	$(WGET) $(LUABINARIES_URL)/$@

lua-$(LUABINARIES_VERSION):
	$(MAKE) lua-$(LUABINARIES_VERSION).tar.gz
	test -d $@ || tar zxvpf lua-$(LUABINARIES_VERSION).tar.gz
	cd lua-$(LUABINARIES_VERSION) && patch -p1 < ../lua-$(LUABINARIES_VERSION)-luabinaries.diff

LuaBinaries-build: $(LUABINARIES_TESTFILE)

$(LUABINARIES_TESTFILE): lua-$(LUABINARIES_VERSION)
	cd lua-$(LUABINARIES_VERSION) && $(MAKE) \
	   LUA_SUFFIX="$(LUA_SUFFIX)" \
	   INSTALL_INC="$(LUABINARIES_INSTALL_INCLUDEDIR)" \
	   MYCFLAGS="$(LUABINARIES_DEFPATH) $(LUABINARIES_MYCFLAGS)" \
	   MYLIBS="$(LUABINARIES_MYLIBS)" \
	   CC="$(CC)" \
	   generic

LuaBinaries-install: LuaBinaries-build
	cd lua-$(LUABINARIES_VERSION) && $(MAKE) \
	   LUA_SUFFIX="$(LUA_SUFFIX)" \
	   INSTALL_INC="$(LUABINARIES_INSTALL_INCLUDEDIR)" \
	   INSTALL_TOP="$(LUABINARIES_INSTALL_DIR)" \
	   MYCFLAGS="$(LUABINARIES_MYCFLAGS)" \
	   MYLIBS="$(LUABINARIES_MYLIBS)" \
	   INSTALL_EXEC="install -p -m 0755" \
	   INSTALL_DATA="install -p -m 0644" \
	   STRIP="true" \
	   install

LuaBinaries-clean:
	if [ -d lua-$(LUABINARIES_VERSION) ]; then \
	   cd lua-$(LUABINARIES_VERSION) && $(MAKE) \
	      LUA_SUFFIX="$(LUA_SUFFIX)" \
	      clean; \
	fi

LuaBinaries-klean:
	# rm -f lua-$(LUABINARIES_VERSION).tar.gz
	rm -rf lua-$(LUABINARIES_VERSION)

################

LUASOCKET_TESTFILE=luasocket-$(LUASOCKET_VERSION)/src/socket.so.$(LUASOCKET_VERSION)

LuaSocket-fetch: luasocket-$(LUASOCKET_VERSION)

luasocket-$(LUASOCKET_VERSION).tar.gz:
	$(WGET) $(LUASOCKET_URL)/$@

luasocket-$(LUASOCKET_VERSION):
	$(MAKE) luasocket-$(LUASOCKET_VERSION).tar.gz
	test -d $@ || tar zxvpf luasocket-$(LUASOCKET_VERSION).tar.gz
	cd luasocket-$(LUASOCKET_VERSION) && patch -p1 < ../luasocket-$(LUASOCKET_VERSION)-lua-5.1.diff

LuaSocket-build: $(LUASOCKET_TESTFILE)

$(LUASOCKET_TESTFILE): luasocket-$(LUASOCKET_VERSION)
	cd luasocket-$(LUASOCKET_VERSION) && $(MAKE) \
	   CC="$(CC)" \
	   CFLAGS+="$(LUASOCKET_FLAGS)" \
	   LDFLAGS="$(LUASOCKET_LDFLAGS)" \
	   LD="$(LUASOCKET_LD)"

LuaSocket-install: LuaSocket-build
	cd luasocket-$(LUASOCKET_VERSION) && $(MAKE) \
	   INSTALL_TOP_SHARE="$(LUASOCKET_INSTALL_DIR)/share/lua/5.1" \
	   INSTALL_TOP_LIB="$(LUASOCKET_INSTALL_DIR)/lib/lua/5.1" \
	   install

LuaSocket-clean:
	if [ -d luasocket-$(LUASOCKET_VERSION) ]; then \
	   cd luasocket-$(LUASOCKET_VERSION) && $(MAKE) clean; \
	fi

LuaSocket-klean:
	# rm -f luasocket-$(LUASOCKET_VERSION).tar.gz
	rm -rf luasocket-$(LUASOCKET_VERSION)

################

CGILUA_TESTFILE=cgilua/src/cgilua/cgilua.lua

CGILua-fetch: cgilua

cgilua:
	cvs $(CVS_LUAFORGE)/cgilua export -r $(CGILUA_TAG) cgilua

CGILua-build: $(CGILUA_TESTFILE)

$(CGILUA_TESTFILE): cgilua

CGILua-install: CGILua-build
	cd cgilua && $(MAKE) \
	   LUA_DIR="$(CGILUA_INSTALL_DIR)/share/lua/5.1" \
	   install
	cd cgilua && if [ ! -e "$(CGILUA_INSTALL_CONFDIR)"/cgilua/config.lua ]; \
	then \
	   mkdir -p "$(CGILUA_INSTALL_CONFDIR)"/cgilua; \
	   cp src/cgilua/config.lua "$(CGILUA_INSTALL_CONFDIR)"/cgilua; \
	fi


CGILua-clean:
	if [ -d cgilua ]; then \
	   cd cgilua && $(MAKE) clean; \
	fi

CGILua-klean:
	rm -rf cgilua

################

COPAS_TESTFILE=copas/src/copas/copas.lua

Copas-fetch: copas

copas:
	cvs $(CVS_LUAFORGE)/copas export -r $(COPAS_TAG) copas

Copas-build: $(COPAS_TESTFILE)

$(COPAS_TESTFILE): copas

Copas-install: Copas-build
	mkdir -p "$(COPAS_INSTALL_DIR)/share/lua/5.1"
	cp copas/src/copas/copas.lua "$(COPAS_INSTALL_DIR)/share/lua/5.1"

Copas-clean:

Copas-klean:
	rm -rf copas

################

LUAFILESYSTEM_TESTFILE=luafilesystem/src/lfs.o

LuaFileSystem-fetch: luafilesystem

luafilesystem:
	cvs $(CVS_LUAFORGE)/luafilesystem export -r $(LUAFILESYSTEM_TAG) luafilesystem

LuaFileSystem-build: $(LUAFILESYSTEM_TESTFILE)

$(LUAFILESYSTEM_TESTFILE): luafilesystem
	cd luafilesystem && $(MAKE) \
	   CC="$(CC)" \
	   LUA_INC=$(LUABINARIES_INCLUDEDIR) \
	   LIB_OPTION="$(LIB_FLAG)"

LuaFileSystem-install: $(LUAFILESYSTEM_TESTFILE)
	cd luafilesystem && $(MAKE) \
	   LUA_LIBDIR="$(LUAFILESYSTEM_INSTALL_DIR)/lib/lua/5.1" \
	   install

LuaFileSystem-clean:
	if [ -d luafilesystem ]; then \
	   cd luafilesystem && $(MAKE) clean; \
	fi

LuaFileSystem-klean:
	rm -rf luafilesystem

################

# built-in
Launcher-cgi-fetch:

Launcher-cgi-build: launcher/cgi/cgi

launcher/cgi/cgi.lua: launcher/cgi/t_cgi.lua
	sed -e "s|\[\[KEPLER_INIT\]\]|\[\[$(KEPLER_INIT)\]\]|" < $< > $@

launcher/cgi/cgi.lc: launcher/cgi/cgi.lua
	$(LUAC) -o $@ $<

launcher/cgi/cgi.lch: launcher/cgi/cgi.lc
	$(BIN2C) $< | sed -e "s|lua_pcall|status=lua_pcall|" > $@

launcher/cgi/cgi: launcher/cgi/cgi.lch
	$(CC) $(LAUNCHER_CGI_FLAGS) -Ilauncher/cgi -o $@ launcher/cgi/cgi.c $(LUABINARIES_LIB)

Launcher-cgi-install: Launcher-cgi-build
	mkdir -p $(LAUNCHER_CGI_INSTALL_DIR)/share/lua/5.1/sapi/cgi
	cp launcher/cgi/sapi/*lua $(LAUNCHER_CGI_INSTALL_DIR)/share/lua/5.1/sapi/cgi
	mkdir -p $(LAUNCHER_CGI_INSTALL_BINDIR)
	cp launcher/cgi/cgi $(LAUNCHER_CGI_INSTALL_BINDIR)

Launcher-cgi-clean:
	cd launcher/cgi && rm -f cgi cgi.o cgi.lch cgi.lc cgi.lua

Launcher-cgi-klean: Launcher-cgi-clean
	cd launcher/cgi && cvs up -dP

################

# built-in
Launcher-fastcgi-fetch:

Launcher-fastcgi-build: launcher/fastcgi/fastcgi launcher/fastcgi/lfcgi.so launcher/fastcgi/fastcgi.lua launcher/fastcgi/fastcgi.conf

launcher/fastcgi/fastcgi.lua: launcher/fastcgi/t_fastcgi.lua
	sed -e "s|\[\[KEPLER_INIT\]\]|\[\[$(KEPLER_INIT)\]\]|" < $< > $@

launcher/fastcgi/fastcgi.lc: launcher/fastcgi/fastcgi.lua
	$(LUAC) -o $@ $<

launcher/fastcgi/fastcgi.lch: launcher/fastcgi/fastcgi.lc
	$(BIN2C) $< | sed -e "s|lua_pcall|status=lua_pcall|" > $@

launcher/fastcgi/fastcgi: launcher/fastcgi/fastcgi.lch launcher/fastcgi/fastcgi.c
	$(CC) $(LAUNCHER_FASTCGI_FLAGS) -o $@ launcher/fastcgi/fastcgi.c $(LUABINARIES_LIB)

launcher/fastcgi/lfcgi.so: launcher/fastcgi/lfcgi.c
	$(CC) $(LIB_FLAG) $(LAUNCHER_FASTCGI_FLAGS) -o $@ $< -lfcgi

launcher/fastcgi/fastcgi.conf: launcher/fastcgi/t_fastcgi.conf
	sed -e "s|FCGI_DIR|$(LAUNCHER_FASTCGI_INSTALL_BINDIR)|" < $< > $@

Launcher-fastcgi-install: Launcher-fastcgi-build
	mkdir -p $(LAUNCHER_FASTCGI_INSTALL_DIR)/lib/lua/5.1
	cp launcher/fastcgi/lfcgi.so $(LAUNCHER_FASTCGI_INSTALL_DIR)/lib/lua/5.1
	mkdir -p $(LAUNCHER_FASTCGI_INSTALL_DIR)/share/lua/5.1/sapi/fastcgi
	cp launcher/fastcgi/sapi/*lua $(LAUNCHER_FASTCGI_INSTALL_DIR)/share/lua/5.1/sapi/fastcgi
	mkdir -p $(LAUNCHER_FASTCGI_INSTALL_BINDIR)
	cp launcher/fastcgi/fastcgi $(LAUNCHER_FASTCGI_INSTALL_BINDIR)

Launcher-fastcgi-clean:
	cd launcher/fastcgi && rm -f fastcgi.lc fastcgi.lch fastcgi lfcgi.so fastcgi.lua fastcgi.conf

Launcher-fastcgi-klean: Launcher-fastcgi-clean
	cd launcher/fastcgi && cvs up -dP

################

# built-in
Launcher-mod2-fetch:

Launcher-mod2-build: launcher/mod2/mod_lua.so launcher/mod2/mod2.conf

launcher/mod2/mod2.lua: launcher/mod2/t_mod2.lua
	sed -e "s|\[\[KEPLER_INIT\]\]|\[\[$(KEPLER_INIT)\]\]|" < $< > $@

launcher/mod2/mod2.lc: launcher/mod2/mod2.lua
	$(LUAC) -o $@ $<

launcher/mod2/mod2.lch: launcher/mod2/mod2.lc
	$(BIN2C) $< | sed -e 's|^.*luaL_loadbuffer.*;|status=luaL_loadbuffer(L,(const char*)B1,sizeof(B1),"launcher/mod2/mod2.lc");if(status==0){lua_pushlightuserdata(L,(void*)r);status=lua_pcall(L,1,1,0);}|' > $@

launcher/mod2/mod_lua.so: launcher/mod2/mod2.lch launcher/mod2/mod_lua.c
	$(CC) $(LAUNCHER_MOD2_LIBFLAGS) -o $@ launcher/mod2/mod_lua.c launcher/mod2/apache2.c $(LUABINARIES_LIB) -DLUA_STATE_PER_REQUEST -DKEPLER_INIT='"$(KEPLER_INIT)"'

launcher/mod2/mod2.conf: launcher/mod2/t_mod2.conf
	sed -e "s|CGILUA_DIR|$(LAUNCHER_MOD2_APACHE_MODDIR)|" < $< > $@

Launcher-mod2-install: Launcher-mod2-build
	mkdir -p $(LAUNCHER_MOD2_INSTALL_DIR)/share/lua/5.1/sapi/mod2
	cp launcher/mod2/sapi/*lua $(LAUNCHER_MOD2_INSTALL_DIR)/share/lua/5.1/sapi/mod2
	cp launcher/mod2/mod_lua.so $(LAUNCHER_MOD2_INSTALL_APACHE_MODDIR)

Launcher-mod2-clean:
	rm -f launcher/mod2/mod2.conf launcher/mod2/mod_lua.so launcher/mod2/mod2.lch launcher/mod2/mod2.lc launcher/mod2/mod2.lua

Launcher-mod2-klean: Launcher-mod2-clean
	cd launcher/mod2 && cvs up -dP

################

XAVANTE_TESTFILE=xavante/src/xavante_start.lua

Launcher-xavante-fetch: xavante

xavante:
	cvs $(CVS_LUAFORGE)/xavante export -r $(XAVANTE_TAG) xavante

Launcher-xavante-build: $(XAVANTE_TESTFILE)

# XXX Will move into Kepler
$(XAVANTE_TESTFILE): xavante
	cd xavante && $(MAKE) \
	   LUA_LIBDIR="$(XAVANTE_INSTALL_DIR)/lib/lua/5.1" \
	   LUA_DIR="$(XAVANTE_INSTALL_DIR)/share/lua/5.1" \
	   SYS_BINDIR="$(XAVANTE_INSTALL_BINDIR)" \
	   XAVANTE_CONF="$(XAVANTE_INSTALL_CONFDIR)" \
	   XAVANTE_WEB="$(XAVANTE_INSTALL_WEBDIR)" \
	   XAVANTE_INIT="$(KEPLER_INIT)" \
	   src/xavante_start.lua
	mv xavante/src/xavante_start.lua xavante/src/xavante_start.lua.old
	sed -e "s|#!/usr/bin/env lua|#!/usr/bin/env lua$(LUA_SUFFIX)|" xavante/src/xavante_start.lua.old > xavante/src/xavante_start.lua
	chmod +x xavante/src/xavante_start.lua

Launcher-xavante-install: Launcher-xavante-build
	cd xavante && $(MAKE) \
	   LUA_DIR="$(XAVANTE_INSTALL_DIR)/share/lua/5.1" \
	   XAVANTE_WEB="$(XAVANTE_INSTALL_WEBDIR)" \
	   install
	mkdir -p "$(XAVANTE_INSTALL_BINDIR)"
	cp xavante/src/xavante_start.lua "$(XAVANTE_INSTALL_BINDIR)"
	if [ ! -e "$(XAVANTE_INSTALL_CONFDIR)"/xavante/config.lua ]; \
	then \
	   mkdir -p "$(XAVANTE_INSTALL_CONFDIR)"/xavante; \
	   cp src/xavante/config.lua "$(XAVANTE_INSTALL_CONFDIR)"/xavante; \
	fi


Launcher-xavante-clean:
	if [ -d xavante ]; then \
	   cd xavante && $(MAKE) clean; \
	fi

Launcher-xavante-klean:
	rm -rf xavante

################

RINGS_TESTFILE=rings/src/rings.o

Rings-fetch: rings

rings:
	cvs $(CVS_LUAFORGE)/rings export -r $(RINGS_TAG) rings

Rings-build: $(RINGS_TESTFILE)

$(RINGS_TESTFILE): rings
	cd rings && $(MAKE) \
	   CC="$(CC)" \
	   OBJS="src/rings.o" \
	   LIB_OPTION="$(LIB_FLAG)" \
	   CFLAGS+="$(RINGS_CFLAGS)"

Rings-install: $(RINGS_TESTFILE)
	cd rings && $(MAKE) \
	   LUA_LIBDIR="$(RINGS_INSTALL_DIR)/lib/lua/5.1" \
	   LUA_DIR=$(RINGS_INSTALL_DIR)/share/lua/5.1 \
	   install

Rings-clean:
	if [ -d rings ]; then \
	   cd rings && $(MAKE) clean; \
	fi

Rings-klean:
	rm -rf rings

################

LUALOGGING_TESTFILE=lualogging/src/lualogging.lua

LuaLogging-fetch: lualogging

lualogging:
	cvs $(CVS_LUAFORGE)/lualogging export -r $(LUALOGGING_TAG) lualogging

LuaLogging-build: $(LUALOGGING_TESTFILE)

$(LUALOGGING_TESTFILE): lualogging

LuaLogging-install: LuaLogging-build
	cd lualogging && $(MAKE) \
	   LUA_DIR="$(LUALOGGING_INSTALL_DIR)/share/lua/5.1" \
	   install

LuaLogging-clean:

LuaLogging-klean:
	rm -rf lualogging

################

LUASQL_TESTFILE=luasql/src/ls_$(LUASQL_DRIVER).o

LuaSQL-fetch: luasql

luasql:
	cvs $(CVS_LUAFORGE)/luasql export -r $(LUASQL_TAG) luasql

LuaSQL-build: $(LUASQL_TESTFILE)

$(LUASQL_TESTFILE): luasql
	cd luasql && $(MAKE) \
	   CC="$(CC)" \
	   T="$(LUASQL_DRIVER)" \
	   LIB_OPTION="$(LUASQL_LIBFLAGS)" \
	   CFLAGS+="$(LUASQL_CFLAGS)" \
	   DRIVER_LIBS=""

LuaSQL-install: LuaSQL-build
	cd luasql && $(MAKE) \
	   T="$(LUASQL_DRIVER)" \
	   LIB_OPTION="$(LUASQL_LIBFLAGS)" \
	   CFLAGS+="$(LUASQL_CFLAGS)" \
	   LUA_LIBDIR="$(LUASQL_INSTALL_DIR)/lib/lua/5.1" \
	   install

LuaSQL-clean:
	if [ -d luasql ]; then \
	cd luasql && $(MAKE) \
	   T="$(LUASQL_DRIVER)" \
	   clean; \
	fi

LuaSQL-klean:
	rm -rf luasql

################

LUAEXPAT_TESTFILE=luaexpat/src/lxplib.o

LuaExpat-fetch: luaexpat

luaexpat:
	cvs $(CVS_LUAFORGE)/luaexpat export -r $(LUAEXPAT_TAG) luaexpat

LuaExpat-build: $(LUAEXPAT_TESTFILE)

$(LUAEXPAT_TESTFILE): luaexpat
	cd luaexpat && $(MAKE) \
	   CC="$(CC)" \
	   LUA_VERSION_NUM="501" \
	   LIB_OPTION="$(LUAEXPAT_LIBFLAGS)" \
	   CFLAGS+="$(LUAEXPAT_CFLAGS)" \
	   lib

LuaExpat-install: LuaExpat-build
	cd luaexpat && $(MAKE) \
	   LUA_VERSION_NUM="501" \
	   LUA_LIBDIR="$(LUAEXPAT_INSTALL_DIR)/lib/lua/5.1" \
	   LUA_DIR="$(LUAEXPAT_INSTALL_DIR)/share/lua/5.1" \
	   install

LuaExpat-clean:
	if [ -d luaexpat ]; then \
	   cd luaexpat && $(MAKE) clean; \
	fi

LuaExpat-klean:
	rm -rf luaexpat

################

LUAZIP_TESTFILE=luazip/src/luazip.o

LuaZip-fetch: luazip

luazip:
	cvs $(CVS_LUAFORGE)/luazip export -r $(LUAZIP_TAG) luazip

LuaZip-build: $(LUAZIP_TESTFILE)

$(LUAZIP_TESTFILE): luazip
	cd luazip && $(MAKE) \
	   CC="$(CC)" \
	   LUA_VERSION_NUM="501" \
	   LIB_OPTION="$(LUAZIP_LIBFLAGS)" \
	   CFLAGS+="$(LUAZIP_CFLAGS)" \
	   lib

LuaZip-install: LuaZip-build
	cd luazip && $(MAKE) \
	   LUA_VERSION_NUM="501" \
	   LUA_LIBDIR="$(LUAZIP_INSTALL_DIR)/lib/lua/5.1" \
	   install

LuaZip-clean:
	if [ -d luazip ]; then \
	   cd luazip && $(MAKE) clean; \
	fi

LuaZip-klean:
	rm -rf luazip

################

MD5_TESTFILE=md5/src/md5.o

MD5-fetch: md5

md5:
	cvs $(CVS_LUAFORGE)/md5 export -r $(MD5_TAG) md5

MD5-build: $(MD5_TESTFILE)

$(MD5_TESTFILE): md5
	cd md5 && $(MAKE) \
	   CC="$(CC)" \
	   LUA_VERSION_NUM="501" \
	   LIB_OPTION="$(LIB_FLAG)" \
	   CFLAGS+="$(MD5_FLAGS)"

MD5-install: MD5-build
	cd md5 && $(MAKE) \
	   LUA_VERSION_NUM="501" \
	   LUA_LIBDIR="$(MD5_INSTALL_DIR)/lib/lua/5.1" \
	   LUA_DIR="$(MD5_INSTALL_DIR)/share/lua/5.1" \
	   install

MD5-clean:
	if [ -d md5 ]; then \
	   cd md5 && $(MAKE) clean; \
	fi

MD5-klean:
	rm -rf md5

################

KEPLER_TESTFILE=src/kepler_init.lua

Kepler-fetch:

Kepler-build: $(KEPLER_TESTFILE)

$(KEPLER_TESTFILE):
	cd src && $(MAKE) \
	   LUA_SUFFIX="$(LUA_SUFFIX)" \
	   LUA_LIBDIR="$(KEPLER_INSTALL_DIR)/lib/lua/5.1" \
	   LUA_DIR="$(KEPLER_INSTALL_DIR)/share/lua/5.1" \
	   KEPLER_CONF="$(KEPLER_CONF)" \
	   XAVANTE_WEB="$(XAVANTE_INSTALL_WEBDIR)" \
	   CGILUA_APPS="$(CGILUA_APPS)"

Kepler-install: Kepler-build
	cd src && $(MAKE) \
	   LUA_SUFFIX="$(LUA_SUFFIX)" \
	   SYS_BINDIR="$(KEPLER_INSTALL_BINDIR)" \
	   KEPLER_ROOT=$(shell dirname "$(KEPLER_INIT)") \
	   KEPLER_INIT="$(KEPLER_INIT)" \
	   install
	mkdir -p "$(KEPLER_WEB)"
	cp -p -R xavante/web/* "$(KEPLER_WEB)"
	mkdir -p "$(KEPLER_WEB)/doc"
	for module in cgilua copas luafilesystem rings `echo $(OPTIONAL_PACKAGES) | sed 's/,/ /g'` `[ "$(LAUNCHER)" = xavante ] && echo xavante` ; \
	do mkdir -p "$(KEPLER_WEB)/doc/$$module" ;\
	   cp -p -R $$module/doc/us/* "$(KEPLER_WEB)/doc/$$module" ;\
	done
	mkdir -p "$(KEPLER_WEB)/doc/luasocket"
	cp -p -R luasocket-$(LUASOCKET_VERSION)/doc/* "$(KEPLER_WEB)/doc/luasocket"
	cp "$(KEPLER_WEB)/doc/luasocket/home.html" "$(KEPLER_WEB)/doc/luasocket/index.html"

Kepler-clean:
	cd src && $(MAKE) clean \
	   LUA_SUFFIX="$(LUA_SUFFIX)"

Kepler-klean: Kepler-clean
	cd src && cvs up -dP

################
