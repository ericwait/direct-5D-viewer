function newTrackID = SplitTrack(trackID,hullID)

global Families Tracks

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
Tracks(newTrackID).startFrame = Tracks(trackID).startFrame + hullIndex - 1;
Tracks(newTrackID).endFrame = Tracks(newTrackID).startFrame + length(hullList) - 1;
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

%update HashedCells
%UpdateHashedCellsTrackID(newTrackID,hullList,Tracks(newTrackID).startTime);

end
