function [trackHulls, familyHulls] = GetHullsToDisplay(curHull)
global Hulls Tracks Families 

if (curHull<1 || curHull>length(Hulls))
    trackHulls = [];
    familyHulls = [];
    return
end

curTrack = Hulls(curHull).track;
curFamily = Tracks(curTrack).family;
trackList = Families(curFamily).tracks;

trackHulls = Tracks(curTrack).hulls;
familyHulls = trackHulls;

for i=1:length(trackList)
    if (trackList(i)==curTrack), continue, end
    
    familyHulls = [familyHulls Tracks(trackList(i)).hulls];
end
end

