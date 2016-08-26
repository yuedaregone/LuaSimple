/*
** ISAPI connector library.
** $Id: isapi_lib.c,v 1.3 2007/07/17 22:16:08 tomas Exp $
*/

#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include <windows.h>
#include <httpext.h>
#include "isapi_lib.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define CHECK(val) if(!(val)) return FALSE;
//#define CHECK(val) val
#define CHECK_HSE(val) if(!(val)) return HSE_STATUS_ERROR;
#define CHECK_LUA(val, L, msg) if(!(val)) { lua_pushstring(L, msg); lua_error(L); }
#define GET_ECB *((EXTENSION_CONTROL_BLOCK**)lua_touserdata(L, lua_upvalueindex(1)))
//#define GET_ECB ( (EXTENSION_CONTROL_BLOCK *)(lua_touserdata(L, lua_upvalueindex(1))) );
#define FREE(ptr) if(ptr) free(ptr);

#define DEFINE_GETTER(func, pusher, field) \
	static int func(lua_State *L) { \
		EXTENSION_CONTROL_BLOCK *pECB; \
		pECB = *((EXTENSION_CONTROL_BLOCK**)lua_touserdata(L, lua_upvalueindex(1))); \
		pusher(L, pECB->field); \
		return 1; \
	}

#define ISAPI_LIBNAME "isapi"
//#define ISAPI_REGISTRY_KEY "SYSTEM\\CurrentControlSet\\Services\\W3SVC"
#define ISAPI_REGISTRY_KEY "SOFTWARE\\CGILua\\5.0"
#define ISAPI_MAINSCRIPTPATH_KEY "MainScriptPath"

static DWORD gdwTlsIndex;

static BOOL InitLuaState(lua_State *L)
{
	luaopen_base(L);
	luaopen_table(L);
	luaopen_io(L);
	luaopen_string(L);
	luaopen_math(L);
	luaopen_debug(L);
	luaopen_loadlib(L);
	return TRUE;
}

static int IsapiRedirect(lua_State *L)
{
	EXTENSION_CONTROL_BLOCK *pECB;
	const char *url;
	DWORD size;

	pECB = GET_ECB;
	CHECK_LUA(url = lua_tostring(L,1), L, "redirect url must be a string");
	size = strlen(url);

	CHECK_LUA(pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_URL_REDIRECT_RESP,
		(LPVOID)url, &size, NULL), L, "error sending redirect request");
	pECB->dwHttpStatusCode = 302;

	return 0;
}

static int SendHeaders(lua_State *L) 
{
	EXTENSION_CONTROL_BLOCK *pECB;
	HSE_SEND_HEADER_EX_INFO hshi;

	hshi.pszStatus = lua_tostring(L, 1);
	hshi.cchStatus = (DWORD)lua_tonumber(L, 2);
	hshi.pszHeader = lua_tostring(L, 3);
	hshi.cchHeader = (DWORD)lua_tonumber(L, 4);
	hshi.fKeepConn = FALSE;

	pECB = GET_ECB;
	CHECK_LUA(pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX,
			&hshi, NULL, NULL), L, "error sending headers");

	return 0;
}

static int Write(lua_State *L)
{
	int nargs = lua_gettop(L);
	int i;
	EXTENSION_CONTROL_BLOCK *pECB;
	size_t cBuf;
	LPVOID pBuf;

	pECB = GET_ECB;
	for (i = 1; i <= nargs; i++) {
		cBuf = lua_strlen(L, i);
		CHECK_LUA(pBuf = (LPVOID)lua_tostring(L, i), L, "argument to write must be string");
		CHECK_LUA(pECB->WriteClient(pECB->ConnID, pBuf, &cBuf , HSE_IO_SYNC), L,
			"error transmitting to client");
	}

	return 0;
}

static void write_log(EXTENSION_CONTROL_BLOCK *pECB, char *msg) {
	size_t cBuf;
	LPVOID pBuf;

	cBuf = strlen(msg);
	pBuf = (LPVOID)msg;

	pECB->ServerSupportFunction(pECB->ConnID, HSE_APPEND_LOG_PARAMETER,
		pBuf, &cBuf, NULL);
}

static int LogError(lua_State *L)
{
	EXTENSION_CONTROL_BLOCK *pECB;
	size_t cBuf;
	LPVOID pBuf;

	cBuf = lua_strlen(L, 1);
	CHECK_LUA(pBuf = (LPVOID)lua_tostring(L, 1), L, "argument to log write must be string");

	pECB = GET_ECB;
	CHECK_LUA(pECB->ServerSupportFunction(pECB->ConnID, HSE_APPEND_LOG_PARAMETER,
		pBuf, &cBuf, NULL), L, "error writing server log");

	return 0;
}

static int GetPostData(lua_State *L)
{
	EXTENSION_CONTROL_BLOCK *pECB;
	int cBuf;
	void *pBuf;

	cBuf = (int)lua_tonumber(L, 1);
	if(cBuf <= 0) return 0;
	CHECK_LUA(pBuf = malloc(cBuf), L, "couldn't allocate buffer for postdata");

	pECB = GET_ECB;
	if(pECB->ReadClient(pECB->ConnID, pBuf, &cBuf)) 
	{
		if(cBuf > 0)
			lua_pushlstring(L, (const char*)pBuf, cBuf);
		else
			lua_pushnil(L);
		FREE(pBuf);
		return 1;
	}
	else
	{
		FREE(pBuf);
		lua_pushstring(L, "error getting postdata");
		lua_error(L);
	}

	return 0;
}

