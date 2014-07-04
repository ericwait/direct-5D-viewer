function [plane] = CreateCleavagePlane(hullId1, hullId2)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

global Hulls

xDim = 0.1;
yDim = 0.1;

normVec = Hulls(hullId1).centerOfMass - Hulls(hullId2).centerOfMass;

centerPoint = (Hulls(hullId1).centerOfMass + Hulls(hullId2).centerOfMass)./2;

[inds, verts] = CreatePlane(normVec,centerPoint,xDim,yDim);

plane = struct(...
    'label',{},...
    'frame',{},...
    'centerOfMass',{},...
    'track',{},...
    'color',{},...
    'boundingBox',{},...
    'pixels',{},...
    'faces',{},...
    'verts',{},...
    'norms',{}...
    );

plane(1).frame = Hulls(hullId1).frame;
plane(1).centerOfMass = centerPoint;%(centerPoint - 0.5) ./ imDiv-1 .* scaleFactor;

verts = verts(:,1:3);
% imDivArray = repmat(imDiv,size(verts,1),1);
% imScaleArray = repmat(scaleFactor,size(verts,1),1);
% verts = ((verts-0.5)./imDivArray-1) .* imScaleArray;

plane(1).boundingBox(1) = min(verts(:,1));
plane(1).boundingBox(2) = min(verts(:,2));
plane(1).boundingBox(3) = min(verts(:,3));
plane(1).boundingBox(4) = max(verts(:,1));
plane(1).boundingBox(5) = max(verts(:,2));
plane(1).boundingBox(6) = max(verts(:,3));

plane(1).verts = verts;
plane(1).faces = inds;

sqrNorm = normVec.^2;
unitNorm = normVec./sqrt(sum(sqrNorm));

normals = ones(size(verts,1),3);
normals(:,1) = normals(:,1)*unitNorm(1);
normals(:,2) = normals(:,2)*unitNorm(2);
normals(:,3) = normals(:,3)*unitNorm(3);

plane(1).norms = normals;
plane(1).track = -1;

plane(1).color = [0.85 1 1];
end

