local md5 = {}
function md5.md5_code(str)
	local md5 = require "md5.core"
	local md5_str = md5.sum(str)
	return (string.gsub(md5_str, ".", function (c)
        return string.format("%02x", string.byte(c))
        end))
end
return md5

--[[
sample:
md5 = require "md5"
print(md5.md5_code("12134"))
]]