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
