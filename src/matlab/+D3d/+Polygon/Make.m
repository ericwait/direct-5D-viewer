function [ polygon ] = Make( pixelList_xy, polyIdx, label, frame, color, reductions )
if (~exist('color','var') || isempty(color))
    color = [1,1,1];
end

if (~exist('label','var') || isempty(label))
    label = '';
end
if (~ischar(label))
    error('label needs to be a string!');
end
if (~exist('reductions','var') || isempty(reductions))
    reductions = ones(1,size(pixelList_xy,2));
end
if (length(reductions)==1)
    reductions = repmat(reductions,1,size(pixelList_xy,2));
end

PADDING = 3*reductions;

bb_xy = [min(pixelList_xy,[],1);max(pixelList_xy,[],1)];

subImSize_rc = Utils.SwapXY_RC(bb_xy(2,:) - bb_xy(1,:) + 2*PADDING +1);

shiftCoords_xy = max(zeros(1,size(bb_xy,2)),bb_xy(1,:) - PADDING +1);
shiftPixelCoords_xy = pixelList_xy - repmat(shiftCoords_xy,size(pixelList_xy,1),1);
shiftPixelInd = Utils.CoordToInd(subImSize_rc,Utils.SwapXY_RC(shiftPixelCoords_xy));

im = zeros(subImSize_rc);
im(shiftPixelInd) = 1;

rp = regionprops(im>0,'Centroid','PixelList');
centerOfMass_xy = rp.Centroid + shiftCoords_xy;

[x,y,z,D] = reducevolume(im,reductions);
D = smooth3(D);
[faces, v] = isosurface(x,y,z,D,1/3);
verts = Utils.SwapXY_RC(v) + repmat(shiftCoords_xy,size(v,1),1);

if (any(faces(:)==0))
    error('there is a zero index in the faces structure!');
end

for j=1:size(faces,1)
    v1 = verts(faces(j,1),:);
    v2 = verts(faces(j,2),:);
    v3 = verts(faces(j,3),:);
    
    e1 = v2-v1;
    e2 = v3-v1;
    uNrm = cross(e1,e2);
    
    dst = centerOfMass_xy - v1;
    nrmDir = dot(uNrm,dst);
    if (nrmDir>0)
        faces(j,[1,2]) = faces(j,[2,1]);
    end
end

norms = D3d.Polygon.CalcNorms(verts,faces);

polygon.index = polyIdx;
polygon.frame = frame;
polygon.label = label;
polygon.color = color;
polygon.faces = faces;
polygon.verts = verts;
polygon.norms = norms;
end

