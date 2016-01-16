function InitializeMipFigure( metadataPath, prompt)
%InitializeMipFigure( metadataPath, prompt)

global MipFigureHandle MipAxesHandle MipPrevRectangleHandle MipDragRectangleHandle MipAnnotationLeftHandle MipAnnotationRightHandle MipDragLineHangle MipFullIm
if (~exist('prompt','var'))
    prompt = [];
end
imD = MicroscopeData.ReadMetadata(metadataPath,prompt);

[clss, imInfo] = MicroscopeData.GetImageClass(imD);
mem = memory;
memNeeded = imInfo(1).BitDepth * prod(imD.Dimensions) * imD.NumberOfChannels * imD.NumberOfFrames;

if (mem.MemAvailableAllArrays>memNeeded)
    MipFullIm = MicroscopeData.Reader(imD,[],[],[],[],[],true);
    imMIP = max(MipFullIm,[],3);
else
    MipFullIm = [];
    imMIP = MicroscopeData.ReaderMIP(imD,[],[],[],[],[],true);
end

colors = MicroscopeData.Colors.GetChannelColors(imD);
imC = ImUtils.ThreeD.ColorMIP(imMIP,colors);

MipFigureHandle = figure;
iH = imshow(imC);
MipAxesHandle = get(iH,'Parent');
hold(MipAxesHandle,'on');

ud.ImData = imD;
ud.IsDown = false;
set(MipAxesHandle,'UserData',ud);

MipPrevRectangleHandle = rectangle('Position',[0,0,0.1,0.1],'EdgeColor',[0.88,0.88,0.3],'LineStyle','-','LineWidth',1.0,'Parent',MipAxesHandle);
MipDragRectangleHandle = rectangle('Position',[0,0,0.1,0.1],'EdgeColor',[1/3,0.61,0.84],'LineStyle','--','LineWidth',1.0,'Parent',MipAxesHandle);
MipDragLineHangle = plot(0,0,'Color',[1.0,0.13,0.11],'LineStyle','-','Parent',MipAxesHandle);

MipAnnotationLeftHandle = annotation('textarrow',[0.1,0.1],[0,0]);
MipAnnotationRightHandle = annotation('textarrow',[0,0],[0.1,0.1],'String','Width = 0');

set(MipPrevRectangleHandle,'Visible','off');
set(MipDragRectangleHandle,'Visible','off');
set(MipAnnotationLeftHandle,'Visible','off');
set(MipAnnotationRightHandle,'Visible','off');
set(MipDragLineHangle,'Visible','off');

set(MipFigureHandle,'WindowButtonDownFcn',@D3d.UI.MipButtonDown,...
    'WindowButtonMotionFcn',@D3d.UI.MipButtonMotion,...
    'WindowButtonUpFcn',@D3d.UI.MipButtonUp);
end
