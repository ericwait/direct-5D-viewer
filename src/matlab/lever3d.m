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

function lever3d()
global imageData orgMetadata  processedMetadata segMetadata distMetadata Hulls Tracks Families tmr uiControlFig uiControlHandles useDistance

if (~isempty(uiControlFig) && ishandle(uiControlFig))
    close(uiControlFig);
end

useDistance = 0;

orgMetadata.PathName = '';
orgMetadata.FileName = '';

[arrowFaces, arrowVerts, arrowNorms] = MakeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = MakeSphere(0.20,40);

[orgMetadata.FileName,orgMetadata.PathName,~] = uigetfile('.txt');
if (orgMetadata.FileName==0), return, end

processedMetadata.PathName = fullfile(orgMetadata.PathName,'Processed');
segMetadata.PathName = fullfile(orgMetadata.PathName,'Processed');
distMetadata.PathName = fullfile(orgMetadata.PathName,'Processed');
try
    if (~exist(fullfile(orgMetadata.PathName,'ScreenShots'),'file'))
        mkdir(orgMetadata.PathName,'ScreenShots');
    end
    captureFilePath = fullfile(orgMetadata.PathName,'ScreenShots');
catch err
    disp('Choose folder to place screen shots...');
    captureFilePath = uigetdir(orgMetadata.PathName);
    if captureFilePath~=0
        try
            mkdir(captureFilePath)
        catch err
            captureFilePath = '.';
        end
    end
end

tic
[orgImage, imageData] = tiffReader(fullfile(orgMetadata.PathName,orgMetadata.FileName),[],[],[],'uint8',true);
fprintf('Took %s to read\n',printTime(toc));

tmr = timer('TimerFcn',@CheckMessage,'ExecutionMode','fixedSpacing','Period',0.1);
start(tmr);

lever_3d('init',arrowFaces, arrowVerts, arrowNorms,sphereFaces, sphereVerts, shereNorms);
lever_3d('takeControl');

lever_3d('loadTexture',orgImage,[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize]);

clear orgImage

lever_3d('setCapturePath',captureFilePath,imageData.DatasetName);

uiControlFig = ViewerControls();
lever_3d('resetView');
lever_3d('releaseControl');

if (exist(fullfile(orgMetadata.PathName,'Processed'),'dir'))
    if (exist(fullfile(orgMetadata.PathName,'Processed','processedMetadata.mat'),'file'))
        load(fullfile(orgMetadata.PathName,'Processed','processedMetadata.mat'));
    end
    if (exist(fullfile(orgMetadata.PathName,'Processed','distMetadata.mat'),'file'))
        load(fullfile(orgMetadata.PathName,'Processed','distMetadata.mat'));
        set(uiControlHandles.m_DistanceChoice,'Enable','on');
    end
    if (exist(fullfile(orgMetadata.PathName,'Processed','segMetadata.mat'),'file'))
        load(fullfile(orgMetadata.PathName,'Processed','segMetadata.mat'));
    end
    if (~isempty(segMetadata) && exist(fullfile(orgMetadata.PathName,'Processed',[imageData.DatasetName,'_Segmenation.mat']),'file'))
        load(fullfile(orgMetadata.PathName,'Processed',[imageData.DatasetName,'_Segmenation.mat']));
        if (~isempty(Hulls))
            set(uiControlHandles.cb_SegmentationResults,'Value',1,'Enable','on');
            set(uiControlHandles.cb_Wireframe,'Value',1,'Enable','on');
            set(uiControlHandles.cb_segLighting,'Enable','on');
            set(uiControlHandles.cb_ShowLabels,'Enable','on');
            
            lever_3d('loadHulls',Hulls);
            if (~isempty(distMetadata))
                set(uiControlHandles.m_DistanceChoice,'Enable','on');
            end
            
            DrawTree();
        end
    end
end

if (~isempty(processedMetadata) && exist(fullfile(processedMetadata.PathName,processedMetadata.FileName),'file'))
    set(uiControlHandles.rb_Processed,'Enable','on','Value',1);
    set(uiControlHandles.rb_orgImage,'Value',0);
    
    viewImage = zeros(imageData.YDimension,imageData.XDimension,imageData.ZDimension,imageData.NumberOfChannels,imageData.NumberOfFrames,'uint8');
    for c=1:imageData.NumberOfChannels
        if (processedMetadata.ChanProcessed(c))
            viewImage(:,:,:,c,:) = tiffReader(fullfile(processedMetadata.PathName,processedMetadata.FileName),[],c,[],'uint8',true,true);
        else
            viewImage(:,:,:,c,:) = tiffReader(fullfile(orgMetadata.PathName,orgMetadata.FileName),[],c,[],'uint8',true,true);
        end
    end
    lever_3d('loadTexture',viewImage,[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize],'processed');
    clear viewImage
end
fprintf('Done loading\n');
end
