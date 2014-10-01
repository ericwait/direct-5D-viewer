%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
%This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
%tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
%for details. LEVER 3-D is free software: you can redistribute it and/or modify
%it under the terms of the GNU General Public License as published by the Free
%Software Foundation, either version 3 of the License, or (at your option) any
%later version.
%LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
%WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
%A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
%You should have received a copy of the GNU General Public License along with
%LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function lever3d(make2d3d)
global imageData orgImage processedImage tmr uiControlFig useDistance

if (~exist('make2d3d','var') || isempty(make2d3d))
    make2d3d = 0;
end

if (~isempty(uiControlFig) && ishandle(uiControlFig))
    close(uiControlFig);
end

useDistance = 0;

[arrowFaces, arrowVerts, arrowNorms] = MakeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = MakeSphere(0.20,40);

[FileName,PathName,~] = uigetfile('.txt');
if (FileName==0), return, end

try
    mkdir(PathName,'ScreenShots');
    captureFilePath = fullfile(PathName,'ScreenShots');
catch err
    disp('Choose folder to place screen shots...');
    captureFilePath = uigetdir(PathName);
    if captureFilePath~=0
        try
            mkdir(captureFilePath)
        catch err
            captureFilePath = '.';
        end
    end
end

%[orgImage, imageData] = tiffReader([],[],[],[],'d:\Users\Eric.Bioimage29\Documents\Images\LEVer3d\Susan_overnight_new');
[orgImage, imageData] = tiffReader([],[],[],[],fullfile(PathName,FileName));

if (make2d3d~=0)
    orgImage = squeeze(orgImage);
    imageData.ZDimension = imageData.NumberOfFrames;
    imageData.NumberOfFrames = 1;
    imageData.ZPixelPhysicalSize = imageData.XPixelPhysicalSize;
else
    processedImage = orgImage;
end

tmr = timer('TimerFcn',@CheckMessage,'ExecutionMode','fixedSpacing','Period',0.1);
start(tmr);

lever_3d('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms);
lever_3d('takeControl');

lever_3d('loadTexture',imageConvert(orgImage,'uint8'),[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize]);

lever_3d('setCapturePath',captureFilePath,imageData.DatasetName);

uiControlFig = ViewerControls();
lever_3d('resetView');
lever_3d('releaseControl');
end
