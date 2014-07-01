%ChangLabel(time,oldTrackID,newTrackID)
%This will attempt to change the trackID from a given time until the end of
%the track.  However, the trackID will only be changed up to the point that
%the newTrackID does not exist.  If when moving hulls from the oldTrack to
%the newTrack, there is a hull already in the newTrack for a given frame,
%this function will return without moving any more hulls.
%
%If the whole oldTrack ends up moving over to the newTrack, and the
%oldTrack has a sibling, the oldTrack's sibling will be merged into the
%parent.
%
%Anytime that the oldTrack can be fully moved from the given time all the
%way to the endFrame of the track, the oldTrack's subtree will move with it.
%The only time that the subtree would not move with the change would be as
%stated above.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%     Copyright 2011 Andrew Cohen, Eric Wait and Mark Winter
%
%     This file is part of LEVer - the tool for stem cell lineaging. See
%     https://pantherfile.uwm.edu/cohena/www/LEVer.html for details
% 
%     LEVer is free software: you can redistribute it and/or modify
%     it under the terms of the GNU General Public License as published by
%     the Free Software Foundation, either version 3 of the License, or
%     (at your option) any later version.
% 
%     LEVer is distributed in the hope that it will be useful,
%     but WITHOUT ANY WARRANTY; without even the implied warranty of
%     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%     GNU General Public License for more details.
% 
%     You should have received a copy of the GNU General Public License
%     along with LEVer in file "gnu gpl v3.txt".  If not, see 
%     <http://www.gnu.org/licenses/>.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ChangeLabel(time,oldTrackID,newTrackID)

global Tracks Hulls SegmentationEdits
bothHaveChildren = 0;%flag to deal with conflicting children
%flags to deal with position of tracks relitive to one another
oldBeforeNew = Tracks(newTrackID).startFrame > Tracks(oldTrackID).endFrame;
newBeforeOld = Tracks(oldTrackID).startFrame > Tracks(newTrackID).endFrame;
oldEmptied = 0;%flag to check to see if all the Hulls were moved from the oldTrack

%march through the hulls removing them from the oldTrack and adding them to
%the newTrack
oldTrackHashedTime = time - Tracks(oldTrackID).startFrame + 1;
newTrackHashedTime = time - Tracks(newTrackID).startFrame + 1;
hulls = Tracks(oldTrackID).hulls(oldTrackHashedTime:end);
lastOldHull = hulls(end);

%if they both have children keep record of who's should be kept
if(~isempty(Tracks(oldTrackID).childrenTracks) && ~isempty(Tracks(newTrackID).childrenTracks))
    bothHaveChildren = 1;
end

for i=1:length(hulls)
    if(newTrackHashedTime+i-1<0 || (newTrackHashedTime+i-1<=length(Tracks(newTrackID).hulls) &&...
            Tracks(newTrackID).hulls(newTrackHashedTime+i-1)~=0)),break,end;
    if(hulls(i)~=0)
        AddHullToTrack(hulls(i),newTrackID,[]);
    end
    Tracks(oldTrackID).hulls(oldTrackHashedTime+i-1) = 0;
end

firstHull = find(Tracks(oldTrackID).hulls,1);
if(isempty(firstHull))
    oldEmptied = 1;
else
    firstHull = Tracks(oldTrackID).hulls(firstHull);
    RehashCellTracks(oldTrackID,Hulls(firstHull).time);
end

