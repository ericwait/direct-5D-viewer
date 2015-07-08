function SegmentBlob(  handles, chan  )
%SEGMENTBLOB Summary of this function goes here
%   Detailed explanation goes here

global imageData segMetadata distMetadata processedMetadata Hulls Tracks Families

if (isempty(segMetadata))
    segMetadata.PathName = fullfile(imageData.imageDir,'Processed');
    segMetadata.FileName = [imageData.DatasetName '_segmentation.txt'];
    segMetadata.ChanProcessed = logical(zeros(1,imageData.NumberOfChannels));
    segMetadata.HullsChan = chan;%TODO this is prob not the right way to do this!
end
params = {'alpha','Opening Radius in X','Opening Radius in Y','Opening Radius in Z','Min Cell Diameter'};
diaTitle = 'Segmentation';
def = {'1.0', '2', '2', '1','6'};
response = inputdlg(params,diaTitle,1,def);
if (~isempty(response))
    hullChan = chan;
    alpha = str2double(response{1});
    oX = str2double(response{2});
    oY = str2double(response{3});
    oZ = str2double(response{4});
    dia = str2double(response{5});
    tic
    segImage = zeros(imageData.YDimension,imageData.XDimension,imageData.ZDimension,1,imageData.NumberOfFrames,'uint8');
    
    numCudaDevices = CudaMex('DeviceCount');
    
    if (numCudaDevices==1)
        lever_3d('takeControl');
    end
    
    if (get(handles.rb_Processed,'Value')==1 ...
            && exist(fullfile(processedMetadata.PathName,processedMetadata.FileName),'file')...
            && processedMetadata.ChanProcessed(chan))
        parfor t=1:imageData.NumberOfFrames
            segImage(:,:,:,1,t) = imageConvertNorm(CudaMex(...
                'Segment',tiffReader(fullfile(processedMetadata.PathName,processedMetadata.FileName),t,chan,[],[],true,true),...
                alpha,[oX,oY,oZ]),'uint8',true);
        end
    else
        parfor t=1:imageData.NumberOfFrames
            segImage(:,:,:,1,t) = imageConvertNorm(CudaMex(...
                'Segment',tiffReader(fullfile(imageData.imageDir,imageData.DatasetName),t,chan,[],[],true,true),...
                alpha,[oX,oY,oZ]),'uint8',true);
        end
    end
    
    if (numCudaDevices==1)
        lever_3d('releaseControl');
    end
    
    processTime = toc;
    fprintf('Image Processing Took: %s, or %s avg per frame\n',printTime(processTime),printTime(processTime/imageData.NumberOfFrames));
    tempData = imageData;
    tempData.DatasetName = [imageData.DatasetName,'_segmentation'];
    tiffWriter(segImage,segMetadata.PathName,tempData,[],chan);
    segMetadata.ChanProcessed(chan) = true;
    save(fullfile(segMetadata.PathName,'segMetadata.mat'),'segMetadata');
    Seg.Segment(segImage,chan,dia);
    
    if (~isempty(distMetadata))
        set(handles.m_DistanceChoice,'Enable','on');
    end
    
    clear segImage
    save(fullfile(segMetadata.PathName,[imageData.DatasetName '_Segmenation.mat']),'Hulls','Tracks','Families','-v7.3');
    msg = 'There are empty structures:';
    if isempty(Hulls)
        msg = [msg;{'Hulls'}];
    end
    if isempty(Tracks)
        msg = [msg;{'Tracks'}];
    end
    if isempty(Families)
        msg = [msg;{'Families'}];
    end
    
    if (size(msg,1)>1)
        warndlg(msg);
    end
end
end

