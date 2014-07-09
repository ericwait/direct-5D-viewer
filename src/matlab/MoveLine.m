function MoveLine(src,evnt)
global uiTreeAx useDistance

time = get(uiTreeAx,'CurrentPoint');
if (useDistance>0)
    time = round(time(1,1));
else
    time = round(time(1,2));
end

UpdateTime(time);
end
