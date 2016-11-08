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

    struct curl_slist *headers=NULL; /* init to NULL is important */
    headers = curl_slist_append(headers, "Accept: image/png, image/svg+xml, image/*;q=0.8, */*;q=0.5");
    headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate");
    headers = curl_slist_append(headers, "Accept-Language: zh-CN");
    headers = curl_slist_append(headers, "Connection: Keep-Alive");
    headers = curl_slist_append(headers, "DNT: 1");
    headers = curl_slist_append(headers, "Host: girlatlas.b0.upaiyun.com");
    headers = curl_slist_append(headers, "Referer: http://girl-atlas.com/album/576545e658e039318beb3a28");
    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; WOW64; Trident/6.0)");

    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 1);
    curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(pCurl, CURLOPT_URL, url);
	curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, handle);
	curl_easy_perform(pCurl);
    curl_slist_free_all(headers);
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
    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 1);
    curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 30);
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

static int downloadFile(lua_State* L)
{
    const char *url = luaL_checkstring(L, 1);
    const char *file = luaL_checkstring(L, 2);
    download(url, file);
    return 0;
}

static const luaL_Reg Http[] = {
	{"fetchHtml", fetchHtml},
    {"download", downloadFile},
	{NULL, NULL}
};

void luaopen_http(lua_State* L)
{
	luaL_newlib(L, Http);
    lua_setglobal(L, "Http");
}
