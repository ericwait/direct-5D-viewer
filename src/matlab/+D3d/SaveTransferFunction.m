function SaveTransferFunction()
    [imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();

    [fileName,pathName,filterIndex] = uiputfile('.json','Save Transfer Function To...',[imageData.DatasetName,'_transfer','.json']);

    if (filterIndex==0)
        return
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