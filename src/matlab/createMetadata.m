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

function createMetadata(root,imageData)

if (~exist(root,'dir'))
    mkdir(root);
end

fileName = fullfile(root,[imageData.DatasetName '.txt']);
fprintf('Creating Metadata %s...',fileName);

fileHandle = fopen(fileName,'wt');

fprintf(fileHandle,'DatasetName:%s\n',imageData.DatasetName);

fprintf(fileHandle,'NumberOfChannels:%d\n',imageData.NumberOfChannels);

if (isfield(imageData,'ChannelColors'))
    fprintf(fileHandle,'ChannelColors:');

    if (size(imageData.ChannelColors,1)==1 && ~iscell(imageData.ChannelColors))
        fprintf(fileHandle,'%s,',imageData.ChannelColors);
    else
        for i=1:length(imageData.ChannelColors)
            fprintf(fileHandle,'%s,',imageData.ChannelColors{i});
        end
    end
    fprintf(fileHandle,'\n');
end

fprintf(fileHandle,'NumberOfFrames:%d\n',imageData.NumberOfFrames);

% fprintf(fileHandle,'XDimension:%d\n',imageData.yDim);
% fprintf(fileHandle,'YDimension:%d\n',imageData.xDim);
fprintf(fileHandle,'XDimension:%d\n',imageData.XDimension);
fprintf(fileHandle,'YDimension:%d\n',imageData.YDimension);
fprintf(fileHandle,'ZDimension:%d\n',imageData.ZDimension);

fprintf(fileHandle,'XPixelPhysicalSize:%f\n',imageData.XPixelPhysicalSize);
fprintf(fileHandle,'YPixelPhysicalSize:%f\n',imageData.YPixelPhysicalSize);
fprintf(fileHandle,'ZPixelPhysicalSize:%f\n',imageData.ZPixelPhysicalSize);

if (isfield(imageData,'XPosition'))
    fprintf(fileHandle,'XPosition:%f\n',imageData.XPosition);
end
if (isfield(imageData,'YPosition'))
    fprintf(fileHandle,'YPosition:%f\n',imageData.YPosition);
end

if (isfield(imageData,'XDistanceUnits'))
    fprintf(fileHandle,'XDistanceUnits:%s\n',imageData.XDistanceUnits);
end
if (isfield(imageData,'YDistanceUnits'))
    fprintf(fileHandle,'YDistanceUnits:%s\n',imageData.YDistanceUnits);
end
if (isfield(imageData,'ZDistanceUnits'))
    fprintf(fileHandle,'ZDistanceUnits:%s\n',imageData.ZDistanceUnits);
end

if (isfield(imageData,'XLength'))
    fprintf(fileHandle,'XLength:%f\n',imageData.XLength);
end
if (isfield(imageData,'YLength'))
    fprintf(fileHandle,'YLength:%f\n',imageData.YLength);
end
if (isfield(imageData,'ZLength'))
    fprintf(fileHandle,'ZLength:%f\n',imageData.ZLength);
end

if (isfield(imageData,'StartCaptureDate'))
    fprintf(fileHandle,'StartCaptureDate:%s\n',imageData.StartCaptureDate);
end

if (isfield(imageData,'TimeStampDeltas'))
    for t=1:imageData.NumberOfFrames
        for z=1:imageData.ZDimension
            for c=1:imageData.NumberOfChannels
                fprintf(fileHandle,'TimeStampDelta(%d,%d,%d):%f\n',c,t,z,imageData.TimeStampDeltas(z,c,t));
            end
        end
    end
end
fclose(fileHandle);

fprintf('Done\n');
end