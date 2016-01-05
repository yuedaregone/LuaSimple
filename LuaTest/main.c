#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
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
	luaL_register(L, "MyLib", MyLib);
	/* 5.2
	luaL_newlib(L, MyLib);
    lua_setglobal(L, "MyLib");
	*/
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
	
	lua_getglobal(L, "get_delay");	
	lua_pcall(L, 0, 1, 0);
	int delay = lua_tonumber(L, -1);
	
	int loop = 1;
	while(loop == 1)
	{		
		lua_getglobal(L, "main_loop");	
		lua_pcall(L, 0, 1, 0);
		loop = lua_toboolean(L, -1);		
		Sleep(delay);		
	}	
	return 0;
}
