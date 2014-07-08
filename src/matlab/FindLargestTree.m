function familyID = FindLargestTree()
global Tracks Families selectedHull trackHulls familyHulls

familyID = 1;
for i=2:length(Families)
    if(length(Families(familyID).tracks) < length(Families(i).tracks))
        familyID = i;
        selectedHull = Tracks(Families(i).tracks(1)).hulls(1);
        [trackHulls, familyHulls] = GetHullsToDisplay(selectedHull);
    end
end 
end
