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
    maxExtent_rc = Utils.SwapXY_RC(max(pixelList_xy,[],1));
    smallSide = min(maxExtent_rc);
    reduc = 10/smallSide;
    reduc = max(reduc,0.125);
    reduc = min(reduc,1.0);
    reductions = repmat(reduc,1,size(pixelList_xy,2));
end
if (length(reductions)==1)
    reductions = repmat(reductions,1,size(pixelList_xy,2));
end
if (~exist('quiet','var') || isempty(quiet))
    quiet = false;
end

%reductions = [1,1,1];

polygon = D3d.Polygon.MakeEmptyStruct();

% padd the subimage to get some room to blur
PADDING = ceil(5*1./reductions);

startCoords_rcz = Utils.SwapXY_RC(min(pixelList_xy,[],1));
endCoords_rcz = Utils.SwapXY_RC(max(pixelList_xy,[],1));

startPadded = startCoords_rcz-PADDING;
endPadded = endCoords_rcz+PADDING;

roiIm = false(ceil(endPadded-startPadded+1));

paddedPixelList_rcz = Utils.SwapXY_RC(pixelList_xy)-startPadded+1;
indList = Utils.CoordToInd(size(roiIm),paddedPixelList_rcz);
roiIm(indList) = true;

cc.Connectivity = 26;
cc.ImageSize = size(roiIm);
cc.NumObjects = 1;
cc.PixelIdxList = {indList};
rp = regionprops(cc,'Centroid');

imR = ImProc.Resize(im2uint8(roiIm),reductions,[],'mean');

R = linspace(startPadded(1),endPadded(1),size(imR,1));
C = linspace(startPadded(2),endPadded(2),size(imR,2));
Z = linspace(startPadded(3),endPadded(3),size(imR,3));
[x,y,z] = meshgrid(C,R,Z);

[faces, v_xy] = isosurface(x,y,z,imR,1);

if (isempty(v_xy))
    return
end

% center the vert in the middle of the voxel
verts_xy = v_xy + 0.5;

com = rp.Centroid + startPadded -1;

if (any(faces(:)==0))
    error('there is a zero index in the faces structure!');
end

norms = D3d.Polygon.CalcNorms(verts_xy,faces);

polygon(1).index = polyIdx;
polygon.frame = frame;
polygon.label = label;
polygon.color = color;
polygon.faces = faces;
polygon.verts = verts_xy;
polygon.norms = norms;
polygon.CenterOfMass = com;
end

