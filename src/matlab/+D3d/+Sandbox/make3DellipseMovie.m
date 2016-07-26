%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Make the data fit a bit tighter so
% the viewer doesn't have to work as hard
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

cmap = hsv(K+1);
cmap = cmap(2:end,:);

[bwSmall,shiftCoords_rcz] = ImUtils.ROI.MakeSubImBW(size(bw),find(bw),2);
ptsShift_rc = cellfun(@(x)(x-repmat(shiftCoords_rcz,size(x,1),1)+1),pts_rc,'uniformOutput',false);
indShift = find(bwSmall);

imD = MicroscopeData.GetEmptyMetadata();
imD.Dimensions = Utils.SwapXY_RC(size(bwSmall));
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
    if (~isempty(rpC))
        rpCluster = [rpCluster,rpC(1)];
    end
    
    curInd = Utils.CoordToInd(size(bwSmall),ptsShift_rc{i});
    curIm = false(size(bwSmall));
    curIm(curInd) = true;
    h = subplot(2,K,i+K);
    ImUtils.ThreeD.ShowMaxImage(curIm,false,[],h);
    rpT = regionprops(curIm,'Centroid','PixelIdxList');
    rpTrue = [rpTrue,rpT];
end
clear curIm

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Make Ploygon and texture data
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure
polygons = [];
imTex = zeros([size(bwSmall),K*2],'uint8');
for i=1:K
    polygons = [polygons,D3d.Polygon.Make(Utils.SwapXY_RC(ptsShift_rc{i}),i,num2str(i),1,[cmap(i,:),1])];
    
    curIm = zeros(size(bwSmall),'uint8');
    if (length(rpCluster)>=i)
        curInd = intersect(rpCluster(i).PixelIdxList,rpTrue(i).PixelIdxList);
        curIm(curInd) = 255;
    end
    h = subplot(3,K,i);
    ImUtils.ThreeD.ShowMaxImage(curIm,false,[],h);
    title('Cluster result');
    imTex(:,:,:,i) = curIm;
    
    curIm = zeros(size(bwSmall),'uint8');
    if (length(rpCluster)>=i)
        curInd = setdiff(rpCluster(i).PixelIdxList,rpTrue(i).PixelIdxList);
        curIm(curInd) = 255;
    end
    h = subplot(3,K,i+K);
    ImUtils.ThreeD.ShowMaxImage(curIm,false,[],h);
    title('Error');
    imTex(:,:,:,i+K) = curIm;
    
    curInd = Utils.CoordToInd(size(bwSmall),ptsShift_rc{i});
    curIm = false(size(bwSmall));
    curIm(curInd) = true;
    h = subplot(3,K,i+2*K);
    ImUtils.ThreeD.ShowMaxImage(curIm,false,[],h);
    title('Ground truth');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Load and set viewer
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

D3d.LoadImage(imTex,imD);
D3d.Viewer.DeleteAllPolygons();
D3d.Viewer.AddPolygons(polygons);
if (K>3)
    mainHeight = 2;
else
    mainHeight = 1;
