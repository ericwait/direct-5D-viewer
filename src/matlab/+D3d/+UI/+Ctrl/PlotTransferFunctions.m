function PlotTransferFunctions()
global D3dUICtrlHandles

if (~isempty(D3dUICtrlHandles))
    
    [imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();
    
    cla(D3dUICtrlHandles.handles.funcPlot);
    hold(D3dUICtrlHandles.handles.funcPlot,'on');
    
    for i=1:length(channelData)
        [minRange, maxRange] = D3d.UI.Ctrl.CalcMidRange(channelData(i).minVal,channelData(i).maxVal);
        
        [x, y, channelData(i).a, channelData(i).b, channelData(i).c]...
            = D3d.UI.Ctrl.CalcCurve(channelData(i).visible, channelData(i).minVal,...
            channelData(i).midVal*(maxRange-minRange)+minRange,...
            channelData(i).maxVal);
        
        if (channelData(i).visible>0)
            plot(D3dUICtrlHandles.handles.funcPlot,x,y,'-','Color',colors(i,:),'LineWidth',2.0);
        else
            plot(D3dUICtrlHandles.handles.funcPlot,x,y,':','Color',colors(i,:),'LineWidth',2.0);
        end
    end
    xlim(D3dUICtrlHandles.handles.funcPlot,[0 1]);
    ylim(D3dUICtrlHandles.handles.funcPlot,[0 1]);
    
    hold(D3dUICtrlHandles.handles.funcPlot,'off')

    if (1==get(D3dUICtrlHandles.handles.rb_Processed,'Value'))
        D3d.Viewer.TransferFunction(channelData,'processed');
    else
        D3d.Viewer.TransferFunction(channelData,'original');
    end
    
    D3d.Update();
    
    D3d.UI.Ctrl.SetUserData(imageData,colors,channelData)
end
end