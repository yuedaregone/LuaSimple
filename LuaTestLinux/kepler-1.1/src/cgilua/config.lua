-- CGILua configuration file for Kepler
-- $Id: config.lua,v 1.1 2007/05/30 22:47:15 carregal Exp $

-- Defining diferent behaviours for CGI URL handling using the dispatcher
--require("cgilua.dispatcher")
--cgilua.dispatcher.map("/wiki", CGILUA_APPS.."wiki.lua")
--cgilua.dispatcher.map("/blog", CGILUA_APPS.."blog.lua")


-- Emulating old behavior loading file "env.lua" from the script's directory
--[[
cgilua.addopenfunction (function ()
	cgilua.doif ("env.lua")
end)
--]]

-- Basic configuration for using sessions
require"cgilua.session"
cgilua.session.setsessiondir ("/tmp/")
-- The following function must be called by every script that needs session.
local already_enabled = false
function cgilua.enablesession ()
	if already_enabled then
		return
	else
		already_enabled = true
	end
	cgilua.session.open ()
	cgilua.addclosefunction (cgilua.session.close)
end

-- Compatibility
cgilua.preprocess = cgilua.handlelp
cgilua.includehtml = cgilua.lp.include

-- Directories for specific applications' libraries.
-- The following table should be indexed by the virtual path of the application
-- and contain the absolute path of the application's Lua-library directory.
local app_lib_dir = {
	["/t/"] = "/usr/local/src/cgilua/tests",
}
local package = package
cgilua.addopenfunction (function ()
	local app = app_lib_dir[cgilua.script_vdir]
	if app then
		package.path = app.."/?.lua;"..package.path
	end
end)
