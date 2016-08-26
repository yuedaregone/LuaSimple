---------------------------------------------------------------------
-- CGI Launcher script.
-- This script is used to comunicate between the servlet and
-- CGILua
-- Author: Thiago Ponte, Danilo Tuler
---------------------------------------------------------------------

----------------------------------------------------------------------------
-- Creates the implementation of the SAPI.Request and SAPI.Response tables
----------------------------------------------------------------------------

local function set_api (request, response)
	SAPI = {
		Response = {},
		Request = {},
		Info = { -- Information data
			_COPYRIGHT = "Copyright (C) 2004-2005 Kepler Project",
			_DESCRIPTION = "Servlet SAPI implementation",
			_NAME = "Servlet SAPI",
			_VERSION = "1.0",
			ispersistent = false,
		},
	}
	SAPI.Response.write = function(text) return response:getWriter():write(tostring(text)) end
	SAPI.Response.header = function(h, v) response:addHeader(h,v) end
	SAPI.Response.errorlog = function(msg) logger:error(msg) end
	SAPI.Response.contenttype = function(contentType) response:setContentType(contentType) end
	SAPI.Response.redirect = function(url) response:sendRedirect(url) end
	SAPI.Request.getpostdata = function (n) return request:getPostData(n) end
	SAPI.Request.servervariable = function (name) return request:serverVariable(name) end
end

---------------------------------------------------------------------
-- Trying to load and execute the "mainscript".

-- compatibility code for Lua version 5.0 providing 5.1 behavior
if string.find (_VERSION, "Lua 5.0") and not _COMPAT51 then
	if not LUA_PATH then
		LUA_PATH = [[LUA_PATH]]
	end
	require"compat-5.1"
	package.cpath = [[LUA_CPATH]]
end

---------------------------------------------------------------------
-- Loading required libraries

require"venv"
require"lfs"

----------------------------------------------------------------------------
-- Main function to launch cgilua

function launch(request, response)
	set_api(request, response)
	local f = venv (function () require"cgilua" cgilua.main() end)
	f()
end
