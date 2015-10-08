function SetUserData(imageData,colors,channelData)
global D3dUICtrlHandles

if (~isempty(D3dUICtrlHandles))
    data = struct('imageData',imageData,'colors',colors,'channelData',channelData);
    set(D3dUICtrlHandles.hObject,'UserData',data);
else
    error('There is no UserData stored in the control window!');
end
end