if(oldEmptied)
    if(oldBeforeNew)
        %deal with any children
        if(~isempty(Tracks(oldTrackID).childrenTracks))
            RemoveChildren(oldTrackID);
        end
        if(~isempty(Tracks(newTrackID).siblingTrack) && ...
                isempty(find(Tracks(oldTrackID).childrenTracks==Tracks(newTrackID).siblingTrack, 1)))
            CombineTrackWithParent(Tracks(newTrackID).siblingTrack);
        end
        Tracks(newTrackID).parentTrack = Tracks(oldTrackID).parentTrack;
        index = Tracks(Tracks(newTrackID).parentTrack).childrenTracks==oldTrackID;
        Tracks(Tracks(newTrackID).parentTrack).childrenTracks(index) = newTrackID;
        Tracks(newTrackID).siblingTrack = Tracks(oldTrackID).siblingTrack;
        Tracks(newTrackID).family = Tracks(oldTrackID).family;
    elseif(newBeforeOld)
        %deal with any children
        if(bothHaveChildren)
            index = Tracks(newTrackID).childrenTracks==oldTrackID;
            if(~isempty(find(index, 1)))
                RemoveFromTree(Tracks(Tracks(newTrackID).childrenTracks(~index)).startFrame,...
                    Tracks(newTrackID).childrenTracks(~index),'no');
                Tracks(newTrackID).childrenTracks = [];
                Tracks(oldTrackID).siblingTrack = [];
            else
                RemoveChildren(newTrackID);
            end
            moveChildren(oldTrackID,newTrackID);
        elseif(~isempty(Tracks(newTrackID).childrenTracks))
            index = Tracks(newTrackID).childrenTracks==oldTrackID;
            if(~isempty(find(index, 1)))
                RemoveFromTree(Tracks(Tracks(newTrackID).childrenTracks(~index)).startFrame,...
                    Tracks(newTrackID).childrenTracks(~index),'no');
                Tracks(newTrackID).childrenTracks = [];
                Tracks(oldTrackID).siblingTrack = [];
            else
                RemoveChildren(newTrackID);
            end
        elseif(~isempty(Tracks(oldTrackID).childrenTracks))
            moveChildren(oldTrackID,newTrackID);
        end
        if(~isempty(Tracks(oldTrackID).siblingTrack) && ...
                isempty(find(Tracks(newTrackID).childrenTracks==Tracks(oldTrackID).siblingTrack, 1)))
            CombineTrackWithParent(Tracks(oldTrackID).siblingTrack);
        end
    else
        if(~isempty(Tracks(oldTrackID).childrenTracks))
            RemoveChildren(oldTrackID);
        end
        if(~isempty(Tracks(oldTrackID).siblingTrack))
            CombineTrackWithParent(Tracks(oldTrackID).siblingTrack);
        end
    end
    
    %clean up other fields
    RemoveTrackFromFamily(oldTrackID);
    ClearTrack(oldTrackID);
else %the old track still exists in some fasion
    if(isempty(find(Tracks(oldTrackID).hulls==lastOldHull, 1)) &&...
            ~isempty(Tracks(oldTrackID).childrenTracks))
        %the last hull from the old track has been moved over and had
        %children
        if(Hulls(lastOldHull).time<Tracks(newTrackID).endFrame)
            RemoveChildren(oldTrackID);
        elseif(Hulls(lastOldHull).time>=Tracks(newTrackID).endFrame)
            if(~isempty(Tracks(newTrackID).childrenTracks))
                RemoveChildren(newTrackID);
            end
            moveChildren(oldTrackID,newTrackID);
        end
    elseif(~isempty(Tracks(newTrackID).childrenTracks) && ...
            (((isempty(find(Tracks(oldTrackID).hulls==lastOldHull, 1)) &&...
            Hulls(lastOldHull).time>Tracks(newTrackID).endFrame)) || ...
            any([Tracks(Tracks(newTrackID).childrenTracks).startFrame]<Tracks(newTrackID).endFrame)))
        RemoveChildren(newTrackID);
    end
end

%check to see if either of the tracks are dead
familyIDs = [];
if(~isempty(GetTimeOfDeath(oldTrackID)))
    familyIDs = StraightenTrack(oldTrackID);
end
if(~isempty(GetTimeOfDeath(newTrackID)))
    familyIDs = [familyIDs StraightenTrack(newTrackID)];
end
if(~isempty(familyIDs))
    ProcessNewborns(familyIDs, SegmentationEdits.maxEditedFrame);
end
end

function moveChildren(oldTrackID,newTrackID)
global CellTracks
if(isempty(CellTracks(oldTrackID).childrenTracks))
    %no children
elseif(isempty(CellTracks(newTrackID).childrenTracks))
    CellTracks(newTrackID).childrenTracks = CellTracks(oldTrackID).childrenTracks;
    for i=1:length(CellTracks(newTrackID).childrenTracks)
        CellTracks(CellTracks(newTrackID).childrenTracks(i)).parentTrack = newTrackID;
    end
    
    if(CellTracks(oldTrackID).family ~= CellTracks(newTrackID).family)
        for i=1:length(CellTracks(oldTrackID).childrenTracks)
            ChangeTrackAndChildrensFamily(CellTracks(oldTrackID).family,CellTracks(newTrackID).family,CellTracks(oldTrackID).childrenTracks(i));
        end
    end
    CellTracks(oldTrackID).childrenTracks = [];
else
    error('Children conflict');
end
end
