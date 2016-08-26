/*
 * Include the core server components.
 */
#include "httpd.h"
#include "http_config.h"
#include "apr_strings.h"
#include "http_protocol.h"
#include "http_log.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "mod_lua.h"

#include "apache2.h"

#ifndef ML_EXTRALIBS
#define ML_EXTRALIBS
#endif

#define MAX_HANDLERS  30

module AP_MODULE_DECLARE_DATA lua_module;


/*
** Server configuration structure.
** Stores the LuaState in a "PER_PROCESS policy".
*/
typedef struct {
	lua_State   *state;
#ifndef INCLUDE_LUA
	const char  *mainscript;
#endif
	server_rec  *srv;
} ml_server;


/*
** Directory configuration structure.
** Stores a list of valid handlers.
*/
typedef struct {
	char      *handlers[MAX_HANDLERS];
	int        num_handlers;
} ml_directory;


/*
** Create server configuration structure with default values.
*/
static void *create_ml_server (apr_pool_t *p, server_rec *s) {
	ml_server *new = (ml_server *) apr_pcalloc (p, sizeof(ml_server));
	(void)s; /* avoid "unused parameter" warning */
	new->state = NULL;
	new->srv = s;
	return (void *) new;
}


/*
** Merge two server configuration structures.
** The structures to be merged must NOT have an initalized lua_State.
*/
static void *merge_ml_server (apr_pool_t *p, void *s1, void *s2) {
	ml_server *srv1 = (ml_server *) s1;
	ml_server *srv2 = (ml_server *) s2;
	(void)p; /* avoid "unused parameter" warning */
	if (srv1->state && srv2->state) {
		ap_log_error (APLOG_MARK, APLOG_ERR, APR_EGENERAL, srv1->srv, "Trying to merge two Lua states! Keeping the first one.");
		ap_log_error (APLOG_MARK, APLOG_ERR, APR_EGENERAL, srv2->srv, "Trying to merge two Lua states! Keeping the first one.");
	}
	else if (srv2->state)
		return (void *)srv2;
	return (void *)srv1;
}


/*
** Create per-directory configuration structure with default values.
** Initiate handlers structure.
*/
static void *create_ml_directory (apr_pool_t *p, char *dirspec) {
	ml_directory *new = (ml_directory *)apr_pcalloc (p, sizeof(ml_directory));
	(void)dirspec; /* avoid "unused parameter" warning */
	new->num_handlers = 0;
	return (void *) new;
}


/*
** Merge two directory configuration structures.
** Copy all handlers from both structures (no check to avoid duplications).
*/
static void *merge_ml_directory (apr_pool_t *p, void *parent_conf, void *newloc_conf) {
	ml_directory *new = (ml_directory *) apr_pcalloc (p, sizeof(ml_directory));
	ml_directory *d1 = (ml_directory *) parent_conf;
	ml_directory *d2 = (ml_directory *) newloc_conf;
	int i;
	new->num_handlers = 0;
	for (i = 0; i < d1->num_handlers; i++)
		new->handlers[new->num_handlers++] = d1->handlers[i];
	for (i = 0; i < d2->num_handlers; i++)
		new->handlers[new->num_handlers++] = d2->handlers[i];
	return (void *)new;
}


/*
** Add a new mod_lua's handler.
*/
static const char *set_config_handler (cmd_parms *parms, void *mconfig, const char *arg) {
	ml_directory *cfg = (ml_directory *) mconfig;
	(void)parms; /* avoid "unused parameter" warning */
	if (cfg->num_handlers == MAX_HANDLERS)
		return MODULE_ERROR_NAME"too many handlers";
	cfg->handlers[cfg->num_handlers++] = (char *)arg;
	return NULL;
}


/*
** Check if the given handler is "valid".
*/
static int is_ml_handler (const request_rec *r, const ml_directory *cfg) {
	int i;
	for (i = 0; i < cfg->num_handlers; i++) {
		if (strcmp (r->handler, cfg->handlers[i]) == 0)
			return 1;
	}
	return 0;
}


#ifndef INCLUDE_LUA
/*
** Define where is the main script.
*/
static const char *set_mainscript (cmd_parms *parms, void *mconfig, const char *arg) {
	ml_server *cfg = ap_get_module_config (parms->server->module_config, &lua_module);
	(void)mconfig; /* avoid "unused parameter" warning */
	cfg->mainscript = (char *)arg;

	return NULL; /* It could return an error message here */
}
#endif


/*
 * Configuration directives supported by mod_lua
 */
static const command_rec ml_cmds[] = {
#ifndef INCLUDE_LUA
	AP_INIT_TAKE1(
		"LuaMain",
		set_mainscript,
		NULL,
		OR_ALL,
		"mod_lua's main script"
	),
#endif
	AP_INIT_TAKE1(
		"LuaHandler",
		set_config_handler,
		NULL,
		OR_ALL,
		"mod_lua's handlers"
	),
	{NULL}
};


/*
** Pushes the request record as argument to a Lua function.
*/
static void push_request (lua_State *L, request_rec *r) {
#if LUA_VERSION_NUM == 500
	/* Creates arg table with request record as its first element */
	lua_pushliteral (L, "arg");
	lua_newtable (L);
	lua_pushlightuserdata (L, (void *)r);
	lua_rawseti (L, -2, 1); /* arg[1] = r */
	lua_rawset (L, LUA_GLOBALSINDEX); /* _G.arg = `arg' */
#else
	lua_pushlightuserdata (L, (void *)r); /* pushes the request record */
#endif
}

