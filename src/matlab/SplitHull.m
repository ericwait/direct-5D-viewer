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

function SplitHull(hull, k)
global Hulls Tracks Families orgMetadata imageData segMetadata trackHulls familyHulls selectedHull uiControlHandles

gmoptions = statset('Display','off', 'MaxIter',400);
obj = fitgmdist(Hulls(hull).pixelsOrg, k, 'Replicates',15, 'Options',gmoptions);
kIdx = cluster(obj, Hulls(hull).pixelsOrg);

im = zeros(imageData.XDimension, imageData.YDimension, imageData.ZDimension, 'uint8');
for i=1:k
    idx = sub2ind([imageData.YDimension, imageData.XDimension, imageData.ZDimension],Hulls(hull).pixelsOrg(kIdx==i,2),...
        Hulls(hull).pixelsOrg(kIdx==i,1),Hulls(hull).pixelsOrg(kIdx==i,3));
    im(idx) = i;
end

newHulls = CreateHulls(im,tiffReader(fullfile(imageData.imageDir,imageData.DatasetName),Hulls(hull).frame,segMetadata.HullsChan,[],[],[],true),...
    1,Hulls(hull).frame,0,0,1);

oldHull = Hulls(hull);
Hulls(hull) = newHulls(1);
Hulls(hull).label = oldHull.label;
Hulls(hull).color = oldHull.color;
Hulls(hull).track = oldHull.track;

newHullIdx = [];
for i=2:length(newHulls)
    Hulls(end+1) = newHulls(i);
    Hulls(end).label = length(Hulls);
    Hulls(end).track = length(Tracks) +1;
    newHullIdx = [newHullIdx length(Hulls)];
    
    Tracks(end+1).hulls = length(Hulls);
    Tracks(end).color = Hulls(end).color;
    Tracks(end).startFrame = Hulls(end).frame;
    Tracks(end).endFrame = Hulls(end).frame;
    Tracks(end).family = length(Families)+1;
    
    Families(end+1).tracks = length(Tracks);
    Families(end).rootTrack = length(Tracks);
    Families(end).startFrame = Hulls(end).frame;
    Families(end).endFrame = Hulls(end).frame;
end

lever_3d('updateHulls',Hulls(hull));
lever_3d('addHulls',Hulls(newHullIdx));

[trackHulls, familyHulls] = GetHullsToDisplay(selectedHull);
familyHulls = [familyHulls newHullIdx];
if (get(uiControlHandles.cb_ShowFamily,'Value')==1)
    lever_3d('displayHulls',familyHulls);
else
    lever_3d('displayHulls',trackHulls);
end
end

