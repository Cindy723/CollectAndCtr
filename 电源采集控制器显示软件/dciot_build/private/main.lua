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

-- ����һ������ �����Ǵ�1��ʼ
-- 0xcd��ʾ�Լ���������ݣ�0x01��ʾ�������ý�����ע��ʱ���·��ı����͵�ַ����
-- �����ǳ���λ�� ��������   
function arrayIndex()
-- ����һ������һЩԪ�ص�����
	local array = {10, 20, 30, 40, 50}
	
	-- ��ӡ����ĵ�һ��Ԫ�غ͵����Ԫ�أ�������1��ʼ
	print("��һ��Ԫ��: " .. array[1])  -- ���: ��һ��Ԫ��: 10
	print("�ڶ���Ԫ��: " .. array[2])  -- ���: �ڶ���Ԫ��: 20
	print("������Ԫ��: " .. array[3])  -- ���: ������Ԫ��: 30
	print("���ĸ�Ԫ��: " .. array[4])  -- ���: ���ĸ�Ԫ��: 40
	print("�����Ԫ��: " .. array[5])  -- ���: �����Ԫ��: 50
	
	-- ��ӡ���鳤��
	print("���鳤��: " .. #array)  -- ���: ���鳤��: 5
	
	-- ���Է��ʵ�0��Ԫ�أ��᷵��nil����ΪLua����������1��ʼ
	print("�����Ԫ��: " .. tostring(array[0]))  -- ���: �����Ԫ��: nil
end


function on_init()   
	arrayIndex()
	change_screen(2) -- �������е�����2 
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
function on_timer(timer_id)
	
	if ( timer_id == timer_send_searchid_text)then

	end

	if ( timer_id == timer_notify) then
		on_control_notify_my(g_screen, g_control, g_value)
	end

end


-- �û�ͨ�������޸Ŀؼ���ִ�д˻ص�������
-- �����ť�ؼ����޸��ı��ؼ����޸Ļ��������ᴥ�����¼� 
-- �ؼ�����Ҫ�д������ݲŻᴥ���ص�
function on_control_notify_my(screen, control, value)
	local sendArray = {0xee, 0xcd, 0x01} -- �Զ����� ֡ͷ �û����� �������� ���ݳ��� ���� 
	print(string.format("on_control_notify %d", control))
	if (screen == 2 and control == 32) then  -- ע�ᰴť���� ����id+name  �����ַ�����ʾ��
		print(string.format("method register"))
		local searchId = get_text(screen, 8)  -- ��ʾhex ��string
		local SetIdName = get_text(screen, 9)  
		
		local idLen = #searchId +1 
		local nameLen = #SetIdName +1 
		local datalen = idLen + nameLen
		
		sendArray[3] = 0xc1
		sendArray[4] = datalen
		
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

		add_end(datalen, sendArray)
		local sendData = shift_left(sendArray)
		uart_send_data(sendData)
	 		--start_timer(timer_send_serchid_text, 100, 0, 1)
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