#include <stdio.h>
#include <stdlib.h>
#include "lua/lua.h"
#include <lua/lualib.h>
#include <lua/lauxlib.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
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




int main(int argc, char *argv[]) {
	lua_State* L = luaL_newstate();		
	luaL_openlibs(L);
	
	//luaL_newlib(L, MyLib);
	//luaL_requiref(L, , lib->func, 1);
    //lua_pop(L, 1); 
    //lua_register(L, "MyLib", MyLib);
    
    
	
	if (luaL_loadfile(L, "./main.lua") || lua_pcall(L, 0, 0, 0))
	{
		printf("LUA ERROR:%s", lua_tostring(L, -1));
		lua_close(L);
		return -1;
	}	
	return 0;
}
