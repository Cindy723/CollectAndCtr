------------------------------------
-- ��ʾ
-- ���ʹ������Lua�༭���߱༭���ĵ����뽫VisualTFT����д򿪵Ĵ��ļ��༭����ͼ�رգ�
-- ��ΪVisualTFT�����Զ����湦�ܣ���������޸�ʱ����ͬ����VisualTFT�༭��ͼ��
-- VisualTFT��ʱ����ʱ����������޸ĵ����ݽ����ָ���
--
-- Attention
-- If you use other Lua Editor to edit this file, please close the file editor view 
-- opened in the VisualTFT, Because VisualTFT has an automatic save function, 
-- other Lua Editor cannot be synchronized to the VisualTFT edit view when it is modified.
-- When VisualTFT saves regularly, the content modified by other Lua Editor will be restored.
------------------------------------

--�����г��˳��õĻص�����
--���๦�����Ķ�<<LUA�ű�API.pdf>>


--��ʼ������
--function on_init()
--end

--��ʱ�ص�������ϵͳÿ��1�����Զ����á�
--function on_systick()
--end

--��ʱ����ʱ�ص������������õĶ�ʱ����ʱʱ��ִ�д˻ص�������timer_idΪ��Ӧ�Ķ�ʱ��ID
--function on_timer(timer_id)
--end

--�û�ͨ�������޸Ŀؼ���ִ�д˻ص�������
--�����ť�ؼ����޸��ı��ؼ����޸Ļ��������ᴥ�����¼���
--function on_control_notify(screen,control,value)
--end

--�������л�ʱ��ִ�д˻ص�������screenΪĿ�껭�档
--function on_screen_change(screen)
--end


local g_screen
local g_control
local g_value

-- ee cd cx �Զ���ָ���Ӧ����
local g_registerButton = 0xc1 -- ע�ᰴť����ָ��
local g_pswEnter = 0xc2		   -- �����������
local g_nodePageChange = 0xc3   -- ���ý���ڵ�ҳ���л� 
local g_dispPageChange = 0xc4   -- ��ʾ����ҳ���л� 
local g_deleteNode = 0xc5 -- ɾ���ڵ�

local sendArray = {0xee, 0xcd} -- �Զ����� ֡ͷ �û����� �������� ���ݳ��� ���� 
  
-- lua ������±�1��ʼ
function on_init()    
	set_text(2, 7, "") 	
	set_value(2, 41, 1)
	--change_screen(2) -- �������е�����2 
end

-- �ַ���תʮ������
function string_to_hex(str)
    local hex = {}
    for i = 1, # (str) do
        local byte = str:byte(i) 
        table.insert(hex, byte)
    end 
   return hex
end

-- �������� ��Ϊ����ʼ����0 ȥ���ʼ��0  
function shift_left(da)
	local tempdata = {0x01}  
	for i = 0, # (da) do  -- ��bug ���������0 
	tempdata[i] = da[i+1]
	end
	return tempdata
end 

 
-- ���֡Ϊ
function add_end(datalen ,arr)
	arr[datalen+5] = 0xff
	arr[datalen+6] = 0xfc
	arr[datalen+7] = 0xff
	arr[datalen+8] = 0xff
end -- error attempt to get length of a nil value ����

-- ��ʱ��id
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


-- �û�ͨ�������޸Ŀؼ���ִ�д˻ص�������
-- �����ť�ؼ����޸��ı��ؼ����޸Ļ��������ᴥ�����¼� 
-- �ؼ�����Ҫ�д������ݲŻᴥ���ص�
function on_control_notify_my(screen, control, value)
	local datalen = 0

	print(string.format("on_control_notify %d", control))


	if (screen == 1 and control == 1) then  -- ��������
		set_text(2, 7, "") -- ����ϴ����������
	end


 	if (screen == 2 and control == 7) then  -- ��������
	-- �漴֮����������ڵ� �ڵ�Ƭ���ж���Ԥ�������Ƿ�һ�� ��һ��ʱ��Ƭ�������ʾ�ı� �Ҳ�������
	end	


	if (screen == 1 and (control == 11)or(control == 12)) then-- ��ʾ�б��л�����ҳ
		  --ͬ (control == 16)or(control == 17)
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
				f1int = f1int +1  --��Ҫ������ҳ
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
		start_timer(timer_send_dispageChange, 100, 0, 2) -- �ٷ�2�α仯������� ��ֹ��Ƭ��©��
	end


	if (screen == 2 and control == 31) then  -- �c������
		 --change_child_screen (3)

	end


	if (screen == 2 and (control == 16)or(control == 17)) then  -- �ڵ��б��л�����ҳ
		 --ҳ�� f1 / f2  �л�ҳ�� �·��仯����� ͬʱ�ϱ���Ӧҳ������չʾ 
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
				f1int = f1int +1  --��Ҫ������ҳ
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


	if (screen == 2 and control == 32) then  -- ע�ᰴť���� ����id+name  �����ַ�����ʾ��
		print(string.format("method register"))
		local searchId = get_text(screen, 8)  -- ��ʾhex ��string
		local SetIdName = get_text(screen, 9)  
		
		local idLen = #searchId +1 
		local nameLen = #SetIdName +1 
	    datalen = idLen + nameLen
		
		sendArray[3] = g_registerButton
		local nameStr = string_to_hex(SetIdName)
		local searchIdStr = string_to_hex(searchId)
		
		-- ���� searchIdStr �� sendArray �ӵ����λ�ÿ�ʼ
 		table.insert(searchIdStr, '\0')-- �ַ���׷��һλ������	
		for i = 1, idLen do
		    sendArray[4 + i] = searchIdStr[i]
		end
		
		-- ���� nameStr �� sendArray �ӵ�11��λ�ÿ�ʼ
 		table.insert(nameStr, '\0')-- �ַ���׷��һλ������	
		for i = 1, nameLen do
		    sendArray[idLen + 4 + i] = nameStr[i]
			-- print(string.format("len1 %d", idLen + 4 + i))
		end   

		sendArray[4] = datalen 
		add_end(datalen, sendArray)
		local sendData = shift_left(sendArray)
		uart_send_data(sendData)
	end

	if (screen == 2 and control == 53) then  -- ɾ���ڵ� 
		print("deleteNode ") 	
		sendArray[3] = g_deleteNode
		-- ʹ��һ�������洢ֵ
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

--  ϵͳ֪ͨ�¼�
function on_control_notify(screen, control, value)

		if((control ~= 8) and (control ~= 9) and (control ~= 30)) then -- ��ǰ���˲���Ҫ���¼� 
			g_screen = screen
			g_control = control
			g_value = value 
			start_timer(timer_notify, 200, 0, 1)	  -- ���ɶ�ʱ������ ��Ϊ��ť����Ҫ�����ݲŴ���֪ͨ �����Լ���Ҫ�������� ���ݼ��̫��	
		end
 		
end