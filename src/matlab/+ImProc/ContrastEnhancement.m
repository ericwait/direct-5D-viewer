function [ processed ] = ContrastEnhancement( handles, chan )
%CONTRASTENHANCEMENT Summary of this function goes here
%   Detailed explanation goes here
global imageData processedMetadata

processed = false;

if (isempty(processedMetadata))
    processedMetadata.PathName = fullfile(imageData.imageDir,'Processed');
    processedMetadata.FileName = [imageData.DatasetName '_processed'];
end

%% setup question dialog
params = {'Gaussian Sigma in X','Gaussian Sigma in Y', 'Gaussian Sigma in Z', 'Median Filter in X', 'Median Filter in Y','Median Filter in Z'};
diaTitle = 'Contrast Enhancement';
def = {'100', '100', '100', '3', '3', '3'};
response = inputdlg(params,diaTitle,1,def);

if (~isempty(response))
    %% parse the response
    gX = str2double(response{1});
    gY = str2double(response{2});
    gZ = str2double(response{3});
    mX = str2double(response{4});
    mY = str2double(response{5});
    mZ = str2double(response{6});
    
    % check if there is current data to process or should the orginal data
    % be used
    if (get(handles.rb_Processed,'Value')==1 ...
            && exist(fullfile(processedMetadata.PathName,processedMetadata.FileName),'file'))
        
        pth = processedMetadata.PathName;
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
            'ContrastEnhancement',tiffReader(fullfile(pth,fName),t,chan,[],[],true,true),...
            [gX,gY,gZ],[mX,mY,mZ]);
    end
    
    % turn the renderer back on
    if (numCudaDevices==1)
        lever_3d('releaseControl');
    end
    
    processTime = toc;
    fprintf('Contrast Enhancement took: %s, or %s avg per frame\n',printTime(processTime),printTime(processTime/size(processedImage,5)));
    
    %% save out processed data
    tempData = imageData;
    tempData.DatasetName = [imageData.DatasetName,'_processed'];
    
    if (get(handles.rb_Processed,'Value')==0 ...
            && exist(fullfile(processedMetadata.PathName,processedMetadata.FileName),'file'))
        
        % fill in the other channels with the previously processed images
        pth = processedMetadata.PathName;
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
end
end

