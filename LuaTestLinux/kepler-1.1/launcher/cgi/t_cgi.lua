---------------------------------------------------------------------
-- Main Lua script.
-- This script should be run by the executable.
-- $Id: t_cgi.lua,v 1.9 2007/05/21 17:15:42 carregal Exp $
---------------------------------------------------------------------

-- Kepler bootstrap
local bootstrap, err = loadfile("kepler_init.lua") or loadfile(os.getenv("KEPLER_INIT") or "") or loadfile([[KEPLER_INIT]])

if bootstrap then
  bootstrap()
end

local info = require"sapi.cgi.info"
local req = require"sapi.cgi.request"
local res = require"sapi.cgi.response"
SAPI = {
	Info = info.open (),
	Request = req.open (),
	Response = res.open (),
}

require"cgilua"
return cgilua.main()
