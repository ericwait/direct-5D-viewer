function DrawTree(dim)
global Hulls Tracks Families selectedHull uiTreeFig uiTreeAx uiFrameTb imageData

if (isempty(selectedHull) || selectedHull==-1)
    familyID = FindLargestTree();
else
    familyID = Tracks(Hulls(selectedHull).track).family;
end

if(isempty(Hulls) || isempty(Tracks) || isempty(Families) || isempty(Families(familyID).tracks)),return,end

[~,id] = min(Families(familyID).tracks);
trackID = Families(familyID).tracks(id);

if isempty(uiTreeFig)
    uiTreeFig = figure();
    set(uiTreeFig,...
        'MenuBar','none',...
        'NumberTitle','off',...
        'Name','LEVer 3D Lineage Tree',...
        'WindowButtonDownFcn',@FigureTreeDown,...
        'WindowButtonUpFcn',@FigureTreeUp,...
        'WindowScrollWheelFcn',@FigureScroll,...
        'CloseRequestFcn',@TreeClose);
end

clf(uiTreeFig)
uiTreeAx = axes('Parent',uiTreeFig);

if dim==2
    set(uiTreeAx,...
        'YDir',     'reverse',...
        'XColor',   [0.35 0.35 0.35],...
        'YColor',   'w',...
        'XTick',    [],...
        'Box',      'off');
else
    set(uiTreeAx,...
        'XLim',     [0 imageData.NumberOfFrames],...
        'Box',      'off');
end
% ylabel('Time (Frames)');

hold(uiTreeAx,'on');
[xMin, ~, xMax] = traverseTree(trackID,0,dim);

xticks = unique(round((0:floor(imageData.NumberOfFrames/3))*3));
xtickLbl = cell(floor(imageData.NumberOfFrames/3),1);
for i=0:floor(imageData.NumberOfFrames/3)
    xtickLbl{i+1} = num2str(i);
end

if dim==2
    set(uiTreeAx,...
        'YLim',     [0 imageData.NumberOfFrames],...
        'XLim',     [xMin-1 xMax+1],...
        'YTick',    xticks,...
        'YTickLabel',   xtickLbl,...
        'Color','w');
    ylabel(uiTreeAx,'Time (Hours)','Color','w');
else
    set(uiTreeAx,...
        'XLim',     [0 imageData.NumberOfFrames],...
        'XTick',    xticks,...
        'XTickLabel',   xtickLbl,...
        'Color', 'w');
    xlabel(uiTreeAx,'Time (Hours)','Color','w');
    ylabel(uiTreeAx,'Distance to Vasculature (voxels)');
end

if dim==3
    set(uiTreeAx,...
        'CameraUpVector', [0 0 1],...
        'CameraUpVectorMode', 'manual');
end

set(uiTreeFig,'color',[0.35 0.35 0.35]);
set(uiTreeAx,'color',[0.35 0.35 0.35]);
t = str2double(get(uiFrameTb,'String'));
UpdateTime(t-1);

hold(uiTreeAx,'off');

end

function [xMin, xCenter, xMax, initDist] = traverseTree(trackID,initXmin,dim)
global Tracks

if(~isempty(Tracks(trackID).childrenTracks))
    [child1Xmin, child1Xcenter, child1Xmax, childDist1] = traverseTree(Tracks(trackID).childrenTracks(1),initXmin,dim);
    [child2Xmin, child2Xcenter, child2Xmax, childDist2] = traverseTree(Tracks(trackID).childrenTracks(2),child1Xmax+1,dim);
    xMin = min(child1Xmin,child2Xmin);
    xMax = max(child1Xmax,child2Xmax);
    if(child1Xcenter < child2Xcenter)
        drawHorizontalEdge(child1Xcenter,child2Xcenter,Tracks(trackID).endFrame,childDist1,childDist2,dim);
        xCenter = (child2Xcenter-child1Xcenter)/2 + child1Xcenter;
    else
        drawHorizontalEdge(child2Xcenter,child1Xcenter,Tracks(trackID).endFrame,trackID,dim);
        xCenter = (child1Xcenter-child2Xcenter)/2 + child2Xcenter;
    end
    initDist = (childDist1+childDist2)/2;
    drawVerticalEdge(trackID,xCenter,dim,initDist);
else
    %This is when the edge is for a leaf node
    initDist = drawVerticalEdge(trackID,initXmin,dim);
    xMin = initXmin;
    xCenter = initXmin;
    xMax = initXmin;
end
end

function drawHorizontalEdge(xMin,xMax,y,z1,z2,dim)
global uiTreeAx
if dim==3
%     plot3([xMin xMax],[y y],[z1 z2],'-k','LineWidth',1.5);
plot(uiTreeAx,[y y],[z1 z2],'-w','LineWidth',1.5);
elseif dim==2
    plot(uiTreeAx,[xMin xMax],[y y],'-w','LineWidth',1.5);
end
%Place the line behind all other elements already graphed
h = get(uiTreeAx,'child');
h = h([2:end, 1]);
set(uiTreeAx, 'child', h);
end

