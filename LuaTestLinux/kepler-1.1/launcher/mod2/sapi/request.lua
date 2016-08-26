----------------------------------------------------------------------------
-- $Id: request.lua,v 1.4 2006/01/26 18:54:38 tomas Exp $
----------------------------------------------------------------------------

local assert = assert
local ap = require"apache2"

module"sapi.mod2.request"

----------------------------------------------------------------------------
-- Input general information
local ap_func = {
	AUTH_TYPE = ap.auth_type,
	CONTENT_LENGTH = function (req)
		return ap.get_header (req, "Content-length")
	end,
	CONTENT_TYPE = function (req)
		return ap.get_header (req, "Content-type")
	end,
	GATEWAY_INTERFACE = function (req) return "CGI/1.1" end,
	HTTP_COOKIE = function (req) return ap.get_header (req, "Cookie") end,
	HTTP_REFERER = function (req) return ap.get_header (req, "Referer") end,
	PATH_INFO = ap.path_info,  -- !!!!!!!!!!!!!!!! see server/util_script.c
	PATH_TRANSLATED = ap.filename,  -- !!!!!!!!!!!!!!!! see server/util_script.c
	QUERY_STRING = ap.args,
	REMOTE_ADDR = function (req)
		return ap.conn_remote_addr (req)
	end,
	REMOTE_HOST = ap.get_remote_host,
	REMOTE_PORT = ap.port,
	REMOTE_USER = ap.user,
	REQUEST_METHOD = ap.method,
	SCRIPT_NAME = ap.uri,  -- !!!!!!!!!!!!!!!! see server/util_script.c
	SERVER_NAME = ap.get_server_name,
	SERVER_PORT = ap.get_server_port,
	SERVER_PROTOCOL = ap.protocol,
	SERVER_SOFTWARE = ap.get_server_version,
}

----------------------------------------------------------------------------
function open (req)
	local first_get_post = true
	return {
		servervariable = function (n)
			local f = ap_func[n]
			return f and f(req)
		end,

		getpostdata = function (n)
			if first_get_post then
				assert (ap.setup_client_block (req, "REQUEST_CHUNKED_ERROR") == ap.OK)
				first_get_post = false
				if ap.should_client_block (req) ~= 0 then
					return ap.get_client_block (req, n)
				end
			else
				return ap.get_client_block (req, n)
			end
		end,
	}
end
