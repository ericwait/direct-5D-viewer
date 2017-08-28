function ToggleChannel(chan,visible)
    [imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();

    channelData(chan).visible = visible;

    D3d.UI.Ctrl.SetUserData(imageData,colors,channelData);
    D3d.UI.Ctrl.PlotTransferFunctions();
end
