#ifndef __HTTP_H__
#define __HTTP_H__

#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "str.h"

void download(const char* url, const char* file);

struct string* getHtml(const char* url);

void luaopen_http(lua_State* L);

#endif
