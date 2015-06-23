function UpdateCurrentState(handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
set(handles.s_alpha,'Value',channelData(chan).alphaMod,'BackgroundColor',[channelData(chan).alphaMod/2,channelData(chan).alphaMod/2,channelData(chan).alphaMod/2]);
set(handles.s_floor,'Value',channelData(chan).minVal,'BackgroundColor',channelData(chan).color .* 0.35);
set(handles.s_mids,'Value',channelData(chan).midVal,'BackgroundColor',channelData(chan).color .* 0.75);
set(handles.s_ceil,'Value',channelData(chan).maxVal,'BackgroundColor',channelData(chan).color);
set(handles.pb_colorChooser,'BackgroundColor',channelData(chan).color);
set(handles.cb_visible,'Value',channelData(chan).visible);

UI.PlotTransferFunctions(handles)
end