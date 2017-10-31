#ifndef __HTTP_H__
#define __HTTP_H__

#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "str.h"
void initCurl();

void setCurlHeader();

void resetCurlHeader();

void download(const char* url, const char* file);

struct string* getHtml(const char* url);

void destroyCurl();

void luaopen_http(lua_State* L);

#endif
