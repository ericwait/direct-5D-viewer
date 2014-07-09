function SplitHull(hull, k)
global Hulls Tracks Families orgImage imageData hullChan trackHulls familyHulls selectedHull uiControlHandles

gmoptions = statset('Display','off', 'MaxIter',400);
obj = gmdistribution.fit(Hulls(hull).pixelsOrg, k, 'Replicates',15, 'Options',gmoptions);
kIdx = cluster(obj, Hulls(hull).pixelsOrg);

im = zeros(imageData.XDimension, imageData.YDimension, imageData.ZDimension, 'uint8');
for i=1:k
    idx = sub2ind([imageData.YDimension, imageData.XDimension, imageData.ZDimension],Hulls(hull).pixelsOrg(kIdx==i,2),...
        Hulls(hull).pixelsOrg(kIdx==i,1),Hulls(hull).pixelsOrg(kIdx==i,3));
    im(idx) = i;
end

newHulls = CreateHulls(im,orgImage(:,:,:,hullChan,Hulls(hull).frame),1,Hulls(hull).frame);

oldHull = Hulls(hull);
Hulls(hull) = newHulls(1);
Hulls(hull).label = oldHull.label;
Hulls(hull).color = oldHull.color;
Hulls(hull).track = oldHull.track;

newHullIdx = [];
for i=2:length(newHulls)
    Hulls(end+1) = newHulls(i);
    Hulls(end).label = length(Hulls);
    Hulls(end).track = length(Tracks) +1;
    newHullIdx = [newHullIdx length(Hulls)];
    
    Tracks(end+1).hulls = length(Hulls);
    Tracks(end).color = Hulls(end).color;
    Tracks(end).startFrame = Hulls(end).frame;
    Tracks(end).endFrame = Hulls(end).frame;
    Tracks(end).family = length(Families)+1;
    
    Families(end+1).tracks = length(Tracks);
    Families(end).rootTrack = length(Tracks);
    Families(end).startFrame = Hulls(end).frame;
    Families(end).endFrame = Hulls(end).frame;
end

lever_3d('updateHulls',Hulls(hull));
lever_3d('addHulls',Hulls(newHullIdx));

[trackHulls, familyHulls] = GetHullsToDisplay(selectedHull);
if (get(uiControlHandles.cb_ShowFamily,'Value')==1)
    lever_3d('displayHulls',familyHulls);
else
    lever_3d('displayHulls',trackHulls);
end
end

