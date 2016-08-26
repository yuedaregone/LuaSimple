/*
** Apache 2 library.
** $Id: apache2.c,v 1.5 2007/04/15 19:39:11 tomas Exp $
*/
#include <sys/types.h>

#include "httpd.h"
#include "util_filter.h"
#include "http_core.h"
#include "http_protocol.h"
#include "apr_strings.h"
#include "http_log.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "apache2.h"

typedef struct {
    const char    *t_name;
    int      t_val;
} TRANS;

static const TRANS priorities[] = {
    {"emerg",   APLOG_EMERG},
    {"alert",   APLOG_ALERT},
    {"crit",    APLOG_CRIT},
    {"error",   APLOG_ERR},
    {"warn",    APLOG_WARNING},
    {"notice",  APLOG_NOTICE},
    {"info",    APLOG_INFO},
    {"debug",   APLOG_DEBUG},
    {NULL,      -1},
};
static int default_priority = 3;

#define AP2_LIBNAME     "apache2"
#define POST_BUFFER_SIZE   (64 * 1024)


/*
** Convert the userdata on top of the stack to a request record.
** Throws an error otherwise.
*/
static request_rec *check_request (lua_State *L) {
	request_rec *p = (request_rec *)lua_touserdata (L, 1);
	if (p == NULL)
		luaL_typerror (L, 1, "userdata");
	return p;
}


/*
** Send http header.
** Uses the request_rec defined as an upvalue.
** Receives a Lua string.
*/
static int ap2_set_content_type (lua_State *L) {
	request_rec *r = check_request(L);
	ap_set_content_type (r, apr_pstrdup(r->pool,(char *)lua_tostring (L, 2)));
	return 0;
}

/*
** Consulting header_only field of request_rec.
** Uses the request_rec defined as an upvalue.
** Returns a Lua boolean.
*/
static int ap2_header_only (lua_State *L) {
	lua_pushboolean (L, check_request(L)->header_only);
	return 1;
}

/*
** Consulting protocol field of request_rec.
** Uses the request_rec defined as an upvalue.
** Returns a Lua string.
*/
static int ap2_protocol (lua_State *L) {
	lua_pushstring (L, check_request(L)->protocol);
	return 1;
}

/*
** Consulting hostname field of request_rec.
** Uses the request_rec defined as an upvalue.
** Returns a Lua string.
*/
static int ap2_hostname (lua_State *L) {
	lua_pushstring (L, check_request(L)->hostname);
	return 1;
}

/*
** Consulting method field of request_rec.
** Uses the request_rec defined as an upvalue.
** Returns a Lua string.
*/
static int ap2_method (lua_State *L) {
	lua_pushstring (L, check_request(L)->method);
	return 1;
}

/*
** Consulting unparsed_uri field of request_rec.
** Uses the request_rec defined as an upvalue.
** Returns a Lua string.
*/
static int ap2_unparsed_uri (lua_State *L) {
	lua_pushstring (L, check_request(L)->unparsed_uri);
	return 1;
}

/*
** Consulting uri field of request_rec.
** Uses the request_rec defined as an upvalue.
** Returns a Lua string.
*/
static int ap2_uri (lua_State *L) {
	lua_pushstring (L, check_request(L)->uri);
	return 1;
}

/*
** Consulting path_info field of request_rec.
** Uses the request_rec defined as an upvalue.
** Returns a Lua string.
*/
static int ap2_path_info (lua_State *L) {
	lua_pushstring (L, check_request(L)->path_info);
	return 1;
}

/*
** Consulting args field of request_rec.
** Uses the request_rec defined as an upvalue.
** Returns a Lua string.
*/
static int ap2_args (lua_State *L) {
	lua_pushstring (L, check_request(L)->args);
	return 1;
}

/*
** Consulting handler field of request_rec.
** Uses the request_rec defined as an upvalue.
** Returns a Lua string.
*/
static int ap2_handler (lua_State *L) {
	lua_pushstring (L, check_request(L)->handler);
	return 1;
}

