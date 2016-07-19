cmap = hsv(K+1);
cmap = cmap(2:end,:);

[bwSmall,shiftCoords_rcz] = ImUtils.ROI.MakeSubImBW(size(bw),find(bw),2);
ptsShift_rc = cellfun(@(x)(x-repmat(shiftCoords_rcz,size(x,1),1)+1),pts_rc,'uniformOutput',false);
indShift = find(bwSmall);

imD = MicroscopeData.GetEmptyMetadata();
imD.Dimensions = SwapXY_RC(size(bwSmall));
imD.NumberOfChannels = K*2;
imD.NumberOfFrames = 1;
imD.PixelPhysicalSize = ones(1,3);
imD.ChannelColors = vertcat(cmap,repmat([1,0,0],K,1));

for i=1:K
    imD.ChannelNames{i,1} = sprintf('Poly %d',i);
    imD.ChannelNames{i+K,1} = sprintf('Error for %d',i);
end

figure
rpCluster = [];
rpTrue = [];
for i=1:K
    colr = cmap(i,:);
    
    curInd = indShift(idx==i);
    curIm = zeros(size(bwSmall),'uint8');
    curIm(curInd) = 255;
    h = subplot(2,K,i);
    ImUtils.ThreeD.ShowMaxImage(curIm,false,[],h);
    rpC = regionprops(curIm>0,'Centroid','PixelIdxList');
    rpCluster = [rpCluster,rpC];
    
    curInd = Utils.CoordToInd(size(bwSmall),ptsShift_rc{i});
    curIm = false(size(bwSmall));
    curIm(curInd) = true;
    h = subplot(2,K,i+K);
    ImUtils.ThreeD.ShowMaxImage(curIm,false,[],h);
    rpT = regionprops(curIm,'Centroid','PixelIdxList');
    rpTrue = [rpTrue,rpT];
end
clear curIm

dst = ones(K)*inf;
for i=1:K
    dst(i,:) = sqrt(sum((repmat(rpTrue(i).Centroid,K,1) - vertcat(rpCluster.Centroid)).^2,2))';
end

[vals,I] = min(dst,[],2);
if (numel(unique(I))==K)
    mapping = I;
else
    newDst = dst;
    mapping = zeros(K,1);
    for i=1:K
        if (I(i)~=0)
            sameIdx = I==I(i);
            % is there more than one good canidate
            if (nnz(sameIdx)==1)
                j = find(sameIdx);
                mapping(j) = I(i);
                newDst(i,:) = inf;
                newDst(:,j) = inf;
            else
                minVal = min(vals(sameIdx));
                j = find(vals==minVal);
                mapping(j) = I(i);
                newDst(i,:) = inf;
                newDst(:,j) = inf;
                sameIdx(j) = 0;
                [vals,I] = min(newDst,[],2);
            end
        end
        I(mapping~=0) = 0;
    end
end

%% 
figure
polygons = [];
imC = zeros([size(bwSmall),K*2],'uint8');
for i=1:K
    j = mapping(i);
    polygons = [polygons,D3d.Polygon.Make(Utils.SwapXY_RC(ptsShift_rc{i}),i,num2str(i),1,[cmap(i,:),1])];
    
    curInd = intersect(rpCluster(j).PixelIdxList,rpTrue(i).PixelIdxList);
    curIm = zeros(size(bwSmall),'uint8');
    curIm(curInd) = 255;
    h = subplot(3,K,i);
    ImUtils.ThreeD.ShowMaxImage(curIm,false,[],h);
    imC(:,:,:,i) = curIm;
    
    curInd = setdiff(rpCluster(j).PixelIdxList,rpTrue(i).PixelIdxList);
    curIm = zeros(size(bwSmall),'uint8');
    curIm(curInd) = 255;
    h = subplot(3,K,i+K);
    ImUtils.ThreeD.ShowMaxImage(curIm,false,[],h);
    imC(:,:,:,i+K) = curIm;
    
    curInd = Utils.CoordToInd(size(bwSmall),ptsShift_rc{i});
    curIm = false(size(bwSmall));
    curIm(curInd) = true;
    h = subplot(3,K,i+2*K);
    ImUtils.ThreeD.ShowMaxImage(curIm,false,[],h);
end

D3d.LoadImage(imC,imD);
D3d.Viewer.DeleteAllPolygons();
D3d.Viewer.AddPolygons(polygons);
D3d.Viewer.SetWindowSize(640,1080);

