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
global imageData  processedMetadata segMetadata distMetadata Hulls Tracks Families tmr uiControlFig uiControlHandles useDistance gSavingData

gSavingData = 0;

if (~isempty(uiControlFig) && ishandle(uiControlFig))
    close(uiControlFig);
end

useDistance = 0;

[arrowFaces, arrowVerts, arrowNorms] = MakeArrow(0.65,0.05,0.15,40);
[sphereFaces, sphereVerts, shereNorms] = MakeSphere(0.20,40);

tic
[orgImage, imageData] = tiffReader([],[],[],[],'uint8',true);
fprintf('Took %s to read\n',printTime(toc));

if(isempty(imageData))
   return; 
end

try
    if (~exist(fullfile(imageData.imageDir,'ScreenShots'),'file'))
        mkdir(imageData.imageDir,'ScreenShots');
    end
    captureFilePath = fullfile(imageData.imageDir,'ScreenShots');
catch err
    disp('Choose folder to place screen shots...');
    captureFilePath = uigetdir(imageData.imageDir);
    if captureFilePath~=0
        try
            mkdir(captureFilePath)
        catch err
            captureFilePath = '.';
        end
    end
end

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

if (exist(fullfile(imageData.imageDir,'Processed'),'dir'))
    if (exist(fullfile(imageData.imageDir,'Processed','processedMetadata.mat'),'file'))
        load(fullfile(imageData.imageDir,'Processed','processedMetadata.mat'));
        processedMetadata.PathName = fullfile(imageData.imageDir,'Processed');
        %set(uiControlHandles.rb_Processed,'Enable','on','Value',1);
        %set(uiControlHandles.rb_orgImage,'Value',0);
    end
    if (exist(fullfile(imageData.imageDir,'Processed','distMetadata.mat'),'file'))
        load(fullfile(imageData.imageDir,'Processed','distMetadata.mat'));
        set(uiControlHandles.m_DistanceChoice,'Enable','on');
        distMetadata.PathName = fullfile(imageData.imageDir,'Processed');
    end
    if (exist(fullfile(imageData.imageDir,'Processed','segMetadata.mat'),'file'))
        load(fullfile(imageData.imageDir,'Processed','segMetadata.mat'));
        segMetadata.PathName = fullfile(imageData.imageDir,'Processed');
    end
    if (~isempty(segMetadata) && exist(fullfile(imageData.imageDir,'Processed',[imageData.DatasetName,'_Segmenation.mat']),'file'))
        load(fullfile(imageData.imageDir,'Processed',[imageData.DatasetName,'_Segmenation.mat']));
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

if (~isempty(processedMetadata) && ~isempty(readMetadata(fullfile(processedMetadata.PathName,processedMetadata.FileName))))
    set(uiControlHandles.rb_Processed,'Enable','on','Value',1);
    set(uiControlHandles.rb_orgImage,'Value',0);
    
    viewImage = tiffReader(fullfile(processedMetadata.PathName,processedMetadata.FileName),[],[],[],'uint8',true,true);
    
    lever_3d('loadTexture',viewImage,[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize],'processed');
    clear viewImage
end
fprintf('Done loading\n');
end
