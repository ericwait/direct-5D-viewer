function FigureTreeDown(src,evnt)
global uiTreeFig
set(uiTreeFig, 'WindowButtonMotionFcn',@MoveLine);   
MoveLine();
end