function initDist = drawVerticalEdge(trackID,xVal,dim,endDist)
global Tracks Hulls uiTreeAx

initDist = 0;

%draw circle for node
FontSize = 14;
% switch length(num2str(trackID))
%     case 1
%         circleSize=10;
%     case 2
%         circleSize=12;
%     case 3
%         circleSize=14;
%     otherwise
%         circleSize=16;
%         FontSize = 7;
% end
circleSize=32;

yMin = Tracks(trackID).startFrame-1;

FaceColor = Tracks(trackID).color;%.background;
EdgeColor = Tracks(trackID).color;%.background;
TextColor = 'k';

%draw vertical line to represent edge length
if dim==3
    distances = [Hulls(Tracks(trackID).hulls).distanceToVasc];
    hflt = fspecial('gaussian',[1,5], 2);
    if ~exist('endDist','var') && ~isempty(distances)
        endDist = distances(end);
    elseif ~exist('endDist','var')
        endDist = 1;
        distances(1:length(Tracks(trackID).hulls)) = 1;
    elseif isempty(distances)
        distances(1:length(Tracks(trackID).hulls)) = endDist;
    end
        
    paddist = [distances(1)*ones(1,round(length(hflt)/2)) distances distances(end)*ones(1,round(length(hflt)/2))];
    distances = conv(paddist,hflt);
    distances = distances((round(length(hflt)/2)+1):end);
    for i=1:length(Tracks(trackID).hulls)-1
%         plot3([xVal xVal],...
%             [CellHulls(CellTracks(trackID).hulls(i)).frame-1 CellHulls(CellTracks(trackID).hulls(i+1)).frame-1],...
%             [CellHulls(CellTracks(trackID).hulls(i)).distanceToVasc CellHulls(CellTracks(trackID).hulls(i+1)).distanceToVasc],...
%             '-','color',FaceColor,'LineWidth',2.0);
        plot(uiTreeAx,[Hulls(Tracks(trackID).hulls(i)).frame-1 Hulls(Tracks(trackID).hulls(i+1)).frame-1],[distances(i) distances(i+1)],...
            '-','color',FaceColor,'LineWidth',2.0);
        childhandles = get(uiTreeAx, 'children');
        childhandles = childhandles([2:end 1]);
        set(uiTreeAx, 'children',childhandles);
%         plot3([xVal xVal],...
%             [CellHulls(CellTracks(trackID).hulls(i)).frame-1 CellHulls(CellTracks(trackID).hulls(i+1)).frame-1],...
%             [CellHulls(CellTracks(trackID).hulls(i)).distanceToVascCOM CellHulls(CellTracks(trackID).hulls(i+1)).distanceToVascCOM],...
%             ':','color',FaceColor,'LineWidth',1.0);
    end
%     plot3([xVal xVal],...
%         [CellHulls(CellTracks(trackID).hulls(i+1)).frame-1 CellTracks(trackID).endFrame],...
%         [CellHulls(CellTracks(trackID).hulls(i+1)).distanceToVasc CellHulls(CellTracks(trackID).hulls(i+1)).distanceToVasc],...
%         '-','color',FaceColor,'LineWidth',2.0);

    plot(uiTreeAx,[Hulls(Tracks(trackID).hulls(i+1)).frame-1 Tracks(trackID).endFrame],[distances(i+1) endDist],'-','color',FaceColor,'LineWidth',2.0);
    
    childhandles = get(uiTreeAx, 'children');
    childhandles = childhandles([2:end 1]);
    set(uiTreeAx, 'children',childhandles);
%     plot3([xVal xVal],...
%         [CellHulls(CellTracks(trackID).hulls(i+1)).frame-1 CellTracks(trackID).endFrame],...
%         [CellHulls(CellTracks(trackID).hulls(i+1)).distanceToVascCOM CellHulls(CellTracks(trackID).hulls(i+1)).distanceToVascCOM],...
%         ':','color',FaceColor,'LineWidth',1.0);

elseif dim==2
    plot(uiTreeAx,[xVal xVal], [Tracks(trackID).startFrame-1 Tracks(trackID).endFrame],'-w','LineWidth',1.5);
end
% plot([xVal xVal],[yMin CellTracks(trackID).endFrame],...
%     '-k','UserData',trackID,'LineWidth',1.5);

if dim==3
    plot(uiTreeAx,yMin,distances(1),...
        'o',...
        'MarkerFaceColor',  FaceColor,...
        'MarkerEdgeColor',  EdgeColor,...
        'MarkerSize',       circleSize,...
        'UserData',         trackID);
    
    text(yMin,distances(1),...
        num2str(trackID-1),...
        'Parent',uiTreeAx,...
        'HorizontalAlignment',  'center',...
        'FontSize',             FontSize,...
        'color',                TextColor,...
        'UserData',             trackID);
    
    initDist = distances(1);
elseif dim==2
    plot(uiTreeAx,xVal,yMin,...
        'o',...
        'MarkerFaceColor',  FaceColor,...
        'MarkerEdgeColor',  EdgeColor,...
        'MarkerSize',       circleSize,...
        'UserData',         trackID);
    
    text(xVal,yMin,...
        num2str(trackID-1),...
        'Parent',uiTreeAx,...
        'HorizontalAlignment',  'center',...
        'FontSize',             FontSize,...
        'color',                TextColor,...
        'UserData',             trackID);
