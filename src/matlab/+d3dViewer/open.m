function [varargout] = open( im, imData, imagePath )
%OPEN Summary of this function goes here
%   Detailed explanation goes here

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
[arrowFaces, arrowVerts, arrowNorms] = Ploygons.makeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = Ploygons.makeSphere(0.20,40);

%% start a timer that will check for any messages that the viewer might want to return
% tmr = timer('TimerFcn',@d3dViewer.checkMessage,'ExecutionMode','fixedSpacing','Period',0.1);
% start(tmr);

%% start the viewer dll
d3dViewer.d3dViewer('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms);

%% check if there should be an image loaded right away.
% [im,imData] = d3dViewer.loadImage(im, imData, imagePath);

%% return the image and data that might have been read in durring the load
if (nargout>1)
    varargout{2} = imData;
end
if (nargout>0)
    varargout{1} = im;
end
end

