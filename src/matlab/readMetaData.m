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

function [imageData,rootDir] = readMetaData(root)

imageData = [];
rootDir = [];

if (~exist('root','var') || isempty(root))
    rootDir = uigetdir('');
    if rootDir==0,
        [fileName,rootDir,filterIndex] = uigetfile('.txt');
        if (filterIndex~=0)
            fileHandle = fopen(fullfile(rootDir,fileName));
            imageData = readfile(fileHandle);
        end
        return
    end
elseif (~isempty(strfind(root,'.txt')))
    fileHandle = fopen(root);
    imageData = readfile(fileHandle);
    pos = strfind(root,'\');
    rootDir = root(1:pos(end));
    imageData.imageDir = rootDir;
    return
else
    rootDir = root;
end

imageData = [];
dlist = dir(rootDir);

for i=1:length(dlist)
    if (strcmp('..',dlist(i).name))
        continue;
    end
    
    dSublist = dir(fullfile(rootDir,dlist(i).name,'*.txt'));
    if isempty(dSublist), continue, end
    
    for j=1:length(dSublist)
        fileHandle = fopen(fullfile(rootDir,dlist(i).name,dSublist(j).name));
        imageDatum = readfile(fileHandle);
        
        if (isempty(imageDatum)), continue, end
        
        if (isempty(imageData))
            imageData = imageDatum;
        else
            imageData(length(imageData)+1) = imageDatum;
        end
    end
    if strcmpi('.',dlist(i).name)
        imageData.imageDir = rootDir;
    else
        imageData.imageDir = fullfile(rootDir,dlist(i).name);
    end
end
rootDir = imageData.imageDir;
end

function imageDatum = readfile(fileHandle)
imageDatum = {};

if fileHandle<=0, return, end

data = textscan(fileHandle,'%s %s', 'delimiter',':','whitespace','\n');
fclose(fileHandle);

if isempty(data), return, end

for k=1:length(data{1})
    val = str2double(data{2}{k});
    if (isnan(val))
        val = data{2}{k};
    end
    if (any(strfind(data{1}{k},'TimeStampDelta')))
        if (~isfield(imageDatum,'TimeStampDeltas'))
            imageDatum.TimeStampDeltas = zeros(imageDatum.ZDimension,imageDatum.NumberOfChannels,imageDatum.NumberOfFrames);
        end
        plane = textscan(data{1}{k},'TimeStampDeltas(%d,%d,%d)%s');
        imageDatum.TimeStampDeltas(plane{1},plane{2},plane{3}) = val;
    else
        imageDatum.(data{1}{k}) = val;
    end
end
imageData.imageDir = '.';
end
