local reptile = {
    url = "http://girl-atlas.com/",
    dest = "D:/img/",
    cur_page = 1,
    tag_title = "<h2><a href=\"(.-)\">.-</a></h2>",
    url_headers = {
        "Accept: image/png, image/svg+xml, image/*;q=0.8, */*;q=0.5",
        "Accept-Encoding: gzip, deflate",
        "Accept-Language: zh-CN",
        "Connection: Keep-Alive",
        "DNT: 1",
        "Host: girlatlas.b0.upaiyun.com",
        "Referer: http://girl-atlas.com",
        "User-Agent: Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; WOW64; Trident/6.0)",
    }
}


function reptile:init()
    Http.initHttp()
    self.cur_page = 1
    self:deal_with_one_page(self.url)
end

function reptile:get_cur_url()
    return string.format("%s?p=%d", self.url, self.cur_page)
end

function reptile:deal_with_one_page(url)
    print("page:"..self.cur_page)
    Http.resetHeader()
    local html = Http.fetchHtml(url)
    if html == nil then
        print(string.format("error: can't fetch url (%s).", url))
        self.cur_page = self.cur_page + 1
        self:deal_with_one_page(self:get_cur_url())
        return
    end

    local atlas = {}
    string.gsub(html, self.tag_title, function(s)
        atlas[#atlas + 1] = s
        return s
    end)

    for i,v in ipairs(atlas) do
        local atlas_url = self.url..v
        self:deal_with_one_atlas(atlas_url)
    end

    self.cur_page = self.cur_page + 1
    self:deal_with_one_page(self:get_cur_url())
end

function reptile:deal_with_one_atlas(url)
    Http.resetHeader()
    local html = Http.fetchHtml(url)
    if html == nil then return end

    local title = ""
    string.gsub(html, "<title>(.-)</title>", function(s)
        title = s
        return s
    end)
    print(title)

    string.gsub(html, "<img title=.-src=\"([^\n<]-)\" />", function(s)
        self:downloadFromUrl(s)
        return s
    end)
    string.gsub(html, "<img title=.-delay=\"([^\n<]-)\" />", function(s)
        self:downloadFromUrl(s)
        return s
    end)
end

function reptile:downloadFromUrl(url)
    print("downloading:"..url)
    local file_name = ""
    string.gsub(url, ".+/(.-)%.", function(s)
        file_name = s..".jpg"
        return s
    end)
    Http.setHeader(self.url_headers)
    Http.download(url, self.dest..file_name)
end

function reptile:destroy()
    Http.destroyHttp()
end

reptile:init()
return reptile
