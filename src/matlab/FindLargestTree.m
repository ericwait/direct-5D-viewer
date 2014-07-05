function familyID = FindLargestTree()
global Families

familyID = 1;
for i=2:length(Families)
    if(length(Families(familyID).tracks) < length(Families(i).tracks))
        familyID = i;
    end
end 
end
