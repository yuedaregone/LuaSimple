#include <stdio.h>
#ifdef _WIN
	#include <windows.h>
#else
	#include <unistd.h>
#endif
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "clib/http.h"

static int my_add(lua_State* L)
{
	int a = luaL_checknumber(L, 1);
	int b = luaL_checknumber(L, 2);
	lua_pushnumber(L, a+b);
	return 1;
}

///test callback
static int my_test_callback(lua_State* L)
{
	int handle = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_rawgeti(L, LUA_REGISTRYINDEX, handle);
	lua_pushstring(L, "Hello");
	lua_pcall(L, 1, 0, 0);
	return 0;
}

static const luaL_Reg MyLib[] = {
	{"my_add", my_add},
	{"my_test_callback", my_test_callback},
	{NULL, NULL}
};

static void luaopen_mylib(lua_State* L)
{
	// 5.1
	//luaL_register(L, "MyLib", MyLib);
	// 5.2
	luaL_newlib(L, MyLib);
    lua_setglobal(L, "MyLib");
}

int get_delay(lua_State* L)
{
	lua_getglobal(L, "get_delay");
	lua_pcall(L, 0, 1, 0);
	return (int)lua_tonumber(L, -1);
}

void onInit(lua_State* L)
{
	if (lua_getglobal(L, "init_program") == LUA_TFUNCTION)
	{
		lua_pcall(L, 0, 1, 0);
	}
}

void onClose(lua_State* L)
{
	if (lua_getglobal(L, "close_program") == LUA_TFUNCTION)
	{
		lua_pcall(L, 0, 1, 0);
	}
}

int main(int argc, char *argv[]) {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
    luaopen_mylib(L);
	luaopen_http(L);

	if (luaL_loadfile(L, "./main.lua") || lua_pcall(L, 0, 0, 0))
	{
		printf("LUA ERROR:%s", lua_tostring(L, -1));
		lua_close(L);
		return -1;
	}

	onInit(L);
	int delay = get_delay(L);
	int loop = 1;
	int index = 0;
	while(loop == 1)
	{
		lua_getglobal(L, "main_loop");
		lua_pcall(L, 0, 1, 0);
		loop = lua_tointeger(L, -1);
#ifdef _WIN
		Sleep(delay);
#else
		usleep(delay);
#endif
	}
	onClose(L);
	return 0;
}
