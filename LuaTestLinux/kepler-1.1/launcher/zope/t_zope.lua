----------------------------------------------------------------------------
-- Main Lua script.
-- It executes the requested Lua script.
-- It also gets all POST data (in case it exists) and create a string with it.
--
-- $Id: t_zope.lua,v 1.2 2005/06/17 01:50:16 tomas Exp $
----------------------------------------------------------------------------

----------------------------------------------------------------------------
-- Setting the Basic API.
local function set_api()
	local headers = {
		Status = "200 OK"
	}

	SAPI = {
		Response = {},
		Request = {},
		Info = { -- Information data
			_COPYRIGHT = "Copyright (C) 2004-2005 Kepler Project",
			_DESCRIPTION = "Zope SAPI implementation",
			_NAME = "Zope SAPI",
			_VERSION = "1.0",
			ispersistent = true,
		},
	}
	-- Headers
	SAPI.Response.contenttype = function (s)
		headers["Content-Type"] = s
	end
	SAPI.Response.redirect = function (url)
		zope.redirect(url)
	end
	SAPI.Response.header = function (h,v)
		headers[h] = v
	end
	-- Contents
	
	local first_write = true
	
	local function send_headers()
		for k, v in pairs(headers) do
			if k == "Status" then
                zope.set_status(v)
			else
                zope.set_header(k, v)
			end
		end
	end
	
	SAPI.Response.write = function (s)
		if first_write then
			first_write = false
			send_headers()
		end
		zope.write(s)
	end
	SAPI.Response.errorlog = function (s) zope.log_error(s) end

	local total_bytes = zope.get_total_bytes()
	local post_data = zope.get_post_data()
	
	local bytes_read = 0
	
	SAPI.Request.getpostdata = function (n)
		local postdata
		if bytes_read >= total_bytes then
			return nil
		elseif bytes_read + n <= total_bytes then
			postdata = string.sub(post_data, bytes_read + 1, bytes_read + n)
			bytes_read = bytes_read + n
			return postdata
		else
			postdata = string.sub(post_data, bytes_read + 1, total_bytes)
			bytes_read = total_bytes
			return postdata
		end
	end

	SAPI.Request.servervariable = function (name)
		return zope.get_server_variable(name)
	end
end

----------------------------------------------------------------------------
-- Trying to load and execute the "mainscript".

-- compatibility code for Lua version 5.0 providing 5.1 behavior
if string.find (_VERSION, "Lua 5.0") and not _COMPAT51 then
	if not LUA_PATH then
		LUA_PATH = [[LUA_PATH]]
	end
	require"compat-5.1"
	package.cpath = [[LUA_CPATH]]
end

set_api()

require"venv"
require"lfs"
require"cgilua"

local f = venv (function () require"cgilua" cgilua.main() end)
f()
