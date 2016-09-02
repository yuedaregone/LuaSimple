/*Manager lua libs*/
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
static int open_curl(lua_State* L)
{
	luaopen_luacurl(L);
	return 0;
}

static const luaL_Reg openlibs[] = {
	{"open_curl", open_curl},
	{NULL, NULL}
};

extern void luaopen_lualibmgr(lua_State* L)
{
	luaL_register(L, "lualibmgr", openlibs);
}
