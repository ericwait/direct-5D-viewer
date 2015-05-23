% --- Executes on button press in save.
function SaveUiData()
global imageData channelData

if (~isempty(imageData) && ~isempty(channelData))
    save([imageData.DatasetName '_Settings.mat'],'channelData');
end
end