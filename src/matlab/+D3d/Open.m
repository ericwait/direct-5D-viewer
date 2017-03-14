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
            imSz = size(im);
            imData.Dimensions = Utils.SwapXY_RC(imSz(1:3));
            imData.NumberOfChannels = size(im,4);
            imData.NumberOfFrames = size(im,5);
            imData.PixelPhysicalSize = [1.0,1.0,1.0];
            imData.imageDir = '.';
        end
    end

    %% open the region chooser if the image is too big
    if (any(imData.Dimensions>2048))
        D3d.UI.InitializeMipFigure(im,imData,imData.imageDir,true);
        imData = [];
        im = [];
    end
    
    %% if there is no metadata, get out without starting anything
    if (isempty(imData))
        if (nargout>1)
            varargout{2} = [];
        end
        if (nargout>0)
            varargout{1} = im;
        end
        return
    end
    
    %% if the data is 2D put time on the third dimension
    if (imData.Dimensions(3)==1)
        imData.Dimensions(3) = imData.NumberOfFrames;
        imData.NumberOfFrames = 1;
        
        imData.PixelPhysicalSize(3) = max(imData.PixelPhysicalSize([1,2]))*10;
        
        im = permute(im,[1,2,5,4,3]);
    end

    %% start the viewer
    if (~D3dIsOpen)
        [pathstr,~,~] = fileparts(which('D3d.Viewer'));
        D3d.Viewer.Init(pathstr);
        D3dIsOpen = true;
    end

    D3d.Viewer.InitVolume([imData.Dimensions([2,1,3]),imData.NumberOfChannels,imData.NumberOfFrames],imData.PixelPhysicalSize([2,1,3]));
    D3d.UI.Controls(imData);
    
    if (~isempty(im))
        if (size(im,4)<imData.NumberOfChannels)
            error('You must pass in all of the channels when opening!');
        end
        
        if (size(im,5)<imData.NumberOfFrames)
            D3d.LoadImage(im(:,:,:,:,1),1,1);
            D3d.Update();
            loadTransFunc(imData,im);
            for t=2:size(im,5)
                D3d.LoadImage(im(:,:,:,:,t),1,t);
            end
        else
            D3d.LoadImage(im);
            loadTransFunc(imData,im);
            D3d.Update();
        end
    else
        im = MicroscopeData.Reader('path',imData.imageDir,'timeRange',[1,1]);
        D3d.LoadImage(im,1,1);
        D3d.Update();
        loadTransFunc(imData,im);
        
        disp('Only the first image was loaded, please wait while I load the others...');
        im = MicroscopeData.Reader('path',imData.imageDir,'imageData',imData,'verbose',true);
        
        prgs = Utils.CmdlnProgress(imData.NumberOfFrames,true,'Loading images in viewer');
        for t=2:imData.NumberOfFrames
            D3d.LoadImage(im(:,:,:,:,t),1,t);
            prgs.PrintProgress(t);
        end
        prgs.ClearProgress(true);
    end
    
    %% start a timer that will check for any messages that the viewer might want to return
    D3d.Messaging.StartTimer();

    %% return the image and data that might have been read in durring the load
    if (nargout>1)
        varargout{2} = imData;
    end
    if (nargout>0)
        varargout{1} = im;
    end
end

function loadTransFunc(imData,im)
    if (exist(fullfile(imData.imageDir,[imData.DatasetName,'_transfer','.json']),'file'))
        D3d.LoadTransferFunction(fullfile(imData.imageDir,[imData.DatasetName,'_transfer','.json']));
    else
        D3d.UI.AutoTransferFunction(im);
    end 
end
