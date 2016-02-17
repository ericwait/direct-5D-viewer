%% set the directory for the movie and image to go to
outputRoot = 'D:\Users\eric\Desktop';

%% open original image
[im,imD] = D3d.Open();

%% do some sort of processing here
imS = im;
for t=1:imD.NumberOfFrames
    for c=1:imD.NumberOfChannels
        imS(:,:,:,c,t) = Cuda.Mex('ContrastEnhancement',im(:,:,:,c,t),[75,75,25],[3,3,3]);
    end
end

%% load the processed image into the second buffer
D3d.LoadImage(imS,imD,[],2);

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
