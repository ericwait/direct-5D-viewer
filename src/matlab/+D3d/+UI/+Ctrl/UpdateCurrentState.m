function UpdateCurrentState()
global D3dUICtrlHandles

if (~isempty(D3dUICtrlHandles))
    [~, colors, channelData] = D3d.UI.Ctrl.GetUserData();
    
    chan = get(D3dUICtrlHandles.handles.m_channelPicker,'Value');   
    set(D3dUICtrlHandles.handles.s_alpha,'Value',channelData(chan).alphaMod,'BackgroundColor',[channelData(chan).alphaMod/2,channelData(chan).alphaMod/2,channelData(chan).alphaMod/2]);
    set(D3dUICtrlHandles.handles.s_floor,'Value',channelData(chan).minVal,'BackgroundColor',colors(chan,:) .* 0.35);
    set(D3dUICtrlHandles.handles.s_mids,'Value',channelData(chan).midVal,'BackgroundColor',colors(chan,:) .* 0.75);
    set(D3dUICtrlHandles.handles.s_ceil,'Value',channelData(chan).maxVal,'BackgroundColor',colors(chan,:));
    set(D3dUICtrlHandles.handles.pb_colorChooser,'BackgroundColor',colors(chan,:));
    set(D3dUICtrlHandles.handles.cb_visible,'Value',channelData(chan).visible);

    D3d.UI.Ctrl.PlotTransferFunctions();
end
end