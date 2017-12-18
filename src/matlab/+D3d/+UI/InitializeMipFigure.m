function InitializeMipFigure(im, imD, metadataPath, prompt)
%InitializeMipFigure( metadataPath, prompt)

global MipFigureHandle MipAxesHandle MipPrevRectangleHandle MipDragRectangleHandle MipTextHandle MipDragLineHandle MipFullIm
if (~exist('prompt','var'))
    prompt = [];
end

if (~exist('imD','var') || isempty(imD))
    if (~exist('metadataPath','var') || isempty(metadataPat))
        error('No image or metadata to read!');
    end
    imD = MicroscopeData.ReadMetadata(metadataPath,prompt);
end

[~, imInfo] = MicroscopeData.GetImageClass(imD);
mem = memory;
memNeeded = imInfo(1).BitDepth * prod(imD.Dimensions) * imD.NumberOfChannels * imD.NumberOfFrames;

if (~exist('im','var') || isempty(im))
    if (mem.MemAvailableAllArrays>memNeeded)
        MipFullIm = MicroscopeData.Reader(imD,[],[],[],[],[],true);
        imMIP = max(MipFullIm,[],3);
    else
        MipFullIm = [];
        imMIP = MicroscopeData.Reader('imageData',imD,'getMIP',true);
    end
else
    MipFullIm = im;
    imMIP = max(MipFullIm,[],3);
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
MipTextHandle = text(0,0,'','Color',[0.71,0.71,0.35],'HorizontalAlignment','center','VerticalAlignment','baseline','Parent',MipAxesHandle);  
MipDragLineHandle = plot(0,0,'Color',[1.0,0.13,0.11],'LineStyle','-','Parent',MipAxesHandle);

set(MipPrevRectangleHandle,'Visible','off');
set(MipDragRectangleHandle,'Visible','off');
set(MipTextHandle,'Visible','off');
set(MipDragLineHandle,'Visible','off');

set(MipFigureHandle,'WindowButtonDownFcn',@D3d.UI.MipButtonDown,...
    'WindowButtonMotionFcn',@D3d.UI.MipButtonMotion,...
    'WindowButtonUpFcn',@D3d.UI.MipButtonUp);
end
