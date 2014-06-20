function Segment(imIn, chan, minCellDia)
global CellHulls orgImage imageData

CellHulls = struct(...
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

imageDims = [imageData.XDimension, imageData.YDimension, imageData.ZDimension];
physDims = [imageData.XPixelPhysicalSize, imageData.YPixelPhysicalSize, imageData.ZPixelPhysicalSize];
scaleFactor  =  imageDims ./ max(imageDims) .* physDims/physDims(1);
imDiv = imageDims /2;
minCellVol = (4*pi*(minCellDia/2)^3)/3;
for t=1:size(imIn,4)
    bw = imIn(:,:,:,t)>0;
    
    stats = regionprops(bw,orgImage(:,:,:,chan,t),'BoundingBox','PixelList','WeightedCentroid');
    
    for i=1:length(stats)
        stats(i).PixelList = stats(i).PixelList - 0.5;
        stats(i).BoundingBox(1,1:3) = stats(i).BoundingBox(1,1:3) - 0.5;
        stats(i).WeightedCentroid = stats(i).WeightedCentroid - 0.5;
        
        if (size(stats(i).PixelList,1) < minCellVol)
            continue;
        end
        
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
        
        idx = length(CellHulls) + 1;
        CellHulls(idx).label = idx;
        CellHulls(idx).frame = t;
        CellHulls(idx).centerOfMass = (stats(i).WeightedCentroid ./ imDiv-1) .* scaleFactor;
        CellHulls(idx).track = -1;
        color = GetNextColor();
        CellHulls(idx).color = color.background;
        bbStart = (stats(i).BoundingBox(1,1:3)./imDiv-1) .* scaleFactor;
        bbEnd = stats(i).BoundingBox(1,4:6) ./ [imageData.XDimension,imageData.YDimension,imageData.ZDimension];
        CellHulls(idx).boundingBox = [bbStart, bbEnd];
        imDivArray = repmat(imDiv,size(stats(i).PixelList,1),1);
        imScaleArray = repmat(scaleFactor,size(stats(i).PixelList,1),1);
        CellHulls(idx).pixels = (stats(i).PixelList./imDivArray-1) .* imScaleArray;
        CellHulls(idx).faces = faces;
        imDivArray = repmat(imDiv,size(verts,1),1);
        imScaleArray = repmat(scaleFactor,size(verts,1),1);
        CellHulls(idx).verts = (verts./imDivArray-1) .* imScaleArray;
        CellHulls(idx).norms = norms;
    end
end

minCellDiaVox = minCellDia / imageDims(1) * scaleFactor(1);

if (~isempty(CellHulls))
    [newHulls,numTracks,costs] = trackerMex(imageData.NumberOfFrames,CellHulls,minCellDiaVox,minCellDiaVox/2);
    for i=1:size(CellHulls,2)
        CellHulls(i).color = newHulls(i).color;
        CellHulls(i).track = newHulls(i).track;
    end
    lever_3d('loadHulls',CellHulls);
end
end