end
D3d.Viewer.SetWindowSize(1920/3,1080/mainHeight);
D3d.Viewer.ResetView();
[imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();
D3d.Viewer.ShowLabels(false);
D3d.Viewer.ShowAllPolygons(false);
D3d.Viewer.TextureAttenuation(true);
D3d.Viewer.TextureLighting(true);

for c=1:K
    channelData(c).visible = 1;
    channelData(c+K).visible = 1;
end
D3d.UI.Ctrl.SetUserData(imageData,colors,channelData);
D3d.UI.Ctrl.UpdateCurrentState();

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Capture all clustering
% Set Zoom before running
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Texture capture
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Capture Ground Truth
% Gound Truth is in the polygons
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for c=1:K
    channelData(c).visible = 0;
    channelData(c+K).visible = 0;
end
D3d.UI.Ctrl.SetUserData(imageData,colors,channelData);
D3d.UI.Ctrl.UpdateCurrentState();
D3d.Viewer.ShowAllPolygons(true);
D3d.Viewer.ShowLabels(false);
D3d.Viewer.ToggleWireframe(false);
D3d.Viewer.TextureAttenuation(false);
D3d.Viewer.TextureLighting(false);

D3d.Viewer.SetCapturePath(fullfile('.','Poly'),'Poly');
if (exist(fullfile('.','Poly'),'dir'))
    rmdir(fullfile('.','Poly'),'s');
end
mkdir(fullfile('.','Poly'));
for t=1:720
    D3d.Viewer.CaptureWindow();
    D3d.Viewer.SetRotation(0,0.5,0);
end

% get viewer set for the individual components
ccWidth = 1920/3;
if (K==2 || K==4)
    ccHeight = 1080/2;
elseif (K==3)
    ccHeight = 1080/3;
end
D3d.Viewer.SetWindowSize(ccWidth,ccHeight);
D3d.Viewer.ShowLabels(false);
D3d.Viewer.ShowAllPolygons(false);
D3d.Viewer.TextureAttenuation(true);
D3d.Viewer.TextureLighting(true);

i = 0;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Set view for each CC
% run this and next section for each cc
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for c=1:K
    channelData(c).visible = 0;
    channelData(c+K).visible = 0;
end
i = i+1;
channelData(i).visible = 1;
channelData(i+K).visible = 1;
D3d.UI.Ctrl.SetUserData(imageData,colors,channelData);
D3d.UI.Ctrl.UpdateCurrentState();
D3d.Viewer.ResetView();
D3d.Viewer.SetViewOrigin(polygons(i).CenterOfMass);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Capture current CC
%  Adjust zoom before running
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Make composite frames for a 1080p movie
% Ensure that the viewer is done rendering
% Set the dir that this movie and frames are saved to
%outDir = 'CompFrames';%default
%outDir = 'TopLeft';% Top Left pannel for 4k
%outDir = 'TopRight';% Top Right pannel for 4k
%outDir = 'BottomLeft';% Bottom Left pannel for 4k
%outDir = 'BottomRight';% Bottom Right pannel for 4k
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
movieOut = fullfile('.',outDir);
if (exist(movieOut,'dir'))
    rmdir(movieOut,'s');
end
mkdir(movieOut);

folderOut = fullfile('.','Texture');
dList = dir(fullfile(folderOut,'*.bmp'));
fNamesTex = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile('.','Poly');
dList = dir(fullfile(folderOut,'*.bmp'));
fNamesTR = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

if (K>1)
    folderOut = fullfile('.','cc1');
    dList = dir(fullfile(folderOut,'*.bmp'));
    fNamesCC1 = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);
    
    folderOut = fullfile('.','cc2');
    dList = dir(fullfile(folderOut,'*.bmp'));
    fNamesCC2 = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);
end
if (K>2)
    folderOut = fullfile('.','cc3');
    dList = dir(fullfile(folderOut,'*.bmp'));
    fNamesCC3 = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);
end
if (K>3)
    folderOut = fullfile('.','cc4');
    dList = dir(fullfile(folderOut,'*.bmp'));
    fNamesCC4 = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);
end
if (K>4)
    folderOut = fullfile('.','cc5');
    dList = dir(fullfile(folderOut,'*.bmp'));
    fNamesCC5 = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);
end

