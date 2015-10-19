function [varargout] = Open( im, imData, imagePath )
%OPEN [varargout] = open( im, imData, imagePath )
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

global d3dtimer

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

%% make a widget to show the orentation of the current view in 3D
[arrowFaces, arrowVerts, arrowNorms] = Polygons.makeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = Polygons.makeSphere(0.20,40);

%% start a timer that will check for any messages that the viewer might want to return
d3dtimer = timer('TimerFcn',@D3d.Messaging.Check,'ExecutionMode','fixedSpacing','Period',0.1);
start(d3dtimer);

%% start the viewer dll
[pathstr,~,~] = fileparts(which('D3d.Viewer'));
D3d.Viewer('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms,pathstr);

%% check if there should be an image loaded right away.
[im,imData] = D3d.Messaging.LoadImage(im, imData, imagePath);

if (~isempty(im))
    D3d.UI.Controls(imData);
    
    %% return the image and data that might have been read in durring the load
    if (nargout>1)
        varargout{2} = imData;
    end
    if (nargout>0)
        varargout{1} = im;
    end
else
    D3d.Close();
end
end

