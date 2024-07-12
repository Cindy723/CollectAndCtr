------------------------------------
-- 提示
-- 如果使用其他Lua编辑工具编辑此文档，请将VisualTFT软件中打开的此文件编辑器视图关闭，
-- 因为VisualTFT具有自动保存功能，其他软件修改时不能同步到VisualTFT编辑视图，
-- VisualTFT定时保存时，其他软件修改的内容将被恢复。
--
-- Attention
-- If you use other Lua Editor to edit this file, please close the file editor view 
-- opened in the VisualTFT, Because VisualTFT has an automatic save function, 
-- other Lua Editor cannot be synchronized to the VisualTFT edit view when it is modified.
-- When VisualTFT saves regularly, the content modified by other Lua Editor will be restored.
------------------------------------

--下面列出了常用的回调函数
--更多功能请阅读<<LUA脚本API.pdf>>


--初始化函数
--function on_init()
--end

--定时回调函数，系统每隔1秒钟自动调用。
--function on_systick()
--end

--定时器超时回调函数，当设置的定时器超时时，执行此回调函数，timer_id为对应的定时器ID
--function on_timer(timer_id)
--end

--用户通过触摸修改控件后，执行此回调函数。
--点击按钮控件，修改文本控件、修改滑动条都会触发此事件。
--function on_control_notify(screen,control,value)
--end

--当画面切换时，执行此回调函数，screen为目标画面。
--function on_screen_change(screen)
--end


local g_screen
local g_control
local g_value

-- ee cd cx 自定义指令对应功能
local g_registerButton = 0xc1 -- 注册按钮按下指令
local g_pswEnter = 0xc2		   -- 密码输入结束
local g_nodePageChange = 0xc3   -- 设置界面节点页面切换 
local g_dispPageChange = 0xc4   -- 显示界面页面切换 
local g_deleteNode = 0xc5 -- 删除节点

local sendArray = {0xee, 0xcd} -- 自定数据 帧头 用户数据 数据类型 数据长度 数据 
  
-- lua 数组从下标1开始
function on_init()    
	set_text(2, 7, "") 	
	set_value(2, 41, 1)
	--change_screen(2) -- 开机就切到画面2 
end

-- 字符串转十六进制
function string_to_hex(str)
    local hex = {}
    for i = 1, # (str) do
        local byte = str:byte(i) 
        table.insert(hex, byte)
    end 
   return hex
end

-- 左移数据 因为数据始终有0 去掉最开始的0  
function shift_left(da)
	local tempdata = {0x01}  
	for i = 0, # (da) do  -- 有bug 这里必须是0 
	tempdata[i] = da[i+1]
	end
	return tempdata
end 

 
-- 添加帧为
function add_end(datalen ,arr)
	arr[datalen+5] = 0xff
	arr[datalen+6] = 0xfc
	arr[datalen+7] = 0xff
	arr[datalen+8] = 0xff
end -- error attempt to get length of a nil value 不管

-- 定时器id
local timer_notify = 0
local timer_send_searchid_text = 1 
local timer_send_dispageChange = 12
function on_timer(timer_id)
	
	if ( timer_id == timer_send_searchid_text)then

	end

	if ( timer_id == timer_notify) then
		on_control_notify_my(g_screen, g_control, g_value)
	end

	if( timer_id == timer_send_dispageChange) then
	    print("repeat")
		 local sendData = shift_left(sendArray)	
		 uart_send_data(sendData)
	end

end


