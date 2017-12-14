function ResetTransferFunctions()
        [imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();
        
        for c=1:length(channelData)
            channelData(c).visible = 1;
            channelData(c).alphaMod = 1.0;
            channelData(c).minVal = 0.0;
            channelData(c).midVal = 0.5;
            channelData(c).maxVal = 1.0;
            channelData(c).a = 0.0;
            channelData(c).b = 1.0;
            channelData(c).c = 0.0;
        end
        
        D3d.UI.Ctrl.SetUserData(imageData,colors,channelData);
        D3d.UI.Ctrl.UpdateCurrentState();
end