/*
 * Consulting content_type field of request_rec.
 * Uses the request_rec defined as an upvalue.
 * Returns a Lua string with the value of the Content-type header.
 */
static int ap2_get_content_type (lua_State *L) {
	lua_pushstring (L, check_request(L)->content_type);
	return 1;
}

/*
 * Consulting filename field of request_rec.
 * Uses the request_rec defined as an upvalue.
 * Returns a Lua string with the value of the filename field.
 */
static int ap2_filename (lua_State *L) {
	lua_pushstring (L, check_request(L)->filename);
	return 1;
}

/*
** Send a string to the client.
** Uses the request_rec defined as an upvalue.
** Receives a Lua string.
*/
static int ap2_rwrite (lua_State *L) {
	request_rec *r = check_request (L);
	int nargs = lua_gettop (L);
	int idx = 2;
	for (; idx <= nargs; idx++) {
		size_t l;
		const char *s = luaL_checklstring(L, idx, &l);
		if (ap_rwrite (s, l, r) == -1)
			luaL_error (L, "error while writing output");
	}
	return 0;
}


/*
** Binding to ap_setup_client_block.
** Uses the request_rec defined as an upvalue.
** Receives a Lua string: "REQUEST_NO_BODY", "REQUEST_CHUNKED_ERROR" or
**	"REQUEST_CHUNKED_DECHUNK".
** It returns the status code.
*/
static int ap2_setup_client_block (lua_State *L) {
	request_rec *r = check_request(L);
	const char *s = luaL_checklstring (L, 2, 0);
	if (strcmp (s, "REQUEST_NO_BODY") == 0)
		lua_pushnumber (L, ap_setup_client_block (r, REQUEST_NO_BODY));
	else if (strcmp (s, "REQUEST_CHUNKED_ERROR") == 0)
		lua_pushnumber (L, ap_setup_client_block (r, REQUEST_CHUNKED_ERROR));
	else if (strcmp (s, "REQUEST_CHUNKED_DECHUNK") == 0)
		lua_pushnumber (L, ap_setup_client_block (r, REQUEST_CHUNKED_DECHUNK));
	else
		lua_pushnil (L);
	return 1;
}

/*
 * Binding to ap_should_client_block.
 * Uses the request_rec defined as an upvalue.
 * Returns the status code.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Esta funcao nao deve ser chamada mais de uma vez.
O que fazer para evitar isto?
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */
static int ap2_should_client_block (lua_State *L) {
	lua_pushnumber (L, ap_should_client_block (check_request(L)));
	return 1;
}

/*
** Binding to ap_get_client_block.
** Uses the request_rec defined as an upvalue.
** Receives a number of bytes to read.
** Returns a string or nil if EOS.
*/
static int ap2_get_client_block (lua_State *L) {
	request_rec *r = check_request(L);
	char buffer[POST_BUFFER_SIZE+1];
	int bytesleft = luaL_optint (L, 2, POST_BUFFER_SIZE);
	int status, n;
	int count = 0;
	if (bytesleft < 0) {
		luaL_error (L, "block size must be positive");
		return 0;
	}
	while (bytesleft) {
		n = (bytesleft > POST_BUFFER_SIZE) ? POST_BUFFER_SIZE : bytesleft;
		status = ap_get_client_block (r, buffer, n);
		if (status == 0) { /* end-of-body */
			if (r->remaining > 0)
				continue; /* still has something to read */
			else
				break; /* end-of-stream */
		} else if (status == -1) { /* error or premature chunk end */
			lua_pushnil (L);
			lua_pushstring (L, "error getting client block");
			return 2;
		} else {
			bytesleft -= status;
			lua_pushlstring (L, buffer, status);
			count++;
		}
	}
	/* is this necessary? */
	if (count)
		lua_concat (L, count);
	else
		lua_pushnil (L);
	return 1;
}

