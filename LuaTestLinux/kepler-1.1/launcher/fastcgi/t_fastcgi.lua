---------------------------------------------------------------------
-- Main Lua script.
-- This script should be run by the FastCGI module.
-- It waits for a request and executes CGILua's main script (main.lua).
--
-- $Id: t_fastcgi.lua,v 1.8 2007/01/10 22:51:28 mascarenhas Exp $
---------------------------------------------------------------------

-- Kepler bootstrap
local bootstrap, err = loadfile(os.getenv("KEPLER_INIT") or [[KEPLER_INIT]])
if bootstrap then
  bootstrap()
else
  io.stderr:write(tostring(err))
  return nil
end

require"lfcgi"
require"rings"

while lfcgi.accept() >= 0 do
	local s = rings.new ()
	local ok, ret = s:dostring ([=[
	-- Kepler bootstrap
	local bootstrap, err = loadfile(os.getenv("KEPLER_INIT") or [[KEPLER_INIT]])
	if bootstrap then
	  bootstrap()
	else
	  io.stderr:write(tostring(err))
	  return nil
	end
    	local info = require"sapi.fastcgi.info"
    	local req = require"sapi.fastcgi.request"
    	local res = require"sapi.fastcgi.response"
    	SAPI = {
        	Info = info.open (),
        	Request = req.open (),
        	Response = res.open (),
    	}

	local lfcgi = require"lfcgi"
	io.stdout:close()
	io.stdout = lfcgi.stdout
	io.stderr:close()
	io.stderr = lfcgi.stderr
	io.stdin:close()
	io.stdin = lfcgi.stdin

    	require"cgilua"
    	return cgilua.main ()
	]=])
	s:close ()
end

