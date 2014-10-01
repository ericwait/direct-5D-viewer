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

function newTrackID = SplitTrack(trackID,hullID)

global Families Tracks Hulls

%get all the hulls that come after hullID (in time that is)
%this also assumes that hulls are ordered
hullIndex = find([Tracks(trackID).hulls]==hullID);

if(1==hullIndex)
    error('Trying to split a track at its root. TrackID: %d, HullID: %d',trackID,hullID);
    return
elseif(isempty(hullIndex))
    error('Trying to split a track at a non-existent hull. TrackID: %d, HullID: %d',trackID,hullID);
    return
end

hullList = Tracks(trackID).hulls(hullIndex:end);
Tracks(trackID).hulls(hullIndex:end) = [];
Tracks(trackID).endFrame = Tracks(trackID).startFrame + length(Tracks(trackID).hulls) - 1;

newTrackID = length(Tracks(1,:)) + 1;

%create the new track
Tracks(newTrackID).family = Tracks(trackID).family;
Tracks(newTrackID).parentTrack = trackID;
Tracks(newTrackID).childrenTracks = [];
Tracks(newTrackID).hulls = hullList;
RehashTracksHulls(newTrackID);
Tracks(newTrackID).color = Tracks(trackID).color;

%attach the parents children to the newTrack
Tracks(newTrackID).childrenTracks = Tracks(trackID).childrenTracks;
for i=1:length(Tracks(newTrackID).childrenTracks)
    Tracks(Tracks(newTrackID).childrenTracks(i)).parentTrack = newTrackID;
end
Tracks(trackID).childrenTracks = newTrackID;

%update the family
trackIndex = length(Families(Tracks(newTrackID).family).tracks) + 1;
Families(Tracks(newTrackID).family).tracks(trackIndex) = newTrackID;

for i=1:length(hullList)
    Hulls(hullList(i)).track = newTrackID;
end
%update HashedCells
%UpdateHashedCellsTrackID(newTrackID,hullList,Tracks(newTrackID).startTime);

end