prgs = Utils.CmdlnProgress(min([length(fNamesTex),length(fNamesTR)]),true,'Making Frames');
for t=1:min([length(fNamesTex),length(fNamesTR)])
    imTex = imread(fNamesTex{t});
    imPol = imread(fNamesTR{t});
    
    im = cat(2,imPol,imTex);
    cent = round(size(im,2)/2);
    im(:,cent-2:cent+2,:) = 255;
    
    if (K>1)
        imCC1 = imread(fNamesCC1{t});
        imCC2 = imread(fNamesCC2{t});
    end
    if (K>2)
        imCC3 = imread(fNamesCC3{t});
    end
    if (K>3)
        imCC4 = imread(fNamesCC4{t});
    end
    if (K>4)
        imCC5 = imread(fNamesCC5{t});
    end
    
    if (K>4)
        imB = cat(2,imCC4,imCC5);
        cent = round(size(imB,2)/2);
        imB(:,cent-2:cent+2,:) = 255;
    elseif (K==4)
        imB = cat(2,imCC4,imCC3);
        cent = round(size(imB,2)/2);
        imB(:,cent-2:cent+2,:) = 255;
    end
    
    if (K>3)
        im = cat(1,im,imB);
        cent = round(size(im,1)/2);
        im(cent-2:cent+2,:,:) = 255;
    end
    
    imR = cat(1,imCC1,imCC2);
    cent = round(size(imR,1)/2);
    imR(cent-2:cent+2,:,:) = 255;
    
    if (K==3)
        imR = cat(1,imR,imCC3);
        imR(end-cent-2:end-cent+2,:,:) = 255;
    end
    
    im = cat(2,im,imR);
    cent = size(imR,2);
    im(:,end-cent-2:end-cent+2,:) = 255;
    
    im = imresize(im,[1080,1920]);
    imwrite(im,fullfile(movieOut,sprintf('t%04d.tif',t)),'compression','lzw');
    
    prgs.PrintProgress(t);
end
prgs.ClearProgress(true);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Create a mp4
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
range = [1,min([length(fNamesTex),length(fNamesTR)])];

fprintf('Making movie...');
tic
ffmpegimages2video(fullfile(movieOut,'t%04d.tif'),...
    fullfile(movieOut,'compare.mp4'),...
    'InputFrameRate',60,...
    'InputStartNumber',range,...
    'x264Preset','veryslow',...
    'x264Tune','stillimage',...
    'OutputFrameRate',60);
fprintf('%s\n',Utils.PrintTime(toc))

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Composite previous runs into 4k
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
movieOut = fullfile('.','4kCompFrames');
if (exist(movieOut,'dir'))
    rmdir(movieOut,'s');
end
mkdir(movieOut);

folderOut = fullfile('.','TopLeft');
dList = dir(fullfile(folderOut,'*.tif'));
fNamesTL = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile('.','TopRight');
dList = dir(fullfile(folderOut,'*.tif'));
fNamesTR = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile('.','BottomLeft');
dList = dir(fullfile(folderOut,'*.tif'));
fNamesBL = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile('.','BottomRight');
dList = dir(fullfile(folderOut,'*.tif'));
fNamesBR = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

prgs = Utils.CmdlnProgress(min([length(fNamesTex),length(fNamesTR)]),true,'Making Frames');
for t=2:2:min([length(fNamesTL),length(fNamesTR),length(fNamesBL),length(fNamesBR)])
    imTL = imread(fNamesTL{t});
    imTR = imread(fNamesTR{t});
    imBL = imread(fNamesBL{t});
    imBR = imread(fNamesBR{t});
    
    imT = cat(2,imTL,imTR);
    cent = round(size(imT,2)/2);
    imT(:,cent-4:cent+4,:) = 255;
    
    imB = cat(2,imBL,imBR);
    cent = round(size(imB,2)/2);
    imB(:,cent-4:cent+4,:) = 255;
    
    im = cat(1,imT,imB);
    cent = round(size(im,1)/2);
    im(cent-4:cent+4,:,:) = 255;

    im = imresize(im,[2160,3840]);
    imwrite(im,fullfile(movieOut,sprintf('t%04d.tif',t/2)),'compression','lzw');
    
    prgs.PrintProgress(t);
end
prgs.ClearProgress(true);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Create 4k mp4
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
range = [1,length(fNamesTL)];

fprintf('Making movie...');
tic
ffmpegimages2video(fullfile(movieOut,'t%04d.tif'),...
    fullfile(movieOut,'compare4k.mp4'),...
    'InputFrameRate',30,...
    'InputStartNumber',range,...
    'x264Preset','veryslow',...
    'x264Tune','stillimage',...
    'OutputFrameRate',30);
fprintf('%s\n',Utils.PrintTime(toc))
