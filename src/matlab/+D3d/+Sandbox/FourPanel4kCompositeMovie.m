function FourPanel4kCompositeMovie(rootDir)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Composite previous runs into 4k
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
movieOut = fullfile(rootDir,'4kCompFrames');
if (exist(movieOut,'dir'))
    rmdir(movieOut,'s');
end
mkdir(movieOut);

folderOut = fullfile(rootDir,'TopLeft');
dList = dir(fullfile(folderOut,'*.tif'));
fNamesTL = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile(rootDir,'TopRight');
dList = dir(fullfile(folderOut,'*.tif'));
fNamesTR = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile(rootDir,'BottomLeft');
dList = dir(fullfile(folderOut,'*.tif'));
fNamesBL = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

folderOut = fullfile(rootDir,'BottomRight');
dList = dir(fullfile(folderOut,'*.tif'));
fNamesBR = cellfun(@(x)(fullfile(folderOut,x)),{dList.name},'uniformOutput',false);

prgs = Utils.CmdlnProgress(min([length(fNamesTL),length(fNamesTR),length(fNamesBL),length(fNamesBR)]),true,'Making Frames');
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
end
