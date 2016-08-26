----------------------------------------------------------------------------
-- $Id: response.lua,v 1.2 2007/04/14 22:16:52 tomas Exp $
----------------------------------------------------------------------------

local unpack = unpack
local format = string.format
local stderr, stdout = io.stderr, io.stdout

module"sapi.cgi.response"

----------------------------------------------------------------------------
function open (req)
	return {
		contenttype = function (header)
			stdout:write ("Content-type: "..header.."\n\n")
		end,

		errorlog = function (msg, errlevel)
			stderr:write (msg)
		end,

		header = function (header, value)
			stdout:write (format ("%s: %s\n", header, value))
		end,

		redirect = function (url)
			stdout:write ("Location: "..url.."\n\n")
		end,

		write = function (...)
			stdout:write (...)
		end,
	}
end