DEFINE_GETTER(GetTotalBytes, lua_pushnumber, cbTotalBytes);
DEFINE_GETTER(GetContentType, lua_pushstring, lpszContentType);
DEFINE_GETTER(GetCachedData, lua_pushstring, lpbData);
DEFINE_GETTER(GetDataAvailable, lua_pushnumber, cbAvailable);
DEFINE_GETTER(GetPathInfo, lua_pushstring, lpszPathInfo);
DEFINE_GETTER(GetPathTranslated, lua_pushstring, lpszPathTranslated);
DEFINE_GETTER(GetQueryString, lua_pushstring, lpszQueryString);
DEFINE_GETTER(GetRequestMethod, lua_pushstring, lpszMethod);

static int GetServerVariable(lua_State *L)
{
	EXTENSION_CONTROL_BLOCK *pECB;
	int cBuf;
	void *pBuf;
	char* szName;
	
	CHECK_LUA(szName = (char*)lua_tostring(L, 1), L, "variable name must be a string");

	pECB = GET_ECB;
	pECB->GetServerVariable(pECB->ConnID, szName, NULL, &cBuf);
	switch(GetLastError())
	{
	case ERROR_INVALID_INDEX:
	case ERROR_NO_DATA:
		return 0;
	case ERROR_INSUFFICIENT_BUFFER:
		CHECK_LUA(pBuf = malloc(cBuf), L, "couldn't allocate buffer for server variable");
		if(pECB->GetServerVariable(pECB->ConnID, szName, pBuf, &cBuf))
		{
			lua_pushlstring(L, (const char*)pBuf, cBuf-1);
			FREE(pBuf);
			return 1;
		}
		FREE(pBuf);
	default:
		lua_pushstring(L, "error getting server variable");
		lua_error(L);
		break;
	}
	return 0;
}

static const luaL_reg isapi_lib[] = {
	{ "redirect", IsapiRedirect },
	{ "send_headers", SendHeaders },
	{ "write", Write },
	{ "log_error", LogError },
	{ "get_postdata", GetPostData },
	{ "get_total_bytes", GetTotalBytes },
	{ "get_content_type", GetContentType },
	{ "get_path_info", GetPathInfo },
	{ "get_path_translated", GetPathTranslated },
	{ "get_query_string", GetQueryString },
	{ "get_request_method", GetRequestMethod },
	{ "get_server_variable", GetServerVariable },
	{ "get_cached_data", GetCachedData },
	{ "get_data_available", GetDataAvailable },
	{ NULL, NULL }
};

static void WriteMessage(EXTENSION_CONTROL_BLOCK *pECB, const char* message) {
    char *pBuf;
    int cBuf;
    
    cBuf = strlen(message);
    pBuf = (char*)message;
    pECB->WriteClient(pECB->ConnID,pBuf,&cBuf,HSE_IO_SYNC);
}

/*
 * Get main script path from windows registry
 */
static int GetMainScriptPath (char* path, DWORD size)
{
	int ret = 0;
	DWORD type;
	HKEY key;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, ISAPI_REGISTRY_KEY, 0, KEY_READ, &key) == ERROR_SUCCESS)
	{
		ret = (RegQueryValueEx(key, ISAPI_MAINSCRIPTPATH_KEY, NULL, &type, path, &size) == ERROR_SUCCESS) ? 1 : 0;
		RegCloseKey(key);
	}
	return ret;
}

/*
 * Request handler
 */
DWORD WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB)
{
	int status = 1;
	const char* err;
	char path[MAX_PATH];
	lua_State *L;
	EXTENSION_CONTROL_BLOCK **ud;
	CHECK_HSE(L = lua_open());
	InitLuaState(L);
	ud = (EXTENSION_CONTROL_BLOCK**)lua_newuserdata(L,sizeof(EXTENSION_CONTROL_BLOCK*));
	*ud = pECB;
	luaL_openlib(L, ISAPI_LIBNAME, isapi_lib, 1);
	lua_getglobal(L, ISAPI_LIBNAME);
    lua_pushstring(L, "log_error");
    lua_gettable(L, -2);
	lua_setglobal(L, "_ALERT");
    lua_pop(L, 1);

#ifndef INCLUDE_ISAPI
	if (GetMainScriptPath(path, MAX_PATH))
        status = lua_dofile(L, path);
#else
	#include "isapi.lch"
#endif
//	WriteMessage(pECB,"<html><body>");
//	WriteMessage(pECB, lua_tostring(L,-1));
//	WriteMessage(pECB, "</body></html>");
//	write_log(pECB, " cheguei aqui " );

	if(status) {
		err = lua_tostring(L,-1);
		lua_close(L);
		return HSE_STATUS_ERROR;
	} else {
		lua_close(L);
		return HSE_STATUS_SUCCESS;
	}
}

BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
	pVer->dwExtensionVersion = MAKELONG(1,0);
	strncpy(pVer->lpszExtensionDesc, "CGILua ISAPI Extension", HSE_MAX_EXT_DLL_NAME_LEN);
	return TRUE;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(lpReserved);
}

