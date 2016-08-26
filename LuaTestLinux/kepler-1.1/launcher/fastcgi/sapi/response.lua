----------------------------------------------------------------------------
-- $Id: response.lua,v 1.3 2007/04/14 22:13:12 tomas Exp $
----------------------------------------------------------------------------

local require, unpack = require, unpack
local format = string.format

module"sapi.fastcgi.response"

----------------------------------------------------------------------------
function open (req)
	local lfcgi = require"lfcgi"
	return {
		contenttype = function (header)
			lfcgi.stdout:write ("Content-type: "..header.."\n\n")
		end,

		errorlog = function (msg)
			lfcgi.stderr:write (msg)
		end,

		header = function (header, value)
			lfcgi.stdout:write (format ("%s: %s\n", header, value))
		end,

		redirect = function (url)
			lfcgi.stdout:write ("Location: "..url.."\n\n")
		end,

		write = function (...)
			lfcgi.stdout:write (...)
		end,
	}
end
