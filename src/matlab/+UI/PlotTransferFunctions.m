function PlotTransferFunctions(handles)
global channelData

cla(handles.funcPlot);
hold on

for i=1:length(channelData)
    [minRange, maxRange] = UI.CalcMidRange(channelData(i).minVal,channelData(i).maxVal);
    
    [x, y, channelData(i).a, channelData(i).b, channelData(i).c]...
        = UI.CalcCurve(channelData(i).visible, channelData(i).minVal,...
        channelData(i).midVal*(maxRange-minRange)+minRange,...
        channelData(i).maxVal);
    
    if (channelData(i).visible>0)
        plot(handles.funcPlot,x,y,'-','Color',channelData(i).color,'LineWidth',2.0);
    else
        plot(handles.funcPlot,x,y,':','Color',channelData(i).color,'LineWidth',2.0);
    end
end
xlim(handles.funcPlot,[0 1]);
ylim(handles.funcPlot,[0 1]);

hold off
UI.SaveUiData();
if (1==get(handles.rb_Processed,'Value'))
    lever_3d('transferUpdate',channelData,'processed');
else
    lever_3d('transferUpdate',channelData,'original');
end
end