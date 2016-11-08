local reptile = {
    url = "http://girl-atlas.com/",
    dest = "D:/img/",
    cur_page = 1,
    tag_title = "<h2><a href=\"(.-)\">.-</a></h2>",
}


function reptile:init()
    self.cur_page = 1
    self:deal_with_one_page(self.url)
    --self:downloadFromUrl("http://girlatlas.b0.upaiyun.com/2554/20160520/01084jde49sq88ivg670.jpg!lrg")
end

function reptile:get_cur_url()
    return string.format("%s?p=%d", self.url, self.cur_page)
end

function reptile:deal_with_one_page(url)
    print("page:"..self.cur_page)
    local html = Http.fetchHtml(url)
    if html == nil then
        print(string.format("error: can't fetch url (%s).", url))
        self.cur_page = self.cur_page + 1
        self:deal_with_one_page(self:get_cur_url())
        return
    end
    print("fetch over")

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
end

function reptile:downloadFromUrl(url)
    print("downloading:"..url)
    local file_name = ""
    string.gsub(url, ".+/(.-)%.", function(s)
        file_name = s..".jpg"
        return s
    end)
    local u = string.gsub(url, "lrg", function(s)
        return "mid"
    end)
    Http.download(u, self.dest..file_name)
end

reptile:init()
return reptile
