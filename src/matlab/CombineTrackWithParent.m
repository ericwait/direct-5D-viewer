% CombineTrackWithParent(trackID)
%Combine childTrack with its parent track - in otherwords the childtrack
%will be merged into the parent track making it all one edge.
%Afterward all of the hulls previously associated with the given track will
%be part of its parent.  The parent will inherit the grandchildren of the
%given track.
%***Make sure that the sibling of the given track has been delt with prior
%to calling this function. IT WILL BE LOST

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

function CombineTrackWithParent(trackID)

global Families Tracks Hulls

parentID = Tracks(trackID).parentTrack;

%combine track hulls with parent hulls
hullList = Tracks(trackID).hulls;
for i=1:length(hullList)
    if(hullList(i)~=0)
        time = Hulls(hullList(i)).time;
        hash = time - Tracks(parentID).startFrame +1;
        if(length(Tracks(parentID).hulls)>hash && (Tracks(parentID).hulls(hash)))
            error('Trying to place a hull where one already exists. Track: %d, Time: %d.',parentID,time);
        else
            Tracks(parentID).hulls(hash) = hullList(i);
        end
    end
end

%inherit grandchildren
Tracks(parentID).childrenTracks = Tracks(trackID).childrenTracks;
for i=1:length(Tracks(parentID).childrenTracks)
    Tracks(Tracks(parentID).childrenTracks(i)).parentTrack = parentID;
end

%update the endFrame of the parent
Tracks(parentID).endFrame = Tracks(trackID).endFrame;

%remove trackID from family
trackIndex = [Families(Tracks(trackID).family).tracks]==trackID;
Families(Tracks(trackID).family).tracks(trackIndex) = [];

%update HashedCells
UpdateHashedCellsTrackID(parentID,hullList,Tracks(trackID).startFrame);

%clear out track
ClearTrack(trackID);
end
