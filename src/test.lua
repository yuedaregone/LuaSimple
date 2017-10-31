print("Hello World!")
print(math.abs(-10))
print(MyLib.my_add(1, 100))

local url = "https://cn.bing.com/az/hprichbg/rb/InspirationPoint_ZH-CN7836594587_1920x1080.jpg"
print(Http.fetchHtml("https://cn.bing.com"))
Http.download(url, "D://1234.jpg")

function say_hello(param)
	print("HelloWorld!")
	print(param)
end
MyLib.my_test_callback(say_hello)
