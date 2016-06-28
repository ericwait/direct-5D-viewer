%% set the directory for the movie and image to go to
outputRoot = 'D:\Users\eric\Desktop';
smallThreshold = 25;

%% open original image
[im,imD] = D3d.Open();

%% do some sort of processing here
imS = im;
for t=1:imD.NumberOfFrames
    for c=1:imD.NumberOfChannels
        imS(:,:,:,c,t) = Cuda.ContrastEnhancement(im(:,:,:,c,t),[75,75,25],[3,3,3],1);
    end
end

%% load the processed image into the second buffer
D3d.LoadImage(imS,imD,[],2);

%% create segmenation results for each channel
[~,~,maxVal] = Utils.GetClassBits(im);
polygons = cell(imD.NumberOfChannels,1);
for t=1:imD.NumberOfFrames
    for c=1:imD.NumberOfChannels
        curIm = imS(:,:,:,c,t);
        level = graythresh(curIm(curIm>0));
        imBW = curIm > level * 0.8 * maxVal;
        imBW = Cuda.MaxFilterEllipsoid(im2uint8(imBW),[3,3,2],1);
        imBW = Cuda.MinFilterEllipsoid(imBW,[3,3,2],1);
        imBW = imBW>0;
        rp = regionprops(imBW,'Area','PixelList');
        for i=1:length(rp)
            if (rp(i).Area>smallThreshold)
                curPoly = D3d.Polygon.Make(rp(i).PixelList,1,'1',t);
                if (~isempty(curPoly))
                    polygons{c} = [polygons{c},curPoly];
                end
            end
        end
    end
end

%% clean up the polygons
parfor c=1:imD.NumberOfChannels
    curPolys = polygons{c};
    if (isfield(imD,'ChannelColors'))
        curColor = 1 - imD.ChannelColors(c,:);
    else
        %do something for each channel or each object here
    end
    for i=1:length(curPolys)
        curPolys(i).color = curColor;
        curPolys(i).index = i + c*1000;
        curPolys(i).label = num2str(curPolys(i).index);
    end
    polygons{c} = curPolys;
end

%% Load the segmenation and show individual channels
for c=1:imD.NumberOfChannels
    D3d.Viewer('loadPolygons',polygons{c});
end

% View just a channel's polygons
%D3d.Viewer('displayPolygons',1001:2000);%shows the first channel polygons
%D3d.Viewer('displayPolygons',2001:5000);%shows the 2nd through 5th channel polygons

%% capture frames for animation
% ******************************************
% did you set the view the way you want it?
% ******************************************

%% setup the directory to save the window captures to
outputPath = fullfile(outputRoot,imD.DatasetName);
if (exist(outputPath,'dir'))
    % clear out the dirctory so that the movie only has the current captures
    rmdir(outputPath,'s');
end
mkdir(outputPath);

% tell the viewer where to put the screen shots
D3d.Viewer('setCapturePath',outputPath,imD.DatasetName);

%% run some sort of animation
for i=1:720
    D3d.Viewer('setRotation',-0.5,0);
    D3d.Viewer('captureWindow');
end

%% get a list of the files captured
dList = dir(fullfile(outputPath,'*.bmp'));
fNames = cellfun(@(x)(fullfile(outputPath,x)),{dList.name},'uniformOutput',false);

%% make the movie
MovieUtils.MakeFromFileNames(fNames,60,fullfile(outputPath,[imD.DatasetName '.mp4']));

%% If you have the ffmpeg toolbox installed from http://www.mathworks.com/matlabcentral/fileexchange/42296-ffmpeg-toolbox
% tokenString = regexptranslate('escape',fullfile(outputPath,imD.DatasetName));
% imageNumbers = regexpi(fNames,[tokenString, '_(\d+)\.bmp'],'tokens','once');
% imageNumbers = cellfun(@(x)(str2double(x)),imageNumbers);
% range = [min(imageNumbers),max(imageNumbers)];
% ffmpegimages2video(fullfile(outputPath,[imD.DatasetName, '_%05d.bmp']),...
%     fullfile(outputPath,[imD.DatasetName,'_5fps.mp4']),...
%     'InputFrameRate',60,...
%     'InputStartNumber',range,...
%     'x264Preset','veryslow',...
%     'x264Tune','stillimage',...
%     'OutputFrameRate',60);
