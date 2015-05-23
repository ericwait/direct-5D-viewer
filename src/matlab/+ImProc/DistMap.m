function [ output_args ] = DistMap( input_args )
%DISTMAP Summary of this function goes here
%   Detailed explanation goes here
if (isempty(distMetadata))
    distMetadata.PathName = fullfile(orgMetadata.PathName,'Processed');
    distMetadata.FileName = [imageData.DatasetName '_distance.txt'];
    distMetadata.ChanProcessed = logical(zeros(1,imageData.NumberOfChannels));
end
params = {'alpha','Opening Radius in X','Opening Radius in Y','Opening Radius in Z'};
diaTitle = 'Distance Map';
def = {'1.0', '2', '2', '1'};
response = inputdlg(params,diaTitle,1,def);
if (~isempty(response))
    alpha = str2double(response{1});
    oX = str2double(response{2});
    oY = str2double(response{3});
    oZ = str2double(response{4});
    tic
    distanceImage = zeros(imageData.YDimension,imageData.XDimension,imageData.ZDimension,1,imageData.NumberOfFrames);
    if (get(handles.rb_Processed,'Value')==1  && ~isempty(processedMetadata)...
            && exist(fullfile(processedMetadata.PathName,processedMetadata.FileName),'file')...
            && processedMetadata.ChanProcessed(chan))
        parfor t=1:imageData.NumberOfFrames
            temp = CudaMex('Segment',tiffReader(fullfile(processedMetadata.PathName,processedMetadata.FileName),t,chan,[],[],true,true),...
                alpha,[oX,oY,oZ]);
            temp = temp>=max(temp(:));
            distanceImage(:,:,:,1,t) = bwdist(temp,'euclidean'); %TODO make this in Cuda for anisotropic images
        end
    else
        parfor t=1:imageData.NumberOfFrames
            temp = CudaMex('Segment',tiffReader(fullfile(orgMetadata.PathName,orgMetadata.FileName),t,chan,[],[],true,true),...
                alpha,[oX,oY,oZ]);
            temp = temp>=max(temp(:));
            distanceImage(:,:,:,1,t) = bwdist(temp,'euclidean'); %TODO make this in Cuda for anisotropic images
        end
    end
    processTime = toc;
    fprintf('Distance Map: %s, or %s avg per frame\n',printTime(processTime),printTime(processTime/imageData.NumberOfFrames));
    tempData = imageData;
    tempData.DatasetName = [imageData.DatasetName,'_distance'];
    tiffWriter(distanceImage,fullfile(distMetadata.PathName,tempData.DatasetName),tempData,[],chan);
    save(fullfile(distMetadata.PathName,'distMetadata.mat'),'distMetadata');
    distMetadata.ChanProcessed(chan) = true;
    if (~isempty(Hulls))
        SetDistances(distanceImage);
        set(handles.m_DistanceChoice,'Enable','on');
    end
    clear distanceImage
end
delete(gcp('nocreate'));

end

