-------------------------------------------------------------------------------
-- Starts the Xavante Web server.
--
-- See xavante/config.lua and the online documentation for configuration details.
--
-- Authors: Javier Guerra and Andre Carregal
-- Copyright (c) 2004-2007 Kepler Project
--
-- $Id: xavante_start.lua,v 1.3 2007/08/29 19:22:31 carregal Exp $
-------------------------------------------------------------------------------

-- Kepler bootstrap
local bootstrap, err = loadfile("kepler_init.lua") or loadfile(os.getenv("KEPLER_INIT") or "") or loadfile([[KEPLER_INIT]])
if bootstrap then
  bootstrap()
else
  io.stderr:write(tostring(err))
  return nil
end

if not REDIRECT_OUTPUT then
  REDIRECT_OUTPUT = function (x) end
end

REDIRECT_OUTPUT(XAVANTE_LOG)

local res, err = pcall(require, "xavante")

if not res then
  io.stderr:write(err .. "\n")
else
  -------------------------------------------------------------------------------
  -- Loads the configuration file and starts Xavante
  --
  -- XAVANTE_ISFINISHED and XAVANTE_TIMEOUT are optional globals that can
  -- control how Xavante will behave when being externally controlled.
  -- XAVANTE_ISFINISHED is a function to be called on every step of Xavante,
  -- XAVANTE_TIMEOUT is the timeout to be used by Copas.
  -------------------------------------------------------------------------------
  io.stdout:write("[xavante launcher] Starting Xavante...\n")
  res, err = pcall(xavante.start, XAVANTE_ISFINISHED, XAVANTE_TIMEOUT)
  if not res then
    io.stderr:write(err .. "\n")
  else
    io.stderr:write("[xavante launcher] Xavante stopped\n")
  end
end


