----------------------------------------------------------------------------
-- Main Lua script.
-- It executes the requested Lua script.
--
-- $Id: t_mod2.lua,v 1.11 2007/03/20 17:31:30 tomas Exp $
----------------------------------------------------------------------------

local info = require"sapi.mod2.info"
local req = require"sapi.mod2.request"
local res = require"sapi.mod2.response"

SAPI = {
Info = info.open (...),
Request = req.open (...),
Response = res.open (...),
}

stable = {}

do
    local tab = {}
    function stable.get(k)
        return tab[k]
    end
    function stable.set(k, v)
        tab[k] = v
    end
end

require"cgilua"
local ok, ret = pcall(cgilua.main)

if ok and type(ret) == "number" then
return ret
elseif not ok and ret then
apache2.log_error (..., tostring(ret), "error")
end

return apache2.OK