%% Capture all texture
D3d.Viewer.SetBackgroundColor([0,0,0]);
D3d.Viewer.SetCapturePath(fullfile('.','Texture'),'Texture');
if (exist(fullfile('.','Texture'),'dir'))
    rmdir(fullfile('.','Texture'),'s');
end
mkdir(fullfile('.','Texture'));
for t=1:720
    D3d.Viewer.CaptureWindow();
    D3d.Viewer.SetRotation(0,0.5,0);
end

%% Capture ground truth poly
D3d.Viewer.SetCapturePath(fullfile('.','Poly'),'Poly');
if (exist(fullfile('.','Poly'),'dir'))
    rmdir(fullfile('.','Poly'),'s');
end
mkdir(fullfile('.','Poly'));
for t=1:720
    D3d.Viewer.CaptureWindow();
    D3d.Viewer.SetRotation(0,0.5,0);
end

%% Capture each CC
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Set this
i = 1;
%for i=1:K

ccWidth = 640/2;
ccHeight = 1080/2;
D3d.Viewer.SetWindowSize(ccWidth,ccHeight);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
D3d.Viewer.SetViewOrigin(polygons(i).CenterOfMass);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Set view
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
ccFolder = sprintf('cc%d',i);
D3d.Viewer.SetCapturePath(fullfile('.',ccFolder),ccFolder);
if (exist(fullfile('.',ccFolder),'dir'))
    rmdir(fullfile('.',ccFolder),'s');
end
mkdir(fullfile('.',ccFolder));
for t=1:720
    D3d.Viewer.CaptureWindow();
    D3d.Viewer.SetRotation(0,0.5,0);
end

%%
folderOut = fullfile('.','Texture');
dList = dir(fullfile(folderOut,'*.bmp'));
fNamesC = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile('.','Poly');
dList = dir(fullfile(folderOut,'*.bmp'));
fNamesLH = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile('.','cc1');
dList = dir(fullfile(folderOut,'*.bmp'));
fNamesRHT = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile('.','cc2');
dList = dir(fullfile(folderOut,'*.bmp'));
fNamesRHC = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile('.','cc3');
dList = dir(fullfile(folderOut,'*.bmp'));
fNamesRHB = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile('.','cc4');
dList = dir(fullfile(folderOut,'*.bmp'));
fNamesRHB1 = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

movieOut = fullfile('.','CombFrames');
if (exist(movieOut,'dir'))
    rmdir(movieOut,'s');
end
mkdir(movieOut);

for t=1:min([length(fNamesC),length(fNamesLH),length(fNamesRHT),length(fNamesRHC),length(fNamesRHB)])
    imC = imread(fNamesC{t});
    imLH = imread(fNamesLH{t});
    
    im = cat(2,imLH,imC);
    cent = round(size(im,2)/2);
    im(:,cent-2:cent+2,:) = 255;
    
    imRT = imread(fNamesRHT{t});
    imRC = imread(fNamesRHC{t});
    imRB = imread(fNamesRHB{t});
    imRB1 = imread(fNamesRHB1{t});
    
    imR = cat(1,imRT,imRC);
    cent2 = round(size(imR,1)/2);
    imR(cent2-2:cent2+2,:,:) = 255;
    
    imR2 = cat(1,imRB,imRB1);
    cent2 = round(size(imR2,1)/2);
    imR2(cent2-2:cent2+2,:,:) = 255;
    
    imR = cat(2,imR,imR2);
    cent2 = round(size(imR,2)/2);
    imR(:,cent2-2:cent2+2,:) = 255;
%     imR = cat(1,imR,imRB);
%     imR(end-cent2-2:end-cent2+2,:,:) = 255;
    
    im = cat(2,im,imR);
    im(:,end-cent-2:end-cent+2,:) = 255;
    im = imresize(im,[1080,1920]);
    imwrite(im,fullfile(movieOut,sprintf('t%04d.tif',t)),'compression','lzw');
end

range = [1,min([length(fNamesC),length(fNamesLH),length(fNamesRHT),length(fNamesRHC),length(fNamesRHB)])];

ffmpegimages2video(fullfile(movieOut,'t%04d.tif'),...
    fullfile(movieOut,'compare.mp4'),...
    'InputFrameRate',60,...
    'InputStartNumber',range,...
    'x264Preset','veryslow',...
    'x264Tune','stillimage',...
    'OutputFrameRate',60);
