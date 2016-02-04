function [im,imData] = LoadImage( im, imData, imagePath, bufferNum )
%LOADIMAGE [varargout] = D3d.LoadImage( im, imData, imagePath, bufferNum )
%sends the image data to the directX viewer.
%
%IM is an 'uint8' image 3D-5D image matrix, optional.
%IM_DATA is astructure that holds metadata for the image and
%   needs to be in the format that tiffReader would return, optional.
%IMAGE_PATH is a path to the metadata file that can be read by
%   tiffReader, optional.

global D3dIsOpen

if (~D3dIsOpen)
    [im,imData] = D3d.Open(im, imData, imagePath);
    return
end

%% check the optional arguments and set the non-existant ones to empty
if (~exist('im','var'))
    im = [];
end
if (~exist('imData','var'))
    imData = [];
end
if (~exist('imagePath','var'))
    imagePath = [];
end
if (~exist('bufferNum','var') || isempty(bufferNum))
    bufferNum = 1;
else
    if (~bufferNum==1 && ~bufferNum==2)
        error('bufferType can only be 1 or 2!');
    end
end

bufferType = 'original';
if (bufferNum==2)
    bufferType = 'processed';
end

%% check for missing data that we might need and load
if (isempty(imData))
    % there is no image data passed in to load
    if (isempty(im))
        % there is also no image to load, so we should read one in
        imData = MicroscopeData.ReadMetadata(imagePath,true);
        if (isempty(imData))
            return
        end
        imagePath = imData.imageDir;
    else
        % there is an image to load just no metadata to go with it
        % assume that the voxels are isomorphic
        imData.DatasetName = sprintf('%d-D image',ndims(im));
        imData.Dimensions = Utils.SwapXY_RC(size(im(:,:,:)));
        imData.NumberOfChannels = size(im,4);
        imData.NumberOfFrames = size(im,5);
        imData.PixelPhysicalSize = [1.0,1.0,1.0];
        imData.imageDir = '.';
    end
end

%% open the missing image if it is small enough
if (all(imData.Dimensions<2048) && isempty(im))
    [im,imData] = MicroscopeData.Reader(imData,[],[],[],'uint8',true,false,true);
elseif (isempty(im) || any(imData.Dimensions>2048))
    D3d.UI.InitializeMipFigure(im,imData,imData.imageDir,true);
    imData = [];
    im = [];
end

%% send the image to the viewer
if (~isempty(im) && ~isempty(imData))
    % these structures could still be empty if the user cancelled the image
    % reading
    
    % if this is a 2D image, we will put time in the z dimension
    if (imData.Dimensions(3)==1)
        im = permute(im,[1,2,5,4,3]);
        imData.Dimensions(3) = imData.NumberOfFrames;
        imData.NumberOfFrames = 1;
        imData.PixelPhysicalSize(3) = max(imData.PixelPhysicalSize([1,2]));
    end
    
    if (~strcmpi('unit8',class(im)))
        im8 = ImUtils.ConvertType(im,'uint8',true);
    else
        im8 = im;
    end
    
    D3d.Viewer('loadTexture',im8,imData.PixelPhysicalSize,bufferType);
    D3d.UI.Ctrl.EnableBuffer(bufferNum);
end
end

