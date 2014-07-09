function SetDistances(chan)
global Hulls distanceImage imageData

for i=1:length(Hulls)
    if (~isempty(Hulls(i).pixelsOrg))
        idx = sub2ind([imageData.YDimension,imageData.XDimension,imageData.ZDimension,imageData.NumberOfChannels,imageData.NumberOfFrames],...
            Hulls(i).pixelsOrg(:,2),Hulls(i).pixelsOrg(:,1),Hulls(i).pixelsOrg(:,3),repmat(chan,size(Hulls(i).pixelsOrg,1),1),...
            repmat(Hulls(i).frame,size(Hulls(i).pixelsOrg,1),1));
        distances = distanceImage(idx);
        Hulls(i).distances(1) = max(distances(:));
        Hulls(i).distances(2) = min(distances(:));
        Hulls(i).distances(3) = distanceImage(round(Hulls(i).centerOfMassOrg(1)),round(Hulls(i).centerOfMassOrg(2)),...
            round(Hulls(i).centerOfMassOrg(3)),chan,Hulls(i).frame);
    end
end
end

