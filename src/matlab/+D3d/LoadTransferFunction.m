function LoadTransferFunction(path)
    [imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();
    
    if (~exist('path','var'))
        path = [];
    end
    
    if (~isempty(path))
        [~,~,ext] = fileparts(path);
        if (isempty(ext) || ~strcmp(ext,'.json'))
            path = [];
        end
    end
        
    while(isempty(path))
        [fileName,pathName,filterIndex] = uigetfile('.json','Load Transfer Function...',fullfile(imageData.imageDir,[imageData.DatasetName,'_transfer','.json']));
        
        if (filterIndex==0)
            return
        end
        
        [~,~,ext] = fileparts(fileName);
        if (isempty(ext) || ~strcmp(ext,'.json'))
            choice = questdlg('Select a valid json file?','Continue?','Yes','Cancel','Yes');
            if (strcmp(choice,'Cancel'))
                return
            end
        end
        
        path = fullfile(pathName,fileName);
    end
    
    f = fopen(path,'rt');
    json = fread(f,'*char').';
    fclose(f);
    json = Utils.ParseJSON(json);
    
    for c=1:imageData.NumberOfChannels
        channelData(c) = getfield(json,sprintf('chan%d',c));
        colors(c,:) = channelData(c).color;
    end
    
    D3d.UI.Ctrl.SetUserData(imageData,colors,channelData);
    D3d.UI.Ctrl.UpdateCurrentState();
end
