function [varargout] = Open( im, imData, imagePath, mesagePkgStr )
%OPEN [varargout] = D3d.Open( im, imData, imagePath )
%Opens the directX 3D-5D viewer with either and image, imData, imagePath,
%or empty.  Will return the image and image data if requested.
%
%IM can be a 3D to 5D image
%IM_DATA can be the meta data in the format like that returned from
%   tiffReader
%IMAGE_PATH can be the path to the metadata file that can be read by
%   tiffReader
%
%*******************************************************************
% GLOBALS:
% d3dtimer - makes a timer that will call checkMessage
%*******************************************************************

global EXT_MESAGE_FUNC D3dIsOpen

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
if (~exist('mesagePkgStr','var'))
    EXT_MESAGE_FUNC = [];
else
    EXT_MESAGE_FUNC = mesagePkgStr;
end
if (isempty(D3dIsOpen))
    D3dIsOpen = false;
end

if (~D3dIsOpen)
    %% start the viewer dll
    [pathstr,~,~] = fileparts(which('D3d.Viewer'));
    D3d.Viewer.Init(pathstr);
    D3dIsOpen = true;
end

%% check if there should be an image loaded right away.
[im,imData] = D3d.LoadImage(im, imData, imagePath);

if (~isempty(im))
    D3d.UI.Controls(imData);
    
    %% start a timer that will check for any messages that the viewer might want to return
    D3d.Messaging.StartTimer();
else
    D3d.Close();
    D3dIsOpen = false;
end

%% return the image and data that might have been read in durring the load
if (nargout>1)
    varargout{2} = imData;
end
if (nargout>0)
    varargout{1} = im;
end
end

