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

function [im, imageData] = tiffReader(outType,chanList,timeList,zList,path)
im = [];
imageData = [];

if (~exist('path','var') || ~exist(path,'file'))
    path = [];
end

[imageData,path] = readMetaData(path);
if (isempty(imageData))
    warning('No image read!');
    return
end

if (~exist('chanList','var') || isempty(chanList))
    chanList = 1:imageData.NumberOfChannels;
end
if (~exist('timeList','var') || isempty(timeList))
    timeList = 1:imageData.NumberOfFrames;
end
if (~exist('zList','var') || isempty(zList))
    zList = 1:imageData.ZDimension;
end

if (~exist('outType','var') || isempty(outType))
    imInfo = imfinfo(fullfile(path,sprintf('%s_c%02d_t%04d_z%04d.tif',imageData.DatasetName,1,1,1)),'tif');
    bytes = imInfo.BitDepth/8;
    if (imInfo.BitDepth==8)
        outType = 'uint8';
    elseif (imInfo.BitDepth==16)
        outType = 'uint16';
    elseif (imInfo.BitDepth==32)
        outType = 'uint32';
    else
        outType = 'double';
    end
elseif (strcmp(outType,'double'))
    bytes=8;
    outType = 'double';
elseif (strcmp(outType,'uint32') || strcmp(outType,'int32') || strcmp(outType,'single'))
    bytes=4;
elseif (strcmp(outType,'uint16') || strcmp(outType,'int16'))
    bytes=2;
elseif (strcmp(outType,'uint8'))
    bytes=1;
else
    error('Unsupported Type');
end

imType = imread(fullfile(path,sprintf('%s_c%02d_t%04d_z%04d.tif',imageData.DatasetName,chanList(1),timeList(1),zList(1))),'tif');
typ = whos('imType');
clear imType

imtemp = zeros(imageData.YDimension,imageData.XDimension,length(zList),typ.class);
im = zeros(imageData.YDimension,imageData.XDimension,length(zList),length(chanList),length(timeList),outType);

fprintf('Type:%s ',outType);
fprintf('(');
fprintf('%d',size(im,2));
fprintf(',%d',size(im,1));
for i=3:length(size(im))
    fprintf(',%d',size(im,i));
end

fprintf(') %5.2fMB\n', (imageData.XDimension*imageData.YDimension*length(zList)*length(chanList)*length(timeList)*bytes)/(1024*1024));

for t=1:length(timeList)
    for c=1:length(chanList)
        for z=1:length(zList)
            try
                imtemp(:,:,z) = imread(fullfile(path,sprintf('%s_c%02d_t%04d_z%04d.tif',imageData.DatasetName,chanList(c),timeList(t),zList(z))),'tif');
            catch err
                fprintf('\n****%s: %s\n',fullfile(path,sprintf('%s_c%02d_t%04d_z%04d.tif',imageData.DatasetName,chanList(c),timeList(t),zList(z))),err.identifier);
            end
        end
        w = whos('imtemp');
        
        if (strcmpi(w.class,outType))
            im(:,:,:,c,t) = imtemp;
        else
            im(:,:,:,c,t) = imageConvert(imtemp,outType);
        end
    end
end

clear imtemp
end
