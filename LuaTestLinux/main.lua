--print("Hello World!")
--print(math.abs(-10))
print(MyLib.my_add(1, 100))

function say_hello(param)
	print("HelloWorld!")
	print(param)
end

function get_delay()
	return 1;
end

function init_program()
	print("init")
	--local url = "https://cn.bing.com/az/hprichbg/rb/NottulnHerbst_ZH-CN9638949027_1920x1080.jpg"
	--print(Http.fetchHtml("https://cn.bing.com"))
	--Http.download(url, "D://1234.jpg")
end

function main_loop()
	local utility = require "lualib/utility"
	print(utility.get_date())
	print(utility.get_time())
	MyLib.my_test_callback(say_hello)
	return 0
end

function close_program()
	print("close")
end
