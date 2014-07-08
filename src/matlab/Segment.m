function Segment(chan, minCellDia)
global imageData orgImage segImage Hulls Tracks Families Costs hullChan
[Hulls,Tracks,Families] = GetEmptyStructs();

hullChan = chan;

for t=1:size(segImage,5)
    bw = segImage(:,:,:,chan,t)>0;
    Hulls = [Hulls CreateHulls(bw,orgImage(:,:,:,chan,t),minCellDia,t)];
end

for i=1:length(Hulls)
    Hulls(i).label = i;
end

imageDims = [imageData.XDimension, imageData.YDimension, imageData.ZDimension];
physDims = [imageData.XPixelPhysicalSize, imageData.YPixelPhysicalSize, imageData.ZPixelPhysicalSize];
scaleFactor  =  imageDims ./ max(imageDims) .* physDims/physDims(1);
minCellDiaVox = minCellDia / imageDims(1) * scaleFactor(1);

if (~isempty(Hulls))
    [newHulls,numTracks,Costs] = trackerMex(imageData.NumberOfFrames,Hulls,minCellDiaVox*6,minCellDiaVox*2);
    
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
        Tracks(i).childrenTracks = [];
        Tracks(i).family = i;
        Tracks(i).color = Hulls(Tracks(i).hulls(1)).color;
        Tracks(i).startFrame = min([Hulls(Tracks(i).hulls).frame]);
        Tracks(i).endFrame = max([Hulls(Tracks(i).hulls).frame]);
        RehashTracksHulls(i);
        
        Families(i).tracks = i;
        Families(i).rootTrack = i;
        
        Families(i).startFrame = Tracks(i).startFrame;
        Families(i).endFrame = Tracks(i).endFrame;
    end
    
    ProcessNewborns(minCellDiaVox*6,minCellDiaVox*2);
    
    lever_3d('loadHulls',Hulls);
    DrawTree(2);
end
end
