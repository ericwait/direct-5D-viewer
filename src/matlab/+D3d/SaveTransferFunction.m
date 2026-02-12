function SaveTransferFunction(pathName,fileName)
%SaveTransferFunction Saves the current transfer function (color/alpha) to JSON.
%   D3d.SaveTransferFunction(pathName, fileName)
%
%   Inputs:
%       pathName:   (Optional) Directory to save to.
%       fileName:   (Optional) Name of the JSON file.
    [imageData, ~, channelData] = D3d.UI.Ctrl.GetUserData();

    if (~exist('fileName','var') || isempty(fileName))
        fileName = [imageData.DatasetName,'_transfer','.json'];
    end
    
    if (~exist('pathName','var'))
        pathName = imageData.imageDir;
        [fileName,pathName,filterIndex] = uiputfile('.json','Save Transfer Function To...',fullfile(pathName,fileName));
        if (filterIndex==0)
            return
        end
    end

    [~,fName,ext] = fileparts(fileName);
    if (isempty(ext) || ~strcmp(ext,'.json'))
        ext = '.json';
    end

    tranData.chan1 = channelData(1);

    for c=2:imageData.NumberOfChannels
        tranData = setfield(tranData,sprintf('chan%d',c),channelData(c));
    end

    json = Utils.CreateJSON(tranData);

    f = fopen(fullfile(pathName,[fName,ext]),'wt');
    fprintf(f,json);
    fclose(f);
end