-- 用户通过触摸修改控件后，执行此回调函数。
-- 点击按钮控件，修改文本控件、修改滑动条都会触发此事件 
-- 控件必须要有串口数据才会触发回调
function on_control_notify_my(screen, control, value)
	local datalen = 0

	print(string.format("on_control_notify %d", control))


	if (screen == 1 and control == 1) then  -- 进入设置
		set_text(2, 7, "") -- 清除上次输入的密码
	end


 	if (screen == 2 and control == 7) then  -- 输入密码
	-- 随即之后如果操作节点 在单片机判定和预设密码是否一致 不一致时单片机变更提示文本 且不作处理
	end	


	if (screen == 1 and (control == 11)or(control == 12)) then-- 显示列表切换上下页
		  --同 (control == 16)or(control == 17)
		sendArray[3] = g_dispPageChange
		datalen = 2
		local f1int = tonumber(get_text(1, 2))  
 
		if(control == 11)then
			if(f1int > 1) then
				f1int = f1int -1
			end
		end		 		
		if(control == 12)then 
		    if(f1int <  tonumber(get_text(2, 22))) then
				f1int = f1int +1  --需要限制总页
			end
		end	

		set_text(1, 2, string.format("%d", f1int))
		--set_text(1, 22, string.format("%d", f2int))

  		local f1hex = string_to_hex(get_text(1, 2))
  		local f2hex = string_to_hex(get_text(1, 22))

		sendArray[4] = 2
		sendArray[5]  =  f1hex[1]
		sendArray[6]  =  f2hex[1]

		add_end(datalen, sendArray)
		local sendData = shift_left(sendArray)
		uart_send_data(sendData) 		
		start_timer(timer_send_dispageChange, 100, 0, 2) -- 再发2次变化后的数据 防止单片机漏掉
	end


	if (screen == 2 and control == 31) then  -- 點擊配置
		 --change_child_screen (3)

	end


	if (screen == 2 and (control == 16)or(control == 17)) then  -- 节点列表切换上下页
		 --页面 f1 / f2  切换页面 下发变化后分数 同时上报对应页的数据展示 
		sendArray[3] = g_nodePageChange
		datalen = 2
		local f1int = tonumber(get_text(2, 21))  
 
		if(control == 16)then
			if(f1int > 1) then
				f1int = f1int -1
			end
		end		 		
		if(control == 17)then 
		    if(f1int <  tonumber(get_text(2, 22))) then
				f1int = f1int +1  --需要限制总页
			end
		end	

		set_text(2, 21, string.format("%d", f1int))
		--set_text(2, 22, string.format("%d", f2int))

  		local f1hex = string_to_hex(get_text(2, 21))
  		local f2hex = string_to_hex(get_text(2, 22))

		sendArray[4] = 2
		sendArray[5]  =  f1hex[1]
		sendArray[6]  =  f2hex[1]

		add_end(datalen, sendArray)
		local sendData = shift_left(sendArray)
		uart_send_data(sendData)
	end


	if (screen == 2 and control == 32) then  -- 注册按钮按下 返回id+name  都是字符串表示的
		print(string.format("method register"))
		local searchId = get_text(screen, 8)  -- 表示hex 的string
		local SetIdName = get_text(screen, 9)  
		
		local idLen = #searchId +1 
		local nameLen = #SetIdName +1 
	    datalen = idLen + nameLen
		
		sendArray[3] = g_registerButton
		local nameStr = string_to_hex(SetIdName)
		local searchIdStr = string_to_hex(searchId)
		
		-- 连接 searchIdStr 到 sendArray 从第五个位置开始
 		table.insert(searchIdStr, '\0')-- 字符串追加一位结束符	
		for i = 1, idLen do
		    sendArray[4 + i] = searchIdStr[i]
		end
		
		-- 连接 nameStr 到 sendArray 从第11个位置开始
 		table.insert(nameStr, '\0')-- 字符串追加一位结束符	
		for i = 1, nameLen do
		    sendArray[idLen + 4 + i] = nameStr[i]
			-- print(string.format("len1 %d", idLen + 4 + i))
		end   

		sendArray[4] = datalen 
		add_end(datalen, sendArray)
		local sendData = shift_left(sendArray)
		uart_send_data(sendData)
	end

	if (screen == 2 and control == 53) then  -- 删除节点 
		print("deleteNode ") 	
		sendArray[3] = g_deleteNode
		-- 使用一个表来存储值
		local values = {}
		for i = 41, 46 do
		    values[i] = get_value(2, i)
		end
		 
		local j = 0  
		for i = 41, 46 do
		    if values[i] == 1 then
		        j = i
		        break
		    end
		end
 
		local psw =  get_text(2, 7) 
		local pswlen = # (psw) +1
		local hexPsw = string_to_hex(psw)

		datalen = pswlen + 2
		sendArray[4] = datalen
		sendArray[5]  =  j
 		sendArray[6]  = tonumber(get_text(2, 21))	

		for i = 1, pswlen do
			sendArray[6 + i] = hexPsw[i] 
		end   	

		add_end(datalen, sendArray)
		local sendData = shift_left(sendArray)
		uart_send_data(sendData)  
	end
end

--  系统通知事件
function on_control_notify(screen, control, value)

		if((control ~= 8) and (control ~= 9) and (control ~= 30)) then -- 提前过滤不需要的事件 
			g_screen = screen
			g_control = control
			g_value = value 
			start_timer(timer_notify, 200, 0, 1)	  -- 再由定时器触发 因为按钮必须要有数据才触发通知 但是自己还要发送数据 数据间隔太短	
		end
 		
end