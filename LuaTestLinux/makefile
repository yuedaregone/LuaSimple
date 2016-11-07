DEFS= -D_WIN
CFLAGS= $(DEFS)
LUA_PATH= 3rd/lua-5.3.3/src
LUAFLAGS= -I$(LUA_PATH) -L$(LUA_PATH)
LUALIB= $(LUA_PATH)/liblua.a
OUT= main
OBJS= main.o

$(OUT): $(OBJS)
	cd $(LUA_PATH) && $(MAKE) generic
	$(CC) $(CFLAGS) $(OBJS) $(LUALIB) -o $(OUT) -lm
	
clean:
	cd $(LUA_PATH) && $(MAKE) $@
	rm -f $(OUT) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -I$(LUA_PATH) -c -o $@ $<
	
main.o: main.c
