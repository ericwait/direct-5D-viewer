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

function Segment(chan, minCellDia)
global imageData orgImage segImage Hulls Tracks Families Costs hullChan
tic
[Hulls,Tracks,Families] = GetEmptyStructs();

hullChan = chan;

for t=1:size(segImage,5)
    bw = segImage(:,:,:,chan,t)>0;
    Hulls = [Hulls CreateHulls(bw,orgImage(:,:,:,chan,t),minCellDia,t)];
end

for i=1:length(Hulls)
    Hulls(i).label = i;
end

segTime = toc;
fprintf('Segmentation took: %f or %f avg per frame and found %d cells\n',segTime,segTime/size(segImage,5),length(Hulls));

imageDims = [imageData.XDimension, imageData.YDimension, imageData.ZDimension];
physDims = [imageData.XPixelPhysicalSize, imageData.YPixelPhysicalSize, imageData.ZPixelPhysicalSize];
scaleFactor  =  imageDims ./ max(imageDims) .* physDims/physDims(1);
minCellDiaVox = minCellDia / imageDims(1) * scaleFactor(1);

if (~isempty(Hulls))
    tic
    [newHulls,numTracks,Costs] = trackerMex(imageData.NumberOfFrames,Hulls,minCellDiaVox*6,minCellDiaVox*2);
    
    Tracks(numTracks).startFrame = -1;
    Families(numTracks).startFrame = -1;
    
    for i=1:length(newHulls)
        Hulls(i).track = newHulls(i).track;
        Hulls(i).color = newHulls(i).color;
        
        if (Tracks(Hulls(i).track).hulls == -1)
            Tracks(Hulls(i).track).hulls = i;
        else
            Tracks(Hulls(i).track).hulls = [Tracks(Hulls(i).track).hulls i];
        end
    end
    
    for i=1:length(Tracks)
        Tracks(i).parent = -1;
        Tracks(i).childrenTracks = [];
        Tracks(i).family = i;
        Tracks(i).color = Hulls(Tracks(i).hulls(1)).color;
        Tracks(i).startFrame = min([Hulls(Tracks(i).hulls).frame]);
        Tracks(i).endFrame = max([Hulls(Tracks(i).hulls).frame]);
        RehashTracksHulls(i);
        
        Families(i).tracks = i;
        Families(i).rootTrack = i;
        
        Families(i).startFrame = Tracks(i).startFrame;
        Families(i).endFrame = Tracks(i).endFrame;
    end
    
    ProcessNewborns(minCellDiaVox*6,minCellDiaVox*2);
    trackTime = toc;
    fprintf('Tracking took: %f or %f avg per frame and found %d tracks\n',trackTime,trackTime/size(segImage,5),length(Tracks));
    
    lever_3d('loadHulls',Hulls);
    DrawTree();
end
end
