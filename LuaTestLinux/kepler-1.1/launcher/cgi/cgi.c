/*
** Simple Lua interpreter.
** This program is used to run a Lua file with the same name but the
** extension (that should be .lua).
** It creates a Lua state, opens all its standard libraries, and run
** the Lua file in a protected environment just to redirect the error
** messages to stdout and stderr.
**
** $Id: cgi.c,v 1.2 2007/01/10 19:57:54 mascarenhas Exp $
*/

#include <string.h>
#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

#define MAX_CGI_NAME 256


/*
** Report error message.
** Assumes that the error message is on top of the stack.
*/
static int report (lua_State *L) {
	fprintf (stderr, "lua: fatal error: `%s'\n", lua_tostring (L, -1));
	fflush (stderr);
	printf ("Content-type: text/plain\n\nConfiguration fatal error: see error log!\n");
	printf ("%s", lua_tostring(L, -1));
	return 1;
}

static int runlua (lua_State *L, char *name) {
#ifdef INCLUDE_LUA
	int status;
	(void)name; /* avoid "unused parameter" warning */
	status = 1;
	#include "cgi.lch"
	return status;
#else
	int err_func;
	char buff[MAX_CGI_NAME+1];
	char *point = strrchr (name, '.');
	if ((strlen(name) + strlen(".lua")) > MAX_CGI_NAME) {
		lua_pushliteral (L, "Path name too long");
		return 1;
	}

	if (point && !strpbrk(point, ":/\\")) {
		int pos = point - name;
		strncpy (buff, name, pos+1);
		strcpy (buff + pos + 1, "lua");
	}
	else {
		strcpy (buff, name);
		strcat (buff, ".lua");
	}
	/* change stdin to binary mode (needed only if WIN32) */
#ifdef WIN32
	_setmode(_fileno(stdin), _O_BINARY);
#endif
	lua_pushliteral(L, "_TRACEBACK");
	lua_rawget(L, LUA_GLOBALSINDEX);  /* get traceback function */
	err_func = lua_gettop (L);
	return (luaL_loadfile (L, buff)) || (lua_pcall (L, 0, 0, err_func));
#endif
}


/*
** MAIN
*/
int main (int argc, char *argv[]) {
	lua_State *L = lua_open();
	(void)argc; /* avoid "unused parameter" warning */
	luaL_openlibs(L);
	if (runlua (L, argv[0])) {
		report (L);
		lua_close (L);
		return EXIT_FAILURE;
	}
	lua_close (L);
	return EXIT_SUCCESS;
}
