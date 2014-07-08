function FigureScroll(src,evnt)
global uiControlHandles
time = str2double(get(uiControlHandles.tb_curFrame,'String'))-1;

time = time + evnt.VerticalScrollCount;
UpdateTime(time);
end