/*
** Consulting remaining field of request_rec.
** Uses the request_rec defined as an upvalue.
** Return a Lua number.
*/
static int ap2_remaining (lua_State *L) {
	lua_pushnumber (L, (lua_Number)check_request(L)->remaining);
	return 1;
}

/*
** Binding to ap_discard_request_body.
** Uses the request_rec defined as an upvalue.
** Returns a status code.
*/
static int ap2_discard_request_body (lua_State *L) {
	lua_pushnumber (L, ap_discard_request_body (check_request(L)));
	return 1;
}

/*
** Consulting headers.
** Uses the request_rec defined as an upvalue.
** Receives a string with the header to search or nil.
** Returns the header value or nil if not found.
** Returns all headers on the stack.
*/
static int ap2_get_header (lua_State *L) {
	request_rec *r = check_request(L);
	const char *header = luaL_optstring (L, 2, NULL);
	const apr_array_header_t *hdrs_arr = apr_table_elts (r->headers_in);
	const apr_table_entry_t *hdrs = (const apr_table_entry_t *) hdrs_arr->elts;
	if (header == NULL) { /* return all headers */
		int i, count;
		luaL_checkstack (L, hdrs_arr->nelts, "too many headers");
		for (i = 0, count = 0; i < hdrs_arr->nelts; i++) {
			if (hdrs[i].key) {
				count++;
				lua_pushstring (L, hdrs[i].key);
				lua_pushliteral (L, " = ");
				lua_pushstring (L, hdrs[i].val);
				lua_concat (L, 3);
			}
		}
		return count;
	} else { /* search for a given header */
		int i;
		for (i = 0; i < hdrs_arr->nelts; i++) {
			if (!hdrs[i].key)
				continue;
			if (!strcasecmp (hdrs[i].key, header)) {
				lua_pushstring (L, hdrs[i].val);
				return 1;
			}
		}
	}
	lua_pushnil (L);
	return 1;
}

/*
** Set an output header.
** Uses the request_rec defined as an upvalue.
** Receives a string with the header's name AND
** a second string with the header's value.
** Note: apr_table_setn DOES NOT copies the key and value strings.
*/
static int ap2_set_headers_out (lua_State *L) {
	const char *header = luaL_checkstring (L, 2);
	const char *value = luaL_checkstring (L, 3);
	apr_table_set (check_request(L)->headers_out, header, value);
	return 0;
}


/*
** Set an output "error" header.
** Uses the request_rec defined as an upvalue.
** Receives a string with the header's name AND
** a second string with the header's value.
** Note: apr_table_setn DOES NOT copies the key and value strings.
** Note 2: this header is "printed even on errors and persist across
** internal redirects" -- from Apache's include/httpd.h
*/
static int ap2_set_err_headers_out (lua_State *L) {
	const char *header = luaL_checkstring (L, 2);
	const char *value = luaL_checkstring (L, 3);
	apr_table_set (check_request(L)->err_headers_out, header, value);
	return 0;
}
/*
** Add an output header.
** Uses the request_rec defined as an upvalue.
** Receives a string with the header's name AND
** a second string with the header's value.
** Note: apr_table_addn DOES NOT copies the key and value strings.
*/
static int ap2_add_headers_out (lua_State *L) {
	const char *header = luaL_checkstring (L, 2);
	const char *value = luaL_checkstring (L, 3);
	apr_table_add (check_request(L)->headers_out, header, value);
	return 0;
}
/*
** Add an output "error" header.
** Uses the request_rec defined as an upvalue.
** Receives a string with the header's name AND
** a second string with the header's value.
** Note: apr_table_addn DOES NOT copies the key and value strings.
** Note 2: this header is "printed even on errors and persist across
** internal redirects" -- from Apache's include/httpd.h
*/
static int ap2_add_err_headers_out (lua_State *L) {
	const char *header = luaL_checkstring (L, 2);
	const char *value = luaL_checkstring (L, 3);
	apr_table_add (check_request(L)->err_headers_out, header, value);
	return 0;
}
/*
                apr_table_add(rmain->err_headers_out, "Set-Cookie", cookie);
*/

