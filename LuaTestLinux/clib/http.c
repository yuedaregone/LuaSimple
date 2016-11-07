#include "http.h"
#include <curl/curl.h>

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
	CURL* pCurl = curl_easy_init();
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, handle);
	curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);
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
	CURL* pCurl = curl_easy_init();
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, html_callback);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, str);
	curl_easy_perform(pCurl);
	curl_easy_cleanup(pCurl);
    return str;
}

static int fetchHtml(lua_State* L)
{
	const char *url = luaL_checkstring(L, 1);
    struct string* content = getHtml(url);
	lua_pushstring(L, content->buff);
    free_buff_str(content);
	return 1;
}

static const luaL_Reg Http[] = {
	{"fetchHtml", fetchHtml},
	{NULL, NULL}
};

void luaopen_http(lua_State* L)
{
	luaL_newlib(L, Http);
    lua_setglobal(L, "Http");
}
