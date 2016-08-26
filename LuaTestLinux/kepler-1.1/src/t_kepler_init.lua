-- Lua 5.1 init file for Kepler
--
-- Prepares the paths for Lua and C modules using three parameters:
--    conf    - configuration files (optional, checked before the Lua modules)
--    luabase - Lua modules
--    libbase - C modules
--
-- (the paths should end in /)
--
-- Defines the default web directory for Lua Pages and Lua Scripts
--
-- $Id: t_kepler_init.lua,v 1.11 2007/08/29 19:21:07 carregal Exp $

-- Lua 5.1 paths
local conf51    = [[KEPLER_CONF]]
local luabase51 = [[LUABASE51]]
local libbase51 = [[LIBBASE51]]

-- Library extension used in the system (dll, so etc)
local libext = [[LIB_EXT]]

-- CGILua applications directory
CGILUA_APPS = [[CGILUA_APPS]]

-- Xavante default web directory
XAVANTE_WEB  = [[XAVANTE_WEB]]

-- Xavante default logfile
XAVANTE_LOG  = [[XAVANTE_LOG]]

--------- end of parameters ------------

local function expandPath(base, conf)
  local path = ""
  if conf and conf ~= "" then
    path = path..conf..[[/?.lua;]]
  end
  path = path..base..[[/?.lua;]]..base..[[/?/init.lua]]
  return path
end

local function expandCPath(base)
  return base..[[/?.]]..libext
end

if string.find (_VERSION, "Lua 5.1") then
  package.path = expandPath(luabase51, conf51)
  package.cpath = expandCPath(libbase51)
else
  error("This init file works only with Lua 5.1")
end

