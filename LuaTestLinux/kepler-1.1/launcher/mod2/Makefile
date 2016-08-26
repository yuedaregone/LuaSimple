# $Id: Makefile,v 1.13 2006/11/24 17:56:16 mascarenhas Exp $

T= mod2

include $(CONFIG)

MT= t_$T.lua
ML= $T.lua
MC= $T.lc
MH= $T.lch
T_CONF= t_$T.conf
CONF= $T.conf
MOD_LUA= mod_lua.so
#APACHE2_LIB= apache2.so
SAPI_DIR= $(LUA_DIR)/sapi/mod2

MOD_HDRS= mod_lua.h compat-5.1.h apache2.h
MOD_SRCS= mod_lua.c compat-5.1.c apache2.c
MOD_OBJS= mod_lua.o compat-5.1.o apache2.o
#AP2_HDRS= apache2.h compat-5.1.h
#AP2_SRCS= apache2.c compat-5.1.c
#AP2_OBJS= apache2.o compat-5.1.o

CFLAGS+= $(WARN) $(INCS) $(CGILUA_DEFS) $(MOD_DEFS) -I$(APACHE2_INC) -I$(APACHE2_APR_INC)


build $(MOD_LUA): $(MH) $(CONF) $(MOD_OBJS) $(APACHE2_LIB)
	export MACOSX_DEPLOYMENT_TARGET="10.3"; $(CC) $(CFLAGS) $(LIB_OPTION) -o $(MOD_LUA) $(MOD_OBJS) $(SYS_LIBDIR)/liblua50.a

#$(APACHE2_LIB): $(AP2_OBJS)
#	export MACOSX_DEPLOYMENT_TARGET="10.3"; $(CC) $(CFLAGS) $(LIB_OPTION) -o $(APACHE2_LIB) $(AP2_OBJS)

compat-5.1.c:
	cp $(COMPAT_DIR)/compat-5.1.c .

compat-5.1.h:
	cp $(COMPAT_DIR)/compat-5.1.h .

compat-5.1.o: compat-5.1.c compat-5.1.h
	$(CC) $(CFLAGS) -c -o compat-5.1.o compat-5.1.c

apache2.o: apache2.c apache2.h
	$(CC) $(CFLAGS) -c -o apache2.o apache2.c

$(ML): $(MT)
	sed -e "s|\[\[KEPLER_INIT\]\]|\[\[$(KEPLER_INIT)\]\]|" < $(MT) > $(ML)

$(MC): $(ML)
	$(LUAC) -o $@ $(ML)

$(MH): $(MC)
	$(BIN2C) $(MC) | sed -e "s|lua_dobuffer|status=lua_dobuffer|" > $@

$(CONF): $(T_CONF)
	sed -e "s|CGILUA_DIR|$(CGILUA_DIR)|" < $(T_CONF) > $@

install: $(ML) $(CONF) $(MOD_LUA) $(APACHE2_LIB)
	mkdir -p $(LUA_LIBDIR)
#	cp $(APACHE2_LIB) $(LUA_LIBDIR)
	mkdir -p $(SAPI_DIR)
	cp sapi/*lua $(SAPI_DIR)
	cp $(MOD_LUA) $(APACHE2_MODDIR)
	if [ ! $(CGILUA_DEFS) ] ; then cp $(ML) $(APACHE2_MODDIR); fi;

clean:
	rm -f *.o *.lo *.slo *.la $(MH) $(MC) $(ML) $(CONF) $(MOD_LUA) $(APACHE2_LIB) compat-5.1.?
