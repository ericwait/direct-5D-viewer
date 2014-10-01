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

function ChangeTrackAndChildrensFamily(oldFamilyID,newFamilyID,trackID)

global Families Tracks

%get the full list of tracks to be updateded
trackList = traverseTree(newFamilyID,trackID);

%remove tracks from family
for i=1:length(trackList)
    Tracks(trackList(i)).family = newFamilyID;
end

if(isempty(Families(oldFamilyID).tracks))
    Families(oldFamilyID).rootTrack = [];
    Families(oldFamilyID).startFrame = [];
    Families(oldFamilyID).endFrame = [];
else
    %update times
    Families(oldFamilyID).startFrame = min([Tracks(Families(oldFamilyID).tracks).startFrame]);
    Families(oldFamilyID).endFrame = max([Tracks(Families(oldFamilyID).tracks).endFrame]);
end
end

function trackList = traverseTree(newFamilyID,trackID)
%recursive helper function to traverse tree and gather track IDs
%will add the tracks to the new family along the way
global Families Tracks

RemoveTrackFromFamily(trackID);
%add track
Families(newFamilyID).tracks(end+1) = trackID;

%update times
if(Tracks(trackID).startFrame < Families(newFamilyID).startFrame)
    Families(newFamilyID).startFrame = Tracks(trackID).startFrame;
end
if(Tracks(trackID).endFrame > Families(newFamilyID).endFrame)
    Families(newFamilyID).endFrame = Tracks(trackID).endFrame;
end

trackList = trackID;
if(~isempty(Tracks(trackID).childrenTracks))    
    for i=1:length(Tracks(trackID).childrenTracks)
        trackList = [trackList traverseTree(newFamilyID, Tracks(trackID).childrenTracks(i))];
    end
end
end