/*
** Get user name.
** Uses the request_rec defined as an upvalue.
** Returns a string with the user name.
*/
static int ap2_user (lua_State *L) {
	lua_pushstring (L, check_request(L)->user);
	return 1;
}

/*
** Get auth type.
** Uses the request_rec defined as an upvalue.
** Returns a string with the auth type.
*/
static int ap2_auth_type (lua_State *L) {
	lua_pushstring (L, check_request(L)->ap_auth_type);
	return 1;
}

/*
** Bind to get_remote_host.
** Uses the request_rec defined as an upvalue.
** Returns a string with the remote host name.
*/
static int ap2_get_remote_host (lua_State *L) {
	request_rec *r = check_request(L);
	lua_pushstring (L, ap_get_remote_host (r->connection, r->per_dir_config, REMOTE_HOST, NULL));
	return 1;
}

/*
** Get remote logname.
** Uses the request_rec defined as an upvalue.
** Returns a string with the remote logname.
*/
static int ap2_get_remote_logname (lua_State *L) {
	lua_pushstring (L, ap_get_remote_logname(check_request(L)));
	return 1;
}

/*
** Bind to get_server_version.
** Uses the request_rec defined as an upvalue.
** Returns a string with the server software version.
*/
static int ap2_get_server_version (lua_State *L) {
	lua_pushstring (L, ap_get_server_version());
	return 1;
}

/*
** Bind to get_server_name.
** Uses the request_rec defined as an upvalue.
** Returns a string with the server name.
*/
static int ap2_get_server_name (lua_State *L) {
	request_rec *r = check_request(L);
	lua_pushstring (L, ap_escape_html (r->pool, ap_get_server_name(r)));
	return 1;
}

/*
** Bind to get_server_port.
** Uses the request_rec defined as an upvalue.
** Returns a string with the server name.
*/
static int ap2_get_server_port (lua_State *L) {
	request_rec *r = check_request(L);
	lua_pushnumber (L, ap_get_server_port(r));
	return 1;
}

/*
** Obtain the remote IP address.
** Uses the request_rec defined as an upvalue.
** Returns a string with the remote IP address.
*/
static int ap2_conn_remote_addr (lua_State *L) {
	request_rec *r = check_request(L);
	lua_pushstring (L, r->connection->remote_ip);
	return 1;
}

/*
** Obtain the remote port.
** Uses the request_rec defined as an upvalue.
** Returns a string with the remote port.
*/
static int ap2_conn_remote_port (lua_State *L) {
	apr_port_t rport;
	request_rec *r = check_request(L);
	//apr_sockaddr_port_get (&rport, r->connection->remote_addr);
	rport = r->connection->remote_addr->port;
	lua_pushstring (L, apr_itoa (r->pool, rport));
	return 1;
}


/*
** Convert a string to a level code.
*/
static int str2level (const char *s) {
	int i;
	for (i = 0; priorities[i].t_name; i++)
		if (strcmp (s, priorities[i].t_name) == 0)
			return priorities[i].t_val;
	return priorities[default_priority].t_val;
}


/*
** Send a string to the error log.
** Receives a string with a message, an optional level string (valid strings
** are "emerg", "alert", "crit", "error", "warn", "notice", "info", "debug"),
** and an optional status number.
*/
static int ap2_log_error (lua_State *L) {
	request_rec *r = check_request(L);
	const char *message = luaL_checkstring (L, 2);
	int level = str2level (luaL_optstring (L, 3, ""));
	apr_status_t status = (apr_status_t) luaL_optnumber (L, 4, APR_SUCCESS);
	ap_log_error (APLOG_MARK, level, status, r->server, message);
	return 0;
}


