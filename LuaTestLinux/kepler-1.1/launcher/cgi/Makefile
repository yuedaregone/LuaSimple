# $Id: Makefile,v 1.3 2006/08/10 19:27:36 mascarenhas Exp $

T= cgi

include $(CONFIG)

SRCS= $T.c
OBJS= $T.o
CT= t_$T.lua
CL= cgi.lua
CGIC= cgi.lc
CH= cgi.lch
SAPI_DIR= $(LUA_DIR)/sapi/cgi

LIBS= $(LUA_LIBS) $(DLLIB)


$T: $(CH) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

build: $T

$(CL): $(CT)
	sed -e "s|\[\[KEPLER_INIT\]\]|\[\[$(KEPLER_INIT)\]\]|" < $(CT) > $(CL)

$(CGIC): $(CL)
	$(LUAC) -o $@ $(CL)

$(CH): $(CGIC)
	$(BIN2C) $(CGIC) | sed -e "s|lua_dobuffer|status=lua_dobuffer|" > $@

install: $T $(CL)
	mkdir -p $(SAPI_DIR)
	cp sapi/*lua $(SAPI_DIR)
	mkdir -p $(CGI_DIR)
	cp $(CL) $(CGI_DIR)
	cp $T $(CGI_DIR)/cgi

clean:
	rm -f $T $(CL) $(CGIC) $(CH) $(OBJS)
