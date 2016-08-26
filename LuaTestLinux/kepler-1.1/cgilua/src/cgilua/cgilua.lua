----------------------------------------------------------------------------
-- CGILua library.
--
-- @release $Id: cgilua.lua,v 1.46 2007/08/21 20:15:55 carregal Exp $
----------------------------------------------------------------------------

local _G, SAPI = _G, SAPI
local urlcode = require"cgilua.urlcode"
local lp = require"cgilua.lp"
local post = require"cgilua.post"
local lfs = require"lfs"
local debug = require"debug"
local assert, error, ipairs, select, tostring, type, unpack, xpcall = assert, error, ipairs, select, tostring, type, unpack, xpcall
local gsub, format, strfind, strlower, strsub = string.gsub, string.format, string.find, string.lower, string.sub
local setmetatable = setmetatable
local _open = io.open
local tinsert, tremove = table.insert, table.remove
local date = os.date
local seeall = package.seeall

lp.setoutfunc ("cgilua.put")
lp.setcompatmode (true)

module ("cgilua")

--
-- Internal state variables.
local default_errorhandler = debug.traceback
local errorhandler = default_errorhandler
local default_erroroutput = function (msg)

    if type(msg) ~= "string" and type(msg) ~= "number" then
        msg = format ("bad argument #1 to 'error' (string expected, got %s)", type(msg))
    end
  
	-- Logging error
	SAPI.Response.errorlog (msg)
	SAPI.Response.errorlog (" ")

	SAPI.Response.errorlog (SAPI.Request.servervariable"REMOTE_ADDR")
	SAPI.Response.errorlog (" ")

	SAPI.Response.errorlog (date())
	SAPI.Response.errorlog ("\n")

	-- Building user message
	SAPI.Response.contenttype ("text/html")
	msg = gsub (gsub (msg, "\n", "<br>\n"), "\t", "&nbsp;&nbsp;")
	SAPI.Response.write (msg)
end
local erroroutput = default_erroroutput
local default_maxfilesize = 512 * 1024
local maxfilesize = default_maxfilesize
local default_maxinput = 1024 * 1024
local maxinput = default_maxinput
script_path = false

_COPYRIGHT = "Copyright (C) 2003-2007 Kepler Project"
_DESCRIPTION = "CGILua is a tool for creating dynamic Web pages and manipulating input data from forms"
_VERSION = "CGILua 5.1.0"

--
-- Header functions

----------------------------------------------------------------------------
-- Sends a header.
-- @param header String with the header.
-- @param value String with the corresponding value.
----------------------------------------------------------------------------
header = SAPI.Response.header

----------------------------------------------------------------------------
-- Sends a Content-type header.
-- @param type String with the type of the header.
-- @param subtype String with the subtype of the header.
----------------------------------------------------------------------------
function contentheader (type, subtype)
	SAPI.Response.contenttype (type..'/'..subtype)
end

----------------------------------------------------------------------------
-- Sends the HTTP header "text/html".
----------------------------------------------------------------------------
function htmlheader()
	SAPI.Response.contenttype ("text/html")
end
local htmlheader = htmlheader

----------------------------------------------------------------------------
-- Sends an HTTP header redirecting the browser to another URL
-- @param url String with the URL.
-- @param args Table with the arguments (optional).
----------------------------------------------------------------------------
function redirect (url, args)
	if strfind(url,"^https?:") then
		local params=""
		if args then
			params = "?"..urlcode.encodetable(args)
		end
		return SAPI.Response.redirect(url..params)
	else
		return SAPI.Response.redirect(mkabsoluteurl(mkurlpath(url,args)))
	end
end

----------------------------------------------------------------------------
-- Returns a server variable
-- @param name String with the name of the server variable.
-- @return String with the value of the server variable.
----------------------------------------------------------------------------
servervariable = SAPI.Request.servervariable

----------------------------------------------------------------------------
-- Primitive error output function
-- @param msg String (or number) with the message.
-- @param level String with the error level (optional).
----------------------------------------------------------------------------
function errorlog (msg, level)
	local t = type(msg)
	if t == "string" or t == "number" then
		SAPI.Response.errorlog (msg, level)
	else
		error ("bad argument #1 to `cgilua.errorlog' (string expected, got "..t..")", 2)
	end
