----------------------------------------------------------------------------
-- $Id: info.lua,v 1.1 2005/12/29 11:12:41 tomas Exp $
----------------------------------------------------------------------------

module"sapi.fastcgi.info"

----------------------------------------------------------------------------
function open (req)
	return {
		_COPYRIGHT = "Copyright (C) 2004-2005 Kepler Project",
		_DESCRIPTION = "FastCGI SAPI implementation",
		_VERSION = "FastCGI SAPI 1.1",
		ispersistent = true,
	}
end
