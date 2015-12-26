#include <stdio.h>
#include <stdlib.h>
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

static int my_add(lua_State* L)
{
	int a = luaL_checknumber(L, 1);
	int b = luaL_checknumber(L, 2);
	lua_pushnumber(L, a+b);
	return 1;
}

static const luaL_Reg MyLib[] = {
	{"my_add", my_add},
	{NULL, NULL}
};

static void luaopen_mylib(lua_State* L)
{
	luaL_newlib(L, MyLib);
    lua_setglobal(L, "MyLib");
}

int main(int argc, char *argv[]) {
	lua_State* L = luaL_newstate();		
	luaL_openlibs(L);
    luaopen_mylib(L);
    
	if (luaL_loadfile(L, "./main.lua") || lua_pcall(L, 0, 0, 0))
	{
		printf("LUA ERROR:%s", lua_tostring(L, -1));
		lua_close(L);
		return -1;
	}
	
	lua_getglobal(L, "my_lua_fun");
	lua_pcall(L, 0, 0, 0);
	
	
	lua_getglobal(L, "my_lua_add");
	lua_pushnumber(L, 10);
	lua_pushnumber(L, 20);
	lua_pcall(L, 2, 1, 0);
	int param = lua_tonumber(L, -1);
	printf("the result is %d", param);
	return 0;
}
