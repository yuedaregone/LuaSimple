----------------------------------------------------------------------------
-- $Id: request.lua,v 1.2 2006/01/02 17:55:48 tomas Exp $
----------------------------------------------------------------------------

local getenv, require = os.getenv, require

module"sapi.fastcgi.request"

----------------------------------------------------------------------------
function open (req)
	local lfcgi = require"lfcgi"
	return {
		servervariable = function (n)
			return lfcgi.getenv (n) or getenv (n)
		end,

		getpostdata = function (n)
			return lfcgi.stdin:read (n)
		end,
	}
end