end

----------------------------------------------------------------------------
-- Converts all its arguments to strings before sending them to the server.
----------------------------------------------------------------------------
function print (...)
	local args = { ... }
	for i = 1, select("#",...) do
		args[i] = tostring(args[i])
	end
	SAPI.Response.write (unpack(args))
end

----------------------------------------------------------------------------
-- Function 'put' sends its arguments (basically strings of HTML text)
--  to the server
-- Its basic implementation is to use Lua function 'write', which writes
--  each of its arguments (strings or numbers) to file _OUTPUT (a file
--  handle initialized with the file descriptor for stdout)
-- @param s String (or number) with output.
----------------------------------------------------------------------------
put = SAPI.Response.write

----------------------------------------------------------------------------
-- Remove globals not allowed in CGILua scripts.
-- @param notallowed Array of Strings with the names to be removed.
----------------------------------------------------------------------------
function removeglobals (notallowed)
	for _, g in ipairs(notallowed) do
		if type(_G[g]) ~= "function" then
			_G[g] = nil
		else
			_G[g] = function()
				 error("Function '"..g.."' is not allowed in CGILua scripts.")
			end
		end
	end
end

----------------------------------------------------------------------------
-- Execute a script
--  If an error is found, Lua's error handler is called and this function
--  does not return
-- @param filename String with the name of the file to be processed.
-- @return The result of the execution of the file.
----------------------------------------------------------------------------
function doscript (filename)
	local res, err = _G.loadfile(filename)
	if not res then
		error (format ("Cannot execute `%s'. Exiting.\n%s", filename, err))
	else
		return res ()
	end
end

----------------------------------------------------------------------------
-- Execute the file if there is no "file error".
--  If an error is found, and it is not a "file error", Lua 'error'
--  is called and this function does not return
-- @param filename String with the name of the file to be processed.
-- @return The result of the execution of the file or nil (in case the
--	file does not exists or if it cannot be opened).
-- @return It could return an error message if the file cannot be opened.
----------------------------------------------------------------------------
function doif (filename)
	if not filename then return end    -- no file
	local f, err = _open(filename)
	if not f then return nil, err end    -- no file (or unreadable file)
	f:close()
	return doscript (filename)
end

---------------------------------------------------------------------------
-- Set the maximum "total" input size allowed (in bytes)
-- @param nbytes Number of the maximum size (in bytes) of the whole POST data.
---------------------------------------------------------------------------
function setmaxinput(nbytes)
	maxinput = nbytes
end

---------------------------------------------------------------------------
-- Set the maximum size for an "uploaded" file (in bytes)
-- Might be less or equal than maxinput.
-- @param nbytes Number of the maximum size (in bytes) of a file.
---------------------------------------------------------------------------
function setmaxfilesize(nbytes)
	maxfilesize = nbytes
end

----------------------------------------------------------------------------
-- Preprocess the content of a mixed HTML file and output a complete
--   HTML document ( a 'Content-type' header is inserted before the
--   preprocessed HTML )
-- @param filename String with the name of the file to be processed.
----------------------------------------------------------------------------
function handlelp (filename)
	htmlheader ()
	lp.include (filename)
end

----------------------------------------------------------------------------
-- Builds a handler that sends a header and the contents of the given file.
-- Sends the contents of the file to the output without processing it.
-- @param type String with the type of the header.
-- @param subtype String with the subtype of the header.
-- @return Function (which receives a filename as argument) that produces
--	the header and copies the content of the given file.
----------------------------------------------------------------------------
function buildplainhandler (type, subtype)
	return function (filename)
		contentheader (type, subtype)
		local fh = assert (_open (filename))
		local prog = fh:read("*a")
		fh:close()
		put (prog)
	end
end

----------------------------------------------------------------------------
-- Builds a handler that sends a header and the processed file.
-- Processes the file as a Lua Page.
-- @param type String with the type of the header.
-- @param subtype String with the subtype of the header.
-- @return Function (which receives a filename as argument) that produces
--	the header and processes the given file.
----------------------------------------------------------------------------
function buildprocesshandler (type, subtype)
	return function (filename)
		contentheader (type, subtype)
		lp.include (filename)
	end
