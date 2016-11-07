DEFS= -D_WIN
CFLAGS= $(DEFS)
CLINK= -lm -lcurl
LUA_PATH= 3rd/lua-5.3.3/src
LUAFLAGS= -I$(LUA_PATH) -L$(LUA_PATH)
LUALIB= $(LUA_PATH)/liblua.a
VPATH= ./clib

OUT= main
OBJS= main.o str.o http.o


$(OUT): $(OBJS)
	cd $(LUA_PATH) && $(MAKE) generic
	$(CC) $(CFLAGS) $(OBJS) $(LUALIB) -o $(OUT) $(CLINK)
	
clean:
	cd $(LUA_PATH) && $(MAKE) $@
	rm -f $(OUT) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -I$(LUA_PATH) -c -o $@ $<
	
main.o: main.c http.h
http.o: http.c http.h str.h
str.o: str.c str.h