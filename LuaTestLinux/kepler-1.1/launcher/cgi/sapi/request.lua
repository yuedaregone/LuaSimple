----------------------------------------------------------------------------
-- $Id: request.lua,v 1.1 2005/12/29 11:12:41 tomas Exp $
----------------------------------------------------------------------------

local getenv = os.getenv
local stdin = io.stdin

module"sapi.cgi.request"

----------------------------------------------------------------------------
function open (req)
	return {
		servervariable = getenv,
		getpostdata = function (n)
			return stdin:read (n)
		end,
	}
end
