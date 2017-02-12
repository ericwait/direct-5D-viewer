function [ polygon ] = Make( pixelList_xy, polyIdx, label, frame, color, reductions, quiet )
%[ polygon ] = D3d.Polygon.Make( pixelList_xy, polyIdx, label, frame, color, reductions, quiet )
if (~exist('color','var') || isempty(color))
    color = [1,1,1,1];
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
PADDING = ceil(3*1./reductions);

maxExtent_rc = Utils.SwapXY_RC(max(pixelList_xy,[],1)) + PADDING;
indList = Utils.CoordToInd(maxExtent_rc,Utils.SwapXY_RC(pixelList_xy));
[im,startCoords_rcz] = ImUtils.ROI.MakeSubImBW(maxExtent_rc,indList,PADDING);
startCoords_xy = Utils.SwapXY_RC(startCoords_rcz);

cc.Connectivity = 26;
cc.ImageSize = size(im);
cc.NumObjects = 1;
cc.PixelIdxList = {find(im)};
rp = regionprops(cc,'Centroid');

imR = ImProc.Resize(im2uint8(im),reductions,[],'mean');

R = linspace(startCoords_rcz(1),maxExtent_rc(1),size(imR,1));
C = linspace(startCoords_rcz(2),maxExtent_rc(2),size(imR,2));
Z = linspace(startCoords_rcz(3),maxExtent_rc(3),size(imR,3));
[x,y,z] = meshgrid(C,R,Z);

[faces, v_xy] = isosurface(x,y,z,imR,128);

if (isempty(v_xy))
    return
end

% center the vert in the middle of the voxel
verts_xy = v_xy - 0.5;

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

