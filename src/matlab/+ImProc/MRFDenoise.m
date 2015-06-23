function [ processed ] = MRFDenoise( handles, chan )
%MRFDENOISE Summary of this function goes here
%   Detailed explanation goes here
global imageData processedMetadata

processed = false;

if (isempty(processedMetadata))
    processedMetadata.PathName = fullfile(imageData.imageDir,'Processed');
    processedMetadata.FileName = [imageData.DatasetName '_processed.txt'];
end

%% setup question dialog
params = {'Max iterations'};
diaTitle = 'Denoise';
def = {'100'};
response = inputdlg(params,diaTitle,1,def);

if (~isempty(response))
    %% parse the response
    iter = str2num(response{1});
    
    % check if there is current data to process or should the orginal data
    % be used
    if (get(handles.rb_Processed,'Value')==1 ...
            && exist(fullfile(processedMetadata.PathName,processedMetadata.FileName),'file'))
        
        pth = processedMetadata.Path;
        fName = processedMetadata.FileName;
    else
        % use org images
        pth = imageData.imageDir;
        fName = imageData.DatasetName;
    end
    
    %% process the image
    % turn off rendering if there is only one GPU
    numCudaDevices = CudaMex('DeviceCount');
    if (numCudaDevices==1)
        lever_3d('takeControl');
    end
    
    tic
    processedImage = zeros(imageData.YDimension,imageData.XDimension,imageData.ZDimension,...
        imageData.NumberOfChannels,imageData.NumberOfFrames,imageData.Type);
    parfor t=1:imageData.NumberOfFrames
        processedImage(:,:,:,chan,t) = CudaMex(...
            'MarkovRandomFieldDenoiser',tiffReader(fullfile(pth,fName),t,chan,[],'single',true,true),iter);
    end
    
    % turn the renderer back on
    if (numCudaDevices==1)
        lever_3d('releaseControl');
    end
    
    processTime = toc;
    fprintf('Markov Random Fields Denoise: %s, or %s avg per frame\n',printTime(processTime),printTime(processTime/imageData.NumberOfFrames));
    
    %% save out processed data
    tempData = imageData;
    tempData.DatasetName = [imageData.DatasetName,'_processed'];
    
    if (get(handles.rb_Processed,'Value')==0 ...
            && exist(fullfile(processedMetadata.PathName,processedMetadata.FileName),'file'))
        
        % fill in the other channels with the previously processed images
        pth = processedMetadata.Path;
        fName = processedMetadata.FileName;
    end
    
    for c=1:imageData.NumberOfChannels
        if (c~=chan)
            processedImage(:,:,:,c,:) = tiffReader(fullfile(pth,fName),[],c,[],[],true,true);
        end
    end
    
    tiffWriter(processedImage,processedMetadata.PathName,tempData);
    processed = true;
    
    save(fullfile(processedMetadata.PathName,'processedMetadata.mat'),'processedMetadata');
    
    %% update the renderer
    Messaging.UpdateTexture(processedImage,false);
    
    updateCurrentState(handles);
end
end