/*
** Retrieves the process identification (PID) and its parent's.
*/
#if !defined(getpid)
#if defined(_getpid)
#define getpid _getpid
#endif
#endif
static int ap2_pid (lua_State *L) {
	lua_pushnumber (L, (lua_Number)getpid ());
#if defined(getppid)
	lua_pushnumber (L, (lua_Number)getppid ());
#else
	lua_pushnil (L);
#endif
	return 2;
}


/*
** Binding functions
*/
static const luaL_reg ap2_lib[] = {
	{ "set_content_type",     ap2_set_content_type },
	{ "header_only",          ap2_header_only },
	{ "protocol",             ap2_protocol },
	{ "hostname",             ap2_hostname },
	{ "method",               ap2_method },
	{ "unparsed_uri",         ap2_unparsed_uri },
	{ "uri",                  ap2_uri },
	{ "path_info",            ap2_path_info },
	{ "args",                 ap2_args },
	{ "handler",              ap2_handler },
	{ "get_content_type",     ap2_get_content_type },
	{ "filename",             ap2_filename },
	{ "rwrite",               ap2_rwrite },
	{ "setup_client_block",   ap2_setup_client_block },
	{ "should_client_block",  ap2_should_client_block },
	{ "get_client_block",     ap2_get_client_block },
	{ "remaining",            ap2_remaining },
	{ "discard_request_body", ap2_discard_request_body },
	{ "get_header",           ap2_get_header },
	{ "set_headers_out",      ap2_set_headers_out },
	{ "set_err_headers_out",  ap2_set_err_headers_out },
	{ "add_headers_out",      ap2_add_headers_out },
	{ "add_err_headers_out",  ap2_add_err_headers_out },
	{ "user",                 ap2_user },
	{ "auth_type",            ap2_auth_type },
	{ "get_remote_host",      ap2_get_remote_host },
	{ "get_remote_logname",   ap2_get_remote_logname },
	{ "get_server_version",   ap2_get_server_version },
	{ "get_server_name",      ap2_get_server_name },
	{ "get_server_port",      ap2_get_server_port },
	{ "conn_remote_addr",     ap2_conn_remote_addr },
	{ "conn_remote_port",     ap2_conn_remote_port },
	{ "log_error",            ap2_log_error },
	{ "pid",                  ap2_pid },
	{ NULL, NULL }
};

typedef struct {
	const char *name;
	int val;
} ap2_constants;

static ap2_constants tab_consts[] = {
	{ "OK",                         OK },
	{ "DECLINED",                   DECLINED },
	{ "HTTP_BAD_REQUEST",           HTTP_BAD_REQUEST },
	{ "HTTP_FORBIDDEN",             HTTP_FORBIDDEN },
	{ "HTTP_NOT_FOUND",             HTTP_NOT_FOUND },
	{ "HTTP_METHOD_NOT_ALLOWED",    HTTP_METHOD_NOT_ALLOWED },
	{ "HTTP_INTERNAL_SERVER_ERROR", HTTP_INTERNAL_SERVER_ERROR },
	{ "HTTP_MOVED_TEMPORARILY",     HTTP_MOVED_TEMPORARILY },
	{ "AP_FILTER_ERROR",            AP_FILTER_ERROR },
	{ NULL, 0 }
};

/*
** Set apachelib constants at the table on top of the stack.
*/
static void ap2_define_constants (lua_State *L, ap2_constants tab[]) {
	int i;
	for (i = 0; tab[i].name != NULL; i++) {
		lua_pushstring (L, tab[i].name);
		lua_pushnumber (L, tab[i].val);
		lua_settable (L, -3);
	}
}

/*
** Register binding functions and the constants.
** All of them use the request_rec as an upvalue.
** Leaves the table on top of the stack.
*/
int luaopen_apache2 (lua_State *L) {
	luaL_openlib (L, AP2_LIBNAME, ap2_lib, 0); /* leaves the table */
	ap2_define_constants (L, tab_consts);
	return 1;
}
