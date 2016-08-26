# $Id: Makefile,v 1.9 2007/06/06 18:31:39 hisham Exp $

T= kepler_init

IT= t_$T.lua
IL= $T.lua
BIN2C= bin2c$(LUA_SUFFIX)

all: $(IL) $(BIN2C)

$(IL): $(IT)
	sed -e "s|\[\[LUABASE51\]\]|\[\[$(LUA_DIR)\]\]|" \
	    -e "s|\[\[LIBBASE51\]\]|\[\[$(LUA_LIBDIR)\]\]|" \
	    -e "s|\[\[KEPLER_CONF\]\]|\[\[$(KEPLER_CONF)\]\]|" \
	    -e "s|\[\[LIB_EXT\]\]|\[\[so\]\]|" \
	    -e "s|\[\[XAVANTE_WEB\]\]|\[\[$(XAVANTE_WEB)\]\]|" \
	    -e "s|\[\[CGILUA_APPS\]\]|\[\[$(CGILUA_APPS)\]\]|" \
	    < $(IT) > $(IL)

$(BIN2C): bin2c.c
	$(CC) $(CFLAGS) -o $(BIN2C) bin2c.c

install: $(IL) $(BIN2C)
	mkdir -p $(KEPLER_ROOT)
	cp $(IL) $(KEPLER_INIT)
	mkdir -p $(SYS_BINDIR)
	cp $(BIN2C) $(SYS_BINDIR)

clean:
	rm -f $(IL) $(BIN2C)
