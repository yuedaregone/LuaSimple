#include "http.h"
#include <curl/curl.h>
static CURL* pCurl = NULL;
static struct curl_slist* headers = NULL;

void initCurl()
{
    pCurl = curl_easy_init();
    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 1);
    curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 30);
}

void setCurlHeader()
{
    //headers = curl_slist_append(headers, );
    //curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);
}

void resetCurlHeader()
{
    if (headers)
    {
        curl_slist_free_all(headers);
        headers = NULL;
    }
    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);
}

static int write_callback(void* buff, size_t size, size_t cn, void* param)
{
	FILE* fp = (FILE*)(param);
	return fwrite(buff, size, cn, fp);
}

void download(const char* url, const char* file)
{
	FILE* handle = fopen(file, "wb");
	if (!handle)
	{
		return;
	}
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, handle);
	curl_easy_perform(pCurl);
	fclose(handle);
}

static int html_callback(void* buff, size_t size, size_t cn, void* param)
{
	struct string* html = (struct string*)(param);
    append_buff(html, buff, cn);
	return cn;
}

struct string* getHtml(const char* url)
{
    struct string* str = new_buff_str();
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, html_callback);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, str);
	curl_easy_perform(pCurl);
    return str;
}

void destroyCurl()
{
    curl_slist_free_all(headers);
    headers = NULL;
    curl_easy_cleanup(pCurl);
    pCurl = NULL;
}

static int initHttp(lua_State* L)
{
    initCurl();
    return 0;
}

static int setHeader(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        if (lua_type(L, -1) == LUA_TSTRING)
        {
            headers = curl_slist_append(headers, lua_tostring(L, -1));
        }
        lua_pop(L, 1);
    }
    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);
    return 0;
}

static int resetHeader(lua_State* L)
{
    resetCurlHeader();
    return 0;
}

static int fetchHtml(lua_State* L)
{
	const char *url = luaL_checkstring(L, 1);
    struct string* content = getHtml(url);
	lua_pushstring(L, content->buff);
    free_buff_str(content);
	return 1;
}

static int downloadFile(lua_State* L)
{
    const char *url = luaL_checkstring(L, 1);
    const char *file = luaL_checkstring(L, 2);
    download(url, file);
    return 0;
}

static int destroyHttp(lua_State* L)
{
    destroyCurl();
    return 0;
}

static const luaL_Reg Http[] = {
    {"initHttp", initHttp},
    {"setHeader", setHeader},
    {"resetHeader", resetHeader},
	{"fetchHtml", fetchHtml},
    {"download", downloadFile},
    {"destroyHttp", destroyHttp},
	{NULL, NULL}
};

void luaopen_http(lua_State* L)
{
	luaL_newlib(L, Http);
    lua_setglobal(L, "Http");
}
