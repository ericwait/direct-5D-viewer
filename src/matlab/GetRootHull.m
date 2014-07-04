function [ hullId ] = GetRootHull( trackId )
global Tracks Hulls

for i=1:length(Tracks(trackId).hulls)
    if (Hulls(Tracks(trackId).hulls(i)).frame == Tracks(trackId).startFrame)
        hullId = Tracks(trackId).hulls(i);
        return 
    end
end
end

