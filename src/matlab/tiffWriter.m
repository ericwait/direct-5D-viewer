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

function tiffWriter(im,prefix,imageData)

if (exist('imageData','var') && ~isempty(imageData))
    idx = strfind(prefix,'\');
    if (isempty(idx))
        idx = length(prefix);
    end
    createMetadata(prefix(1:idx(end)),imageData);
end

sizes = size(im);
numDim = length(sizes);

if numDim<5
    frames= 1;
else
    frames = sizes(5);
end
if numDim<4
    channels = 1;
else
    channels = sizes(4);
end
if numDim<3
    stacks = 1;
else
    stacks = sizes(3);
end

imUint = uint8(im);

for t=1:frames
    for c=1:channels
        for z=1:stacks
            fileName = sprintf('%s_c%02d_t%04d_z%04d.tif',prefix,c,t,z);
            imwrite(imUint(:,:,z,c,t),fileName,'tif','Compression','lzw');
        end
    end
end

fprintf('Wrote %s_c%d_t%d_z%d.tif\n',prefix,channels,frames,stacks);

end