end

----------------------------------------------------------------------------
-- Create an URL path to be used as a link to a CGILua script
-- @param script String with the name of the script.
-- @param args Table with arguments to script (optional).
-- @return String in URL format.
----------------------------------------------------------------------------
function mkurlpath (script, args)
	-- URL-encode the parameters to be passed do the script
	local params = ""
	if args then
		params = "?"..urlcode.encodetable(args)
	end
	if strsub(script,1,1) == "/" then
		return urlpath .. script .. params
	else
		return urlpath .. script_vdir .. script .. params
	end
end

----------------------------------------------------------------------------
-- Create an absolute URL containing the given URL path
-- @param path String with the path.
-- @param protocol String with the name of the protocol (default = "http").
-- @return String in URL format.
----------------------------------------------------------------------------
function mkabsoluteurl (path, protocol)
	protocol = protocol or "http"
	if path:sub(1,1) ~= '/' then
		path = '/'..path
	end
	return format("%s://%s:%s%s",
		protocol,
		servervariable"SERVER_NAME",
		servervariable"SERVER_PORT",
		path)
end

----------------------------------------------------------------------------
-- Extract the "directory" and "file" parts of a path
-- @param path String with a path.
-- @return String with the directory part.
-- @return String with the file part.
----------------------------------------------------------------------------
function splitpath (path)
	local _,_,dir,file = strfind(path,"^(.-)([^:/\\]*)$")
	return dir,file
end

--
-- Define variables and build the cgilua.POST, cgilua.GET and the global `cgi' table.
-- @param args Table where to store the parameters (the actual `cgi' table).
--
local function getparams ()
	-- Define variables.
	script_path = script_path or servervariable"PATH_TRANSLATED"
    if not script_path then
        script_path = servervariable"DOCUMENT_ROOT" ..
            servervariable"SCRIPT_NAME"
    end
    script_pdir, script_file = splitpath (script_path)
	local vpath = servervariable"PATH_INFO"
	script_vpath = vpath
	if vpath and vpath ~= "" then
		script_vdir = splitpath (vpath)
		urlpath = servervariable"SCRIPT_NAME"
	else
		script_vdir = splitpath (servervariable"SCRIPT_NAME")
		urlpath = ""
	end
	-- Fill in the POST table.
	POST = {}
	if servervariable"REQUEST_METHOD" == "POST" then
		post.parsedata {
			read = SAPI.Request.getpostdata,
			discardinput = ap and ap.discard_request_body,
			content_type = servervariable"CONTENT_TYPE",
			content_length = servervariable"CONTENT_LENGTH",
			maxinput = maxinput,
			maxfilesize = maxfilesize,
			args = POST,
		}
	end
	-- Fill in the QUERY table.
	QUERY = {}
	urlcode.parsequery (servervariable"QUERY_STRING", QUERY)
	-- Links POST and QUERY data to the CGI table for backward compatibility
	local mt = {}
	mt.__index = function(t,v) return POST[v] or QUERY[v] end
	setmetatable(CGI, mt)
end

--
-- Stores all script handlers and the file extensions used to identify
-- them.
local script_handlers = {}

--
-- Default handler.
-- Sends the contents of the file to the output without processing it.
-- @param filename String with the name of the file.
--
local function default_handler (filename)
	htmlheader ()
	local fh = assert (_open (filename))
	local prog = fh:read("*a")
	fh:close()
	put (prog)
end

----------------------------------------------------------------------------
-- Add a script handler.
-- @param file_extension String with the lower-case extension of the script.
-- @param func Function to handle this kind of scripts.
----------------------------------------------------------------------------
function addscripthandler (file_extension, func)
	assert (type(file_extension) == "string", "File extension must be a string")
	if strfind (file_extension, '%.', 1) then
		file_extension = strsub (file_extension, 2)
	end
	file_extension = strlower(file_extension)
	assert (type(func) == "function", "Handler must be a function")

	script_handlers[file_extension] = func
end

