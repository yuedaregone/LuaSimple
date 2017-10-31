local utility = {}

function utility.get_time()	
	local tb_cur = os.date("*t", os.time())
	return tb_cur.hour, tb_cur.min, tb_cur.sec
end

function utility.get_date()
	local tb_cur = os.date("*t", os.time())
	return tb_cur.year, tb_cur.month, tb_cur.day
end


return utility
