function [ newHulls ] = CreateHulls(bwIm, orgIm, minCellDia, frame)
global imageData

[newHulls,~,~] = GetEmptyStructs();
imageDims = [imageData.XDimension, imageData.YDimension, imageData.ZDimension];
physDims = [imageData.XPixelPhysicalSize, imageData.YPixelPhysicalSize, imageData.ZPixelPhysicalSize];
scaleFactor  =  imageDims ./ max(imageDims) .* physDims/physDims(1);
imDiv = imageDims /2;
minCellVol = (4*pi*(minCellDia/2)^3)/3;

stats = regionprops(bwIm,orgIm,'BoundingBox','PixelList','WeightedCentroid');

newHulls(length(stats)).label = -1;

for i=1:length(stats) 
    if (size(stats(i).PixelList,1)<minCellVol || size(stats(i).PixelList,1)>imageData.XDimension*imageData.YDimension)
        continue;
    end
    
    newHulls(i).pixelsOrg = stats(i).PixelList;
    newHulls(i).centerOfMassOrg = stats(i).WeightedCentroid;
    
    stats(i).PixelList = stats(i).PixelList - 0.5;
    stats(i).BoundingBox(1,1:3) = stats(i).BoundingBox(1,1:3) - 0.5;
    stats(i).WeightedCentroid = stats(i).WeightedCentroid - 0.5;
    
    k = convhulln(stats(i).PixelList);
    u = unique(k(:));
    remap = zeros(size(stats(i).PixelList,1),1);
    remap(u) = 1:length(u);
    verts = stats(i).PixelList(u,:);
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
        
        dst = stats(i).WeightedCentroid - v1;
        nrmDir = dot(uNrm,dst);
        if (nrmDir>0)
            faces(j,[1,2]) = faces(j,[2,1]);
        end
    end
    
    norms = calcVertNormals(verts,faces);
    
    newHulls(i).label = i;
    newHulls(i).frame = frame;
    newHulls(i).centerOfMass = (stats(i).WeightedCentroid ./ imDiv-1) .* scaleFactor;
    newHulls(i).track = -1;
    color = GetNextColor();
    newHulls(i).color = color.background;
    bbStart = (stats(i).BoundingBox(1,1:3)./imDiv-1) .* scaleFactor;
    bbEnd = stats(i).BoundingBox(1,4:6) ./ [imageData.XDimension,imageData.YDimension,imageData.ZDimension];
    newHulls(i).boundingBox = [bbStart, bbEnd];
    imDivArray = repmat(imDiv,size(stats(i).PixelList,1),1);
    imScaleArray = repmat(scaleFactor,size(stats(i).PixelList,1),1);
    newHulls(i).pixels = (stats(i).PixelList./imDivArray-1) .* imScaleArray;
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

