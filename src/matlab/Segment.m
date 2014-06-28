function Segment(imIn, chan, minCellDia)
global imageData orgImage Hulls Tracks Families Costs

Hulls = struct(...
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

Tracks = struct(...
    'hulls',{},...
    'startFrame',{},...
    'endFrame',{},...
    'parentTrack',{},...
    'childrenTracks',{},...
    'family',{},...
    'color',{});

Families = struct(...
    'tracks',{},...
    'rootTrack',{},...
    'startFrame',{},...
    'endFrame',{});

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
        
        idx = length(Hulls) + 1;
        Hulls(idx).label = idx;
        Hulls(idx).frame = t;
        Hulls(idx).centerOfMass = (stats(i).WeightedCentroid ./ imDiv-1) .* scaleFactor;
        Hulls(idx).track = -1;
        color = GetNextColor();
        Hulls(idx).color = color.background;
        bbStart = (stats(i).BoundingBox(1,1:3)./imDiv-1) .* scaleFactor;
        bbEnd = stats(i).BoundingBox(1,4:6) ./ [imageData.XDimension,imageData.YDimension,imageData.ZDimension];
        Hulls(idx).boundingBox = [bbStart, bbEnd];
        imDivArray = repmat(imDiv,size(stats(i).PixelList,1),1);
        imScaleArray = repmat(scaleFactor,size(stats(i).PixelList,1),1);
        Hulls(idx).pixels = (stats(i).PixelList./imDivArray-1) .* imScaleArray;
        Hulls(idx).faces = faces;
        imDivArray = repmat(imDiv,size(verts,1),1);
        imScaleArray = repmat(scaleFactor,size(verts,1),1);
        Hulls(idx).verts = (verts./imDivArray-1) .* imScaleArray;
        Hulls(idx).norms = norms;
    end
end

minCellDiaVox = minCellDia / imageDims(1) * scaleFactor(1);

if (~isempty(Hulls))
    [newHulls,numTracks,Costs] = trackerMex(imageData.NumberOfFrames,Hulls,minCellDiaVox,minCellDiaVox/2);
    
    Tracks(numTracks).startFrame = -1;
    Families(numTracks).startFrame = -1;
    
    for i=1:length(newHulls)
        Hulls(i).track = newHulls(i).track;
        Hulls(i).color = newHulls(i).color;
        
        if (Tracks(Hulls(i).track).hulls == -1)
            Tracks(Hulls(i).track).hulls = i;
        else
            Tracks(Hulls(i).track).hulls = [Tracks(Hulls(i).track).hulls i];
        end
    end
    
    for i=1:length(Tracks)
        Tracks(i).parent = -1;
        Tracks(i).childrenTracks = [-1 -1];
        Tracks(i).family = i;
        Tracks(i).color = Hulls(Tracks(i).hulls(1)).color;
        Tracks(i).startFrame = min([Hulls(Tracks(i).hulls).frame]);
        Tracks(i).endFrame = max([Hulls(Tracks(i).hulls).frame]);
        
        
        Families(i).tracks = i;
        Families(i).rootTrack = i;
        
        Families(i).startFrame = Tracks(i).startFrame;
        Families(i).endFrame = Tracks(i).endFrame;
    end
    
    ProcessNewborns();
    
    lever_3d('loadHulls',Hulls);
end


end