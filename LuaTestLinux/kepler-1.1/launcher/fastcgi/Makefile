# $Id: Makefile,v 1.7 2006/08/10 19:27:36 mascarenhas Exp $

T= fastcgi

include $(CONFIG)

SRCS= $T.c lfcgi.c lfcgi.h $(COMPAT_DIR)/compat-5.1.c
OBJS= $T.o $(COMPAT_DIR)/compat-5.1.o
LFCGI_OBJS= lfcgi.o $(COMPAT_DIR)/compat-5.1.o
LFCGI_LIB= lfcgi.so
FT= t_$T.lua
FL= $T.lua
FC= $T.lc
FH= $T.lch
T_CONF= t_$T.conf
CONF= $T.conf
SAPI_DIR= $(LUA_DIR)/sapi/fastcgi

FCGI_LIB= -lfcgi

LIBS= $(LUA_LIBS) $(DLLIB)


$T: $(FH) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

build: $T $(LFCGI_LIB)

$(LFCGI_LIB): $(LFCGI_OBJS)
	export MACOSX_DEPLOYMENT_TARGET="10.3"; $(CC) $(CFLAGS) $(LIB_OPTION) -o $(LFCGI_LIB) $(LFCGI_OBJS) $(FCGI_LIB)

$(FL): $(FT)
	sed -e "s|\[\[KEPLER_INIT\]\]|\[\[$(KEPLER_INIT)\]\]|" < $(FT) > $(FL)

$(FC): $(FL)
	$(LUAC) -o $@ $(FL)

$(FH): $(FC)
	$(BIN2C) $(FC) | sed -e "s|lua_dobuffer|status=lua_dobuffer|" > $@

$(CONF): $(T_CONF)
	sed -e "s|FCGI_DIR|$(FCGI_DIR)|" < $(T_CONF)  > $@

install: $T $(FL) $(CONF)
	mkdir -p $(LUA_LIBDIR)
	cp $(LFCGI_LIB) $(LUA_LIBDIR)
	mkdir -p $(SAPI_DIR)
	cp sapi/*lua $(SAPI_DIR)
	mkdir -p $(FCGI_DIR)
	cp $T $(FL) $(FCGI_DIR)

clean:
	rm -f $T $(FL) $(FC) $(FH) $(OBJS) $(LFCGI_OBJS) $(CONF) $(LFCGI_LIB)
