[im,imD] = MicroscopeData.Reader([],16);
D3d.Open(im,imD);
imS = im;
for t=1:imD.NumberOfFrames,for c=1:imD.NumberOfChannels,imS(:,:,:,c,t) = Cuda.CudaMex('ContrastEnhancement',im(:,:,:,c,t),[50,50,15],[3,3,3]);end;end
D3d.LoadImage(imS,imD,[],2);
global D3dUICtrlHandles

t = 1;
curIm = ImUtils.ConvertType(imS(:,:,:,1,t),'double',true);

[numBits,minVal,maxVal] = Utils.GetClassBits(curIm);

sliderMin = get(D3dUICtrlHandles.handles.s_floor,'value');
sliderMin = max(sliderMin,0.01);

imBW = curIm>sliderMin;
ImUtils.ThreeD.ShowMaxImage(imBW,1);

cc = bwconncomp(imBW);
rp = regionprops(cc,'Area','PixelList','Centroid');

hulls = [];
for i=1:length(rp)
    polygon  = D3d.Polygon.Make(rp(i).PixelList, length(hulls)+1, num2str(length(hulls)+1), t, [1,0,0], [3,3,1]);
    if (~isempty(polygon))
        polygon.CenterOfMass = rp(i).Centroid;
        hulls = [hulls,polygon];
    end
end
D3d.Viewer.DeleteAllPolygons();
D3d.Viewer.LoadPolygons(hulls);
D3d.UI.Ctrl.UpdateTime(t);

testHull = 5;
curSeg = rp(testHull);
gm = fitgmdist(curSeg.PixelList,2);
l = cluster(gm,curSeg.PixelList);
cell1.PixelList = curSeg.PixelList(l==1,:);
cell2.PixelList = curSeg.PixelList(l==2,:);

gmmList = testHull;
polygon  = D3d.Polygon.Make(cell1.PixelList, length(hulls)+1, num2str(length(hulls)+1), t, [1,0,0], [3,3,1]);
polygon.CenterOfMass = gm.mu(1,:);
polygon.color = [0,1,0];
hulls = [hulls,polygon];
gmmList = [gmmList,length(hulls)];

polygon  = D3d.Polygon.Make(cell2.PixelList, length(hulls)+1, num2str(length(hulls)+1), t, [1,0,0], [3,3,1]);
polygon.CenterOfMass = gm.mu(2,:);
polygon.color = [0,.5,1];
hulls = [hulls,polygon];
gmmList = [gmmList,length(hulls)];

[kIdx,C] = kmeans(curSeg.PixelList,2);
cell1.PixelList = curSeg.PixelList(kIdx==1,:);
cell2.PixelList = curSeg.PixelList(kIdx==2,:);

kList = testHull;
polygon  = D3d.Polygon.Make(cell1.PixelList, length(hulls)+1, num2str(length(hulls)+1), t, [1,0,0], [3,3,1]);
polygon.CenterOfMass = C(1,:);
polygon.color = [0,1,0];
hulls = [hulls,polygon];
kList = [kList,length(hulls)];

polygon  = D3d.Polygon.Make(cell2.PixelList, length(hulls)+1, num2str(length(hulls)+1), t, [1,0,0], [3,3,1]);
polygon.CenterOfMass = C(2,:);
polygon.color = [0,.5,1];
hulls = [hulls,polygon];
kList = [kList,length(hulls)];

D3d.Viewer.DeleteAllPolygons();
D3d.Viewer.LoadPolygons(hulls);

D3d.Viewer.DisplayPolygons(gmmList);
