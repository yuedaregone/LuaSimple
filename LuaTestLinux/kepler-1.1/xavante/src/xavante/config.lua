-------------------------------------------------------------------------------
-- Xavante configuration file.
--
--
-- Authors: Javier Guerra and Andre Carregal
-- Copyright (c) 2004-2007 Kepler Project
---
-- $Id: config.lua,v 1.24 2007/08/20 22:20:44 carregal Exp $
------------------------------------------------------------------------------

require "xavante.httpd"

require "xavante.vhostshandler"
require "xavante.urlhandler"
require "xavante.indexhandler"
require "xavante.filehandler"
require "xavante.cgiluahandler"


xavante.httpd.handle_request = xavante.vhostshandler {
	[""] = xavante.urlhandler {
		["/"] = xavante.indexhandler ("/cgi/index.lp"),
		["/cgi/"] = xavante.cgiluahandler.makeHandler (XAVANTE_WEB),
		["/img/"] = xavante.filehandler (XAVANTE_WEB.."/img"),
	}
}

xavante.httpd.register ("*", 8080, "Xavante 1.3")