end
end


% 
% % imTemp = mean(imSmooth(:,:,:,4:50,2),4);
% % imTemp = imTemp./max(imTemp(:));
% % thresh = graythresh(imTemp(:));
% 
% dists = zeros(50,length(0.1:0.1:2.0),2);
% 
% track73Hulls = find([Hulls(:).track]==73);
% track371Hulls = find([Hulls(:).track]==371);
% track578Hulls = find([Hulls(:).track]==578);
% 
% j=1;
% alpha = 0.8;
% % for alpha=0.1:0.1:2.0
%     
%     for t=1:50
%         imTemp = double(imSmooth(:,:,:,t,2));
%         imTempD = imTemp./255;
%         thresh = graythresh(imTempD(:));
%         if (thresh*alpha>1)
%             break
%         end
%         imBW = im2bw(imTempD(:),thresh*alpha);
%         imBW = reshape(imBW,size(imTempD));
%         imDist(:,:,:,t) = bwdist(imBW);
%     end
%     
%     for i=1:length(track73Hulls)
%         coord = round(Hulls(track73Hulls(i)).centerOfMass);
%         t = Hulls(track73Hulls(i)).frame;
%         dists(t,j,1) = imDist(coord(2),coord(1),coord(3),t);
%     end
%     
%     for i=1:length(track371Hulls)
%         coord = round(Hulls(track371Hulls(i)).centerOfMass);
%         t = Hulls(track371Hulls(i)).frame;
%         dists(t,j,2) = imDist(coord(2),coord(1),coord(3),t);
%     end
%     
%     for i=1:length(track578Hulls)
%         coord = round(Hulls(track578Hulls(i)).centerOfMass);
%         t = Hulls(track578Hulls(i)).frame;
%         dists(t,j,1) = imDist(coord(2),coord(1),coord(3),t);
%     end
%     
%     j = j+1;
% %end
% 
% 
% % for i=1:j
% %     figure
% %     hold on
% %     plot(1:50,dists(:,i,1),'-b');
% %     plot(30:50,dists(30:50,i,2),'-g');
% % end
% 
% newDistParent = dists(4:29,1);%0,1);
% newDistParent = conv(newDistParent,ones(1,4)/4,'valid');
% newDist371 = dists(30:50,1);%0,2);
% newDist371 = conv(newDist371,ones(1,4)/4,'valid');
% newDist578 = dists(30:50,1);%0,1);
% newDist578 = conv(newDist578,ones(1,4)/4,'valid');
% 
% splitFrame = 24;
% 
% figure
% plot([newDistParent(end) newDist371(1)],[splitFrame-1 splitFrame],'-','color',Hulls(track371Hulls(1)).color,'linewidth',3);
% hold on
% plot([newDistParent(end) newDist578(1)],[splitFrame-1 splitFrame],'-','color',Hulls(track578Hulls(1)).color,'linewidth',3);
% plot(newDistParent,1:splitFrame-1,'-','color',Hulls(track73Hulls(1)).color,'linewidth',3);
% plot(newDist371,24:(splitFrame+length(newDist371)-1),'-','color',Hulls(track371Hulls(1)).color,'linewidth',3);
% plot(newDist578,24:(splitFrame+length(newDist578)-1),'-','color',Hulls(track578Hulls(1)).color,'linewidth',3);
% 
% plot(newDistParent(1),1,'.','color',Hulls(track73Hulls(1)).color,'markersize',64);
% plot(newDist371(1),splitFrame,'.','color',Hulls(track371Hulls(1)).color,'markersize',64);
% plot(newDist578(1),splitFrame,'.','color',Hulls(track578Hulls(1)).color,'markersize',64);
% 
% text(newDistParent(1),1,'73',...
%     'fontweight','bold','horizontalalignment','center','verticalalignment','middle');
% text(newDist371(1),splitFrame,'371',...
%     'fontweight','bold','horizontalalignment','center','verticalalignment','middle');
% text(newDist578(1),splitFrame,'578',...
%     'fontweight','bold','horizontalalignment','center','verticalalignment','middle');
% 
% axis ij
% xlim([min([newDistParent;newDist371;newDist578])-0.5 max([newDistParent;newDist371;newDist578])+0.5]);
% ylim([0 splitFrame+length(newDist578)-2])
% set(gca,'color',[0.35 0.35 0.35],'XColor',[1 1 1],'YColor',[1 1 1]);
% 
% set(gca,...
%     'YTick',    linspace(0,splitFrame+length(newDist578)-1,14),...
%     'YTickLabel',   {'0','1','2','3','4','5','6','7','8','9','10','11','12','13','14'},...
%     'Box',      'off');
% ylabel(gca,'frame (Hours)','color','w');
% xlabel(gca,'Distance to Vasculature (voxels)','color','w');
% set(gcf,'color',[0.35 0.35 0.35]);
% end
% 
% 
% end

