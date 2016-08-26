----------------------------------------------------------------------------
-- $Id: info.lua,v 1.1 2005/12/06 16:28:45 tomas Exp $
----------------------------------------------------------------------------

module"sapi.mod2.info"

function open (req)
	return {
		_COPYRIGHT = "Copyright (C) 2004-2005 Kepler Project",
		_DESCRIPTION = "Apache 2 module SAPI implementation",
		_VERSION = "Apache 2 module SAPI 1.0",
		ispersistent = not LUA_STATE_PER_REQUEST,
	}
end
