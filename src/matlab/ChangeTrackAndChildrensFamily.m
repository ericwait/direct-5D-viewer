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
