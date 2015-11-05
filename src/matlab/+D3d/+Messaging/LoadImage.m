function [varargout] = LoadImage( im, imData, imagePath, bufferType )
%LOADIMAGE [varargout] = loadImage( im, imData, imagePath )
%sends the image data to the directX viewer.
%
%IM is an 'uint8' image 3D-5D image matrix, optional.
%IM_DATA is astructure that holds metadata for the image and
%   needs to be in the format that tiffReader would return, optional.
%IMAGE_PATH is a path to the metadata file that can be read by
%   tiffReader, optional.

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
if (~exist('bufferType','var') || isempty(bufferType))
    bufferType = 'orginal';
end

%% check for missing data that we might need and load 
if (isempty(imData))
    % there is no image data passed in to load
    if (isempty(im))
        % there is also no image to load, so we should read one in
        [im,imData] = MicroscopeData.Reader(imagePath,[],[],[],'uint8',true,[],true);
    else
        % there is an image to load just no metadat to go with it
        % assume that the voxels are isomorphic
        imData.XPixelPhysicalSize = 1.0;
        imData.YPixelPhysicalSize = 1.0;
        imData.ZPixelPhysicalSize = 1.0;
    end
elseif (isempty(im))
    % there is image data that will allow us to load up the missing image
    [im,imData] = MicroscopeData.Reader(imData,[],[],[],'uint8',true,[],true);
else
    % we should have everything we need to load the image in the viewer
end

%% send the image to the viewer
if (~isempty(im) && ~isempty(imData))
    % these structures could still be empty if the user cancelled the image
    % reading
    D3d.Viewer('loadTexture',im,[imData.XPixelPhysicalSize,imData.YPixelPhysicalSize,imData.ZPixelPhysicalSize],bufferType);
end

%% return the image and data that might have been read in durring the load
if (nargout>1)
    varargout{2} = imData;
end
if (nargout>0)
    varargout{1} = im;
end
end

