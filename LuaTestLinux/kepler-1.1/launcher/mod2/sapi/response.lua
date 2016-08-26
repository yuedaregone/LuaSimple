----------------------------------------------------------------------------
-- $Id: response.lua,v 1.3 2007/07/14 11:03:23 tomas Exp $
----------------------------------------------------------------------------

local ap = require"apache2"

module"sapi.mod2.response"

----------------------------------------------------------------------------
function open (req)
	return {
		contenttype = function (header)
			return ap.set_content_type (req, header)
		end,

		errorlog = function (msg, errlevel)
			return ap.log_error (req, msg, errlevel)
		end,

		header = function (header, value)
			return ap.add_err_headers_out (req, header, value)
		end,

		redirect = function (url)
			ap.set_headers_out (req, "Location", url)
			return ap.HTTP_MOVED_TEMPORARILY
		end,

		write = function (...)
			return ap.rwrite (req, ...)
		end,
	}
end
