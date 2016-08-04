function [imageData, colors, channelData] = GetUserData()
global D3dUICtrlHandles

if (~isempty(D3dUICtrlHandles))
    data = get(D3dUICtrlHandles.hObject,'UserData');
    
    imageData = data.imageData;
    colors = data.colors;
    channelData = data.channelData;
    for c=1:length(channelData)
        channelData(c).color = colors(c,:);
    end
else
    error('There is no UserData stored in the control window!');
end
end
