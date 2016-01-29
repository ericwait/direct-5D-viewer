function [ polygon ] = Make( pixelList_xy, polyIdx, label, frame, color, reductions, quiet )
%[ polygon ] = D3d.Polygon.Make( pixelList_xy, polyIdx, label, frame, color, reductions, quiet )
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
if (~exist('quiet','var') || isempty(quiet))
    quiet = false;
end

polygon = D3d.Polygon.MakeEmptyStruct();

% padd the subimage to get some room to blur
PADDING = 3*reductions;

maxExtent_rc = Utils.SwapXY_RC(max(pixelList_xy,[],1)) + PADDING;
indList = Utils.CoordToInd(maxExtent_rc,Utils.SwapXY_RC(pixelList_xy));
[im,shiftCoords_rcz] = ImUtils.ROI.MakeSubImBW(maxExtent_rc,indList,PADDING);
shiftCoords_xy = Utils.SwapXY_RC(shiftCoords_rcz);

cc.Connectivity = 26;
cc.ImageSize = size(im);
cc.NumObjects = 1;
cc.PixelIdxList = {find(im)};
rp = regionprops(cc,'Centroid');

[x,y,z,D] = reducevolume(im,reductions);
D = smooth3(D);

[faces, v_xy] = isosurface(x,y,z,D,graythresh(D));

if (isempty(v_xy))
    return
end

% center the vert in the middle of the voxel
v_xy = v_xy + 0.5;

verts_xy = v_xy + repmat(startCoords_xy -1,size(v_xy,1),1);
com = rp.Centroid + startCoords_xy -1;

if (any(faces(:)==0))
    error('there is a zero index in the faces structure!');
end

norms = D3d.Polygon.CalcNorms(verts_xy,faces);

polygon.index = polyIdx;
polygon.frame = frame;
polygon.label = label;
polygon.color = color;
polygon.faces = faces;
polygon.verts = verts_xy;
polygon.norms = norms;
polygon.CenterOfMass = com;
end

