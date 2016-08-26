----------------------------------------------------------------------------
-- $Id: info.lua,v 1.1 2005/12/29 11:12:41 tomas Exp $
----------------------------------------------------------------------------

module"sapi.cgi.info"

function open (req)
	return {
		_COPYRIGHT = "Copyright (C) 2004-2005 Kepler Project",
		_DESCRIPTION = "CGI SAPI implementation",
		_VERSION = "CGI SAPI 1.0",
		ispersistent = false,
	}
end
