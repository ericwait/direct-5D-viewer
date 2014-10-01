%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
%This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
%tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
%for details. LEVER 3-D is free software: you can redistribute it and/or modify
%it under the terms of the GNU General Public License as published by the Free
%Software Foundation, either version 3 of the License, or (at your option) any
%later version.
%LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
%WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
%A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
%You should have received a copy of the GNU General Public License along with
%LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function DrawTree()
global Hulls Tracks Families selectedHull uiTreeFig uiTreeAx uiControlHandles imageData useDistance

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

if useDistance>0
    set(uiTreeAx,...
        'XLim',     [0 imageData.NumberOfFrames],...
        'Box',      'off');
else
    set(uiTreeAx,...
        'YDir',     'reverse',...
        'XColor',   [0.35 0.35 0.35],...
        'YColor',   'w',...
        'XTick',    [],...
        'Box',      'off');
end
% ylabel('Time (Frames)');

hold(uiTreeAx,'on');
[xMin, ~, xMax] = traverseTree(trackID,0);

xticks = unique(round((0:floor(imageData.NumberOfFrames/3))*3));
xtickLbl = cell(floor(imageData.NumberOfFrames/3),1);
for i=0:floor(imageData.NumberOfFrames/3)
    xtickLbl{i+1} = num2str(i);
end

if useDistance>0
        set(uiTreeAx,...
        'XLim',     [0 imageData.NumberOfFrames],...
        'XTick',    xticks,...
        'XColor','w',...
        'YColor','w',...
        'XTickLabel',   xtickLbl);
    xlabel(uiTreeAx,'Time (Hours)','Color','w');
    ylabel(uiTreeAx,'Distance to Vasculature (voxels)','Color','w');
else
    set(uiTreeAx,...
        'YLim',     [0 imageData.NumberOfFrames],...
        'XLim',     [xMin-1 xMax+1],...
        'YTick',    xticks,...
        'YTickLabel',   xtickLbl,...
        'Color','w');
    ylabel(uiTreeAx,'Time (Hours)','Color','w');
end

if useDistance>0
    set(uiTreeAx,...
        'CameraUpVector', [0 0 1],...
        'CameraUpVectorMode', 'manual');
end

set(uiTreeFig,'color',[0.35 0.35 0.35]);
set(uiTreeAx,'color',[0.35 0.35 0.35]);
t = str2double(get(uiControlHandles.tb_curFrame,'String'));
UpdateSegmentationResults('on');
UpdateTime(t-1);

hold(uiTreeAx,'off');

end

function [xMin, xCenter, xMax, initDist] = traverseTree(trackID,initXmin)
global Tracks

if(~isempty(Tracks(trackID).childrenTracks))
    [child1Xmin, child1Xcenter, child1Xmax, childDist1] = traverseTree(Tracks(trackID).childrenTracks(1),initXmin);
    [child2Xmin, child2Xcenter, child2Xmax, childDist2] = traverseTree(Tracks(trackID).childrenTracks(2),child1Xmax+1);
    xMin = min(child1Xmin,child2Xmin);
    xMax = max(child1Xmax,child2Xmax);
    if(child1Xcenter < child2Xcenter)
        drawHorizontalEdge(child1Xcenter,child2Xcenter,Tracks(trackID).endFrame,childDist1,childDist2);
        xCenter = (child2Xcenter-child1Xcenter)/2 + child1Xcenter;
    else
        drawHorizontalEdge(child2Xcenter,child1Xcenter,Tracks(trackID).endFrame,childDist2,childDist1);
        xCenter = (child1Xcenter-child2Xcenter)/2 + child2Xcenter;
    end
    initDist = (childDist1+childDist2)/2;
    initDist = drawVerticalEdge(trackID,xCenter,initDist);
else
    %This is when the edge is for a leaf node
    initDist = drawVerticalEdge(trackID,initXmin);
    xMin = initXmin;
    xCenter = initXmin;
    xMax = initXmin;
end
end

function drawHorizontalEdge(xMin,xMax,y,z1,z2)
global uiTreeAx useDistance
if useDistance>0
%    plot3([xMin xMax],[y y],[z1 z2],'-k','LineWidth',1.5);
    plot(uiTreeAx,[y y],[z1 z2],'-k','LineWidth',1.5);
else
    plot(uiTreeAx,[xMin xMax],[y y],'-k','LineWidth',1.5);
end
%Place the line behind all other elements already graphed
h = get(uiTreeAx,'child');
h = h([2:end, 1]);
set(uiTreeAx, 'child', h);
%end
end

function initDist = drawVerticalEdge(trackID,xVal,endDist)
global Tracks Hulls uiTreeAx useDistance

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
if useDistance>0
    distances = arrayfun(@(x)(Hulls(x).distances(useDistance)),Tracks(trackID).hulls);
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

    plot(uiTreeAx,[Hulls(Tracks(trackID).hulls(end)).frame-1 Tracks(trackID).endFrame],[distances(length(Tracks(trackID).hulls)) endDist],'-','color',FaceColor,'LineWidth',2.0);
    
    childhandles = get(uiTreeAx, 'children');
    childhandles = childhandles([2:end 1]);
    set(uiTreeAx, 'children',childhandles);
%     plot3([xVal xVal],...
%         [CellHulls(CellTracks(trackID).hulls(i+1)).frame-1 CellTracks(trackID).endFrame],...
%         [CellHulls(CellTracks(trackID).hulls(i+1)).distanceToVascCOM CellHulls(CellTracks(trackID).hulls(i+1)).distanceToVascCOM],...
%         ':','color',FaceColor,'LineWidth',1.0);

else
    plot(uiTreeAx,[xVal xVal], [Tracks(trackID).startFrame-1 Tracks(trackID).endFrame],'-w','LineWidth',1.5);
end
% plot([xVal xVal],[yMin CellTracks(trackID).endFrame],...
%     '-k','UserData',trackID,'LineWidth',1.5);

if useDistance>0
    plot(uiTreeAx,yMin,distances(1),...
        'o',...
        'MarkerFaceColor',  FaceColor,...
        'MarkerEdgeColor',  EdgeColor,...
        'MarkerSize',       circleSize,...
        'UserData',         trackID);
    
    text(yMin,distances(1),...
        num2str(trackID),...
        'Parent',uiTreeAx,...
        'HorizontalAlignment',  'center',...
        'FontSize',             FontSize,...
        'color',                TextColor,...
        'UserData',             trackID);
    
    initDist = distances(1);
else
    plot(uiTreeAx,xVal,yMin,...
        'o',...
        'MarkerFaceColor',  FaceColor,...
        'MarkerEdgeColor',  EdgeColor,...
        'MarkerSize',       circleSize,...
        'UserData',         trackID);
    
    text(xVal,yMin,...
        num2str(trackID),...
        'Parent',uiTreeAx,...
        'HorizontalAlignment',  'center',...
        'FontSize',             FontSize,...
        'color',                TextColor,...
        'UserData',             trackID);
end
end
