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

function ChangeTrackParent(parentTrackID,time,childTrackID)
global Tracks Families

%see if the child exists before time
if(time > Tracks(childTrackID).startFrame)
    newFamilyID = RemoveFromTree(time,childTrackID,'yes');
    childTrackID = Families(newFamilyID).rootTrack;
end

%find where the child should attach to the parent
hash = time - Tracks(parentTrackID).startFrame + 1;
if(hash <= 0)
    error('Trying to attach a parent that comes after the child');
elseif(hash <= length(Tracks(parentTrackID).hulls))
    parentHullID = Tracks(parentTrackID).hulls(hash);
    siblingTrackID = SplitTrack(parentTrackID,parentHullID); % SplitTrack adds sibling to the parent already
else
    %just rename the child to the parent
     ChangeLabel(time,childTrackID,parentTrackID);
    return
end

oldFamilyID = Tracks(childTrackID).family;
newFamilyID = Tracks(siblingTrackID).family;

childIndex = length(Tracks(parentTrackID).childrenTracks) + 1;
Tracks(parentTrackID).childrenTracks(childIndex) = childTrackID;

%clean up old parent
if(~isempty(Tracks(childTrackID).siblingTrack))
    Tracks(Tracks(childTrackID).siblingTrack).siblingTrack = [];
     CombineTrackWithParent(Tracks(childTrackID).siblingTrack);
end
Tracks(childTrackID).parentTrack = parentTrackID;

%Detatch childTrack and clean up child's family
if(oldFamilyID~=newFamilyID)
     ChangeTrackAndChildrensFamily(oldFamilyID,newFamilyID,childTrackID);
end

Tracks(childTrackID).siblingTrack = siblingTrackID;
Tracks(siblingTrackID).siblingTrack = childTrackID;

end
