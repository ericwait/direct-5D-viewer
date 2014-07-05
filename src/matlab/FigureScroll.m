function FigureScroll(src,evnt)
global uiFrameTb
time = str2double(get(uiFrameTb,'String'))-1;

time = time + evnt.VerticalScrollCount;
UpdateTime(time);
end