#ifndef INCLUDE_LUA
/*
** Load the mainscript and run it.
*/
static int load_and_run (lua_State *L, request_rec *r, const char *script) {
	int errfunc, status;
	lua_getglobal (L, "_TRACEBACK");
	errfunc = lua_gettop (L);
	if ((status = luaL_loadfile (L, script)) != 0) {
		lua_pop (L, 1); /* removes errfunc */
		return status;
	}
	/* compiled ok */
	push_request (L, r);
	status = lua_pcall (L, 1, 1, errfunc);
	lua_remove (L, errfunc);
	return status;
}
#endif

/*
** Request handler.
*/
static int ml_handler (request_rec *r) {
	int status;
	lua_State *L;
	int i;
	char *kepler_init;
	ml_directory *ml_dir = (ml_directory *) ap_get_module_config (r->per_dir_config, &lua_module);
	ml_server *ml_srv = (ml_server *) ap_get_module_config (r->server->module_config, &lua_module);

	if (!is_ml_handler (r, ml_dir))
		return DECLINED;

	kepler_init = getenv("KEPLER_INIT");
	if(kepler_init==NULL) kepler_init=KEPLER_INIT;

#if LUA_STATE_PER_REQUEST
	/* Create new lua_State */
	L = lua_open ();
	lua_pushliteral (L, "LUA_STATE_PER_REQUEST");
	lua_pushboolean (L, 1);
	lua_rawset (L, LUA_GLOBALSINDEX);
#endif
#if LUA_STATE_PER_PROCESS
	/* Get the module's state */
	L = ml_srv->state;
#endif
	if (L == NULL) {
		ap_log_error (APLOG_MARK, APLOG_ERR, 0, ml_srv->srv, "fatal error: NULL Lua state.");
		return DECLINED;
	}
#if LUA_STATE_PER_REQUEST
	luaL_openlibs (L);
	status = luaL_loadfile(L, kepler_init) || lua_pcall(L, 0, 0, 0);
	if(status!=OK) {
		ap_log_error (APLOG_MARK, APLOG_ERR, 0, ml_srv->srv, "fatal error: could not load kepler_init.lua");
		return DECLINED;
	}
	luaopen_apache2 (L);
#endif
	/* Run mod_lua's global configuration script (mod2.lua) */
#ifdef INCLUDE_LUA
	push_request (L, r);
	#include "mod2.lch"
#else
	status = load_and_run (L, r, ml_srv->mainscript); /* 0 == OK */
#endif
	if (status != 0) /* error while compiling or running Lua script */
		status = HTTP_INTERNAL_SERVER_ERROR;
	else { /* status == 0 */
		if (lua_gettop (L) == 0)
			status = OK;
		else /* Get result from script */
			if (lua_isnumber (L, -1)) {
				status = lua_tonumber (L, -1);
			} else {
				status = HTTP_INTERNAL_SERVER_ERROR;
			}
	}
	/* Discard input */
	if (status == OK)
		status = ap_discard_request_body (r);
#if LUA_STATE_PER_REQUEST
	lua_close (L);
#endif
	return status;
}

#if LUA_STATE_PER_PROCESS
static apr_status_t ml_close (void *L) {
	lua_close ((lua_State *)L);
	return APR_SUCCESS;
}

static void ml_child_init (apr_pool_t *p, server_rec *s) {
        char *kepler_init;
	ml_server *cfg = (ml_server *) ap_get_module_config (s->module_config, &lua_module);
	lua_State *L = lua_open ();
	kepler_init = getenv("KEPLER_INIT");
	if(kepler_init==NULL) kepler_init=KEPLER_INIT;
	apr_pool_cleanup_register (p, (void *)L, ml_close, ml_close);
	cfg->state = L;
	cfg->srv = s;
	/* Open all distributed libraries & apache2 library */
        luaL_openlibs(L);
	status = luaL_loadfile(L, kepler_init) || lua_pcall(L, 0, 0, 0);
	if(status!=OK) {
		ap_log_error (APLOG_MARK, APLOG_ERR, 0, ml_srv->srv, "fatal error: could not load kepler_init.lua");
		lua_close(L);
		cfg->state = NULL;
	} else luaopen_apache2 (L);
}
#endif

static void ml_register_hooks (apr_pool_t *p) {
	(void)p; /* avoid "unused parameter" warning */
	ap_hook_handler (ml_handler, NULL, NULL, APR_HOOK_LAST);
#if LUA_STATE_PER_PROCESS
	ap_hook_child_init (ml_child_init, NULL, NULL, APR_HOOK_LAST);
#endif
}

/*
 * see: include/http_config.h: struct module_struct
 */
module AP_MODULE_DECLARE_DATA lua_module = {
	STANDARD20_MODULE_STUFF,
	create_ml_directory,	/* create per-directory config structures */
	merge_ml_directory,	/* merge per-directory config structures */
	create_ml_server,	/* create per-server config structures */
	merge_ml_server,	/* merge per-server config structures */
	ml_cmds,	/* command handlers */
	ml_register_hooks,			/* callback for registering hooks */
};