---------------------------------------------------------------------------
-- Obtains the handler corresponding to the given script path.
-- @param path String with a script path.
-- @return Function that handles it or nil.
----------------------------------------------------------------------------
function getscripthandler (path)
	local i,f, ext = strfind (path, "%.([^.]+)$")
	return script_handlers[strlower(ext or '')]
end

---------------------------------------------------------------------------
-- Execute the given path with the corresponding handler.
-- @param path String with a script path.
-- @return The returned values from the script.
---------------------------------------------------------------------------
function handle (path)
	local h = assert (getscripthandler (path), "There is no handler defined to process this kind of file ("..path..")")
	return h (path)
end

---------------------------------------------------------------------------
-- Sets "errorhandler" function
-- This function is called by Lua when an error occurs.
-- It receives the error message generated by Lua and it is resposible
-- for the final message which should be returned.
-- @param Function.
---------------------------------------------------------------------------
function seterrorhandler (f)
	local tf = type(f)
	if tf == "function" then
		errorhandler = f
	else
		error (format ("Invalid type: expected `function', got `%s'", tf))
	end
end

---------------------------------------------------------------------------
-- Defines the "erroroutput" function
-- This function is called to generate the error output.
-- @param Function.
---------------------------------------------------------------------------
function seterroroutput (f)
	local tf = type(f)
	if tf == "function" then
		erroroutput = f
	else
		error (format ("Invalid type: expected `function', got `%s'", tf))
	end
end

--
-- Executes a function with an error handler.
-- @param f Function to be called.
--
local function _xpcall (f)
	local ok, result = xpcall (f, errorhandler)
	if ok then
		return result
	else
		erroroutput (result)
	end
end

--
-- Stores all close functions in order they are set.
local close_functions = {
}

---------------------------------------------------------------------------
-- Adds a function to be executed after the script.
-- @param f Function to be registered.
---------------------------------------------------------------------------
function addclosefunction (f)
	local tf = type(f)
	if tf == "function" then
		tinsert (close_functions, f)
	else
		error (format ("Invalid type: expected `function', got `%s'", tf))
	end
end

--
-- Close function.
--
local function close()
	for i = #close_functions, 1, -1 do
		close_functions[i]()
	end
end

--
-- Stores all open functions in order they are set.
local open_functions = {
}

---------------------------------------------------------------------------
-- Adds a function to be executed before the script.
-- @param f Function to be registered.
---------------------------------------------------------------------------
function addopenfunction (f)
	local tf = type(f)
	if tf == "function" then
		tinsert (open_functions, f)
	else
		error (format ("Invalid type: expected `function', got `%s'", tf))
	end
end

--
-- Open function.
-- Call all defined open-functions in the order they were created.
--
local function open()
	for i = #open_functions, 1, -1 do
		open_functions[i]()
	end
end

--
-- Resets CGILua's state.
--
local function reset ()
	script_path = false
	maxfilesize = default_maxfilesize
	maxinput = default_maxinput
	-- Error Handling
	errorhandler = default_errorhandler
	erroroutput = default_erroroutput
	-- Handlers
	script_handlers = {}
	open_function = {}
	close_functions = {}
end

---------------------------------------------------------------------------
-- Request processing.
---------------------------------------------------------------------------
function main ()
	-- Default values
	addscripthandler ("lua", doscript)
	addscripthandler ("lp", handlelp)
	CGI = {}
	-- Configuring CGILua (trying to load cgilua/config.lua)
	_xpcall (function () _G.require"cgilua.config" end)

	-- Cleaning environment
	removeglobals {
		"os.execute",
		"loadlib",
		"package",
		"debug",
	}
	-- Build fake package
	_G.package = { seeall = seeall, }
	_xpcall (getparams)
	-- Changing curent directory to the script's "physical" dir
	local curr_dir = lfs.currentdir ()
	_xpcall (function () lfs.chdir (script_pdir) end)
	-- Opening function
	_xpcall (open)
	-- Executing script
	local result = _xpcall (function () return handle (script_file) end)

	-- Closing function
	_xpcall (close)
	-- Cleanup
	reset ()
	-- Changing to original directory
	_xpcall (function () lfs.chdir (curr_dir) end)
	if result then -- script executed ok!
		return result
	end
end
