function MoveLine(src,evnt)
global uiTreeAx

time = get(uiTreeAx,'CurrentPoint');
time = round(time(3));

UpdateTime(time);
end
