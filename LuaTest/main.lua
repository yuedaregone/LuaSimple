--print("Hello World!")
--print(math.abs(-10))
--print(MyLib.my_add(1, 100))

function get_delay()
	return 1/60*1000;
end

local isReturn = true
function main_loop()	
	print("lua loop")
	md5 = require "lua/md5"
	print(md5.md5_code("12134"))
	
	return isReturn;
end
isReturn = false



