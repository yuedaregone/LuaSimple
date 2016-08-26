----------------------------------------------------------------------------
-- Main Lua script.
-- It executes the requested Lua script.
-- It also gets all POST data (in case it exists) and create a string with it.
--
-- $Id: t_isapi.lua,v 1.7 2007/07/17 22:16:08 tomas Exp $
----------------------------------------------------------------------------

----------------------------------------------------------------------------
-- Setting the Basic API.
local function set_api()
	local headers = {
		Status = "200 OK"
	}
	SAPI = {
		Response = {},
		Request = {},
		Info = { -- Information data
			_COPYRIGHT = "Copyright (C) 2004-2005 Kepler Project",
			_DESCRIPTION = "ISAPI SAPI implementation",
			_NAME = "ISAPI SAPI",
			_VERSION = "1.0",
			ispersistent = true,
		},
	}
	-- Headers
	SAPI.Response.contenttype = function (s)
		headers["Content-Type"] = s
	end
	SAPI.Response.redirect = function (url)
		isapi.redirect(url)
	end
	SAPI.Response.header = function (h,v)
		headers[h] = v
	end
	-- Contents
	
	local first_write = true
	
	local function send_headers()
		local str_headers, str_status = {}, ""
		local size_headers, size_status = 0, 0
		for k, v in pairs(headers) do
			if k == "Status" then
				size_status = string.len(v)
				str_status = v
			else
				local header = k .. ": " .. v .. "\n"
				table.insert(str_headers, header)
			end
		end
		table.insert(str_headers, "\n")
		str_headers = table.concat(str_headers)
		size_headers = string.len(str_headers)
		isapi.send_headers(str_status, size_status, str_headers, size_headers)
	end
	
	SAPI.Response.write = function (...)
		if first_write then
			first_write = false
			send_headers()
		end
		isapi.write(...)
	end
	SAPI.Response.errorlog = isapi.log_error

	local total_bytes = isapi.get_total_bytes()
	local cached_data = isapi.get_cached_data()
	local data_available = isapi.get_data_available()
	
	local bytes_read = 0
	
	SAPI.Request.getpostdata = function (n)
		local postdata = ""
		if bytes_read >= total_bytes then
			return nil
		elseif bytes_read + n <= data_available then
			postdata = string.sub(cached_data, bytes_read + 1, bytes_read + n)
			bytes_read = bytes_read + n
			return postdata
		elseif bytes_read < data_available then
			postdata = string.sub(cached_data, bytes_read + 1, data_available)
			local read = data_available - bytes_read
			n = n - read
			bytes_read = bytes_read + read
		end
		bytes_read = bytes_read + n
		return postdata .. isapi.get_postdata(n)
	end

	-- Input general information
	local isapi_func = {
		CONTENT_LENGTH = isapi.get_total_bytes,
		CONTENT_TYPE = isapi.get_content_type,
		PATH_INFO = isapi.get_path_info,
		PATH_TRANSLATED = isapi.get_path_translated,
		QUERY_STRING = isapi.get_query_string,
		REQUEST_METHOD = isapi.get_request_method
	}
	SAPI.Request.servervariable = function (name)
		local f = isapi_func[name]
		return f and f() or isapi.get_server_variable(name)
	end
end

----------------------------------------------------------------------------
-- Trying to load and execute the "mainscript".

-- Kepler bootstrap
local bootstrap, err = loadfile(os.getenv("KEPLER_INIT") or [[KEPLER_INIT]])
if bootstrap then
  bootstrap()
else
  isapi.log_error(tostring(err))
  return nil
end

set_api()

do
  local tab = {}
  function stable.get(k)
    return tab[k]
  end
  function stable.set(k, v)
    tab[k] = v
  end
end

require"lfs"
require"cgilua"

cgilua.main()


