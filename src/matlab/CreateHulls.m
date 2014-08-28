function [ newHulls ] = CreateHulls(bwIm, orgIm, minCellDia, frame, minXY, minZ)
%CreateHulls(bwIm, orgIm, minCellDia, frame, minXY, minZ) Creates hull
%structures for a given frame.
%   minCellDia is the smallest elipsoid that the blobs should fit within.
%   minXY and minZ are optional
global imageData

newHulls = [];

if ~exist('minXY','var')
    minXY = 0;
end
if ~exist('minZ','var')
    minZ = 0;
end

[newHulls,~,~] = GetEmptyStructs();
imageDims = [imageData.XDimension, imageData.YDimension, imageData.ZDimension];
physDims = [imageData.XPixelPhysicalSize, imageData.YPixelPhysicalSize, imageData.ZPixelPhysicalSize];
scaleFactor  =  imageDims ./ max(imageDims) .* physDims/physDims(1);
imDiv = imageDims /2;
minCellVol = (4*pi*(minCellDia/2)^3)/3;

stts = regionprops(bwIm,orgIm,'BoundingBox','PixelList','WeightedCentroid');
if isempty(stts)
    return
end

bb = vertcat(stts(:).BoundingBox);
if (max(bb(:,4))<minXY || max(bb(:,5))<minXY || max(bb(:,6))<minZ)
    return
end

newHulls(length(stts)).label = -1;

for i=1:length(stts) 
    if (size(stts(i).PixelList,1)<minCellVol ||...
            size(stts(i).PixelList,1)>imageData.XDimension*imageData.YDimension ||...
            (stts(i).BoundingBox(4)<minXY && stts(i).BoundingBox(5)<minXY) || stts(i).BoundingBox(6)<minZ)
        continue;
    end
    
    newHulls(i).pixelsOrg = stts(i).PixelList;
    newHulls(i).boundingBoxOrg = stts(i).BoundingBox;
    newHulls(i).centerOfMassOrg = stts(i).WeightedCentroid;
    
    stts(i).PixelList = stts(i).PixelList - 0.5;
    stts(i).BoundingBox(1,1:3) = stts(i).BoundingBox(1,1:3) - 0.5;
    stts(i).WeightedCentroid = stts(i).WeightedCentroid - 0.5;
    
    try
        k = convhulln(stts(i).PixelList);
    catch err
        continue
    end
    u = unique(k(:));
    remap = zeros(size(stts(i).PixelList,1),1);
    remap(u) = 1:length(u);
    verts = stts(i).PixelList(u,:);
    faces = remap(k);
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
        
        dst = stts(i).WeightedCentroid - v1;
        nrmDir = dot(uNrm,dst);
        if (nrmDir>0)
            faces(j,[1,2]) = faces(j,[2,1]);
        end
    end
    
    norms = calcVertNormals(verts,faces);
    
    newHulls(i).label = i;
    newHulls(i).frame = frame;
    newHulls(i).centerOfMass = (stts(i).WeightedCentroid ./ imDiv-1) .* scaleFactor;
    newHulls(i).track = -1;
    color = GetNextColor();
    newHulls(i).color = color.background;
    bbStart = (stts(i).BoundingBox(1,1:3)./imDiv-1) .* scaleFactor;
    bbEnd = stts(i).BoundingBox(1,4:6) ./ [imageData.XDimension,imageData.YDimension,imageData.ZDimension];
    newHulls(i).boundingBox = [bbStart, bbEnd];
    imDivArray = repmat(imDiv,size(stts(i).PixelList,1),1);
    imScaleArray = repmat(scaleFactor,size(stts(i).PixelList,1),1);
    newHulls(i).pixels = (stts(i).PixelList./imDivArray-1) .* imScaleArray;
    newHulls(i).faces = faces;
    imDivArray = repmat(imDiv,size(verts,1),1);
    imScaleArray = repmat(scaleFactor,size(verts,1),1);
    newHulls(i).verts = (verts./imDivArray-1) .* imScaleArray;
    newHulls(i).norms = norms;
end

goodHulls = [newHulls.label];
goodHulls = goodHulls(goodHulls>0);
newHulls = newHulls(goodHulls);
end

