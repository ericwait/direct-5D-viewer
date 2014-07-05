function [curHull, curTrack] = CreateCleavagePlane(hullId1, hullId2)
global Hulls Tracks Families

xDim = 0.1;
yDim = 0.1;

normVec = Hulls(hullId1).centerOfMass - Hulls(hullId2).centerOfMass;

centerPoint = (Hulls(hullId1).centerOfMass + Hulls(hullId2).centerOfMass)./2;

[inds, verts] = CreatePlane(normVec,centerPoint,xDim,yDim);

Hulls(end+1).frame = Hulls(hullId1).frame;
curHull = length(Hulls);
Hulls(curHull).label = curHull;
Tracks(end+1).hulls = curHull;
curTrack = length(Tracks);
Hulls(curHull).track = curTrack;
Tracks(curTrack).startFrame = Hulls(curHull).frame;
Tracks(curTrack).endFrame = Hulls(curHull).frame;
Tracks(curTrack).parentTrack = Tracks(Hulls(hullId1).track).parentTrack;
Tracks(curTrack).family = Tracks(Hulls(hullId1).track).family;
Families(Tracks(curTrack).family).tracks = [Families(Tracks(curTrack).family).tracks curTrack];

Hulls(curHull).centerOfMass = centerPoint;
verts = verts(:,1:3);
Hulls(curHull).boundingBox(1) = min(verts(:,1));
Hulls(curHull).boundingBox(2) = min(verts(:,2));
Hulls(curHull).boundingBox(3) = min(verts(:,3));
Hulls(curHull).boundingBox(4) = max(verts(:,1));
Hulls(curHull).boundingBox(5) = max(verts(:,2));
Hulls(curHull).boundingBox(6) = max(verts(:,3));

Hulls(curHull).verts = verts;
Hulls(curHull).faces = inds;

sqrNorm = normVec.^2;
unitNorm = normVec./sqrt(sum(sqrNorm));

normals = ones(size(verts,1),3);
normals(:,1) = normals(:,1)*unitNorm(1);
normals(:,2) = normals(:,2)*unitNorm(2);
normals(:,3) = normals(:,3)*unitNorm(3);
Hulls(curHull).norms = normals;

Hulls(curHull).color = [0.85 1 1];
Tracks(curTrack).color  = [0.85 1 1];
end

