%IMAGECONVERT converts image from current type into the specified type

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

function [ imageOut ] = imageConvert( imageIn, typ )
w = whos('imageIn');

if (strcmpi(w.class,typ))
    imageOut = imageIn;
else
    imageIn = double(imageIn);
    
    switch w.class
        case 'uint8'
            imtemp = imageIn ./ (2^8-1);
        case 'uint16'
            if (max(imageIn(:))<=2^12-1)
                imtemp = imageIn ./(2^12-1);
            else
                imtemp = imageIn ./ (2^16-1);
            end
        case 'int16'
            imtemp = (imageIn+(2^16/2 - 1)) ./ (2^16-1);
        case 'uint32'
            imtemp = imageIn ./ 2^32-1;
        case 'int32'
            imtemp = (imageIn+(2^32/2 -1)) ./ (2^32-1);
        case 'single'
            imtemp = (imageIn+abs(min(imageIn(:)))) ./ max(imageIn(:));
        case 'double'
            imtemp = (imageIn+abs(min(imageIn(:)))) ./ max(imageIn(:));
        case 'logical'
            imtemp = imageIn;
        otherwise
            error('Unkown type of image to convert!');
    end
    
    switch typ
        case 'uint8'
            imageOut = uint8(imtemp*(2^8-1));
        case 'uint16'
            imageOut = uint16(imtemp*(2^16-1));
        case 'int16'
            if (max(imtemp(:))<=0.5)
                imageOut = int16(imtemp*(2^16-1));
            else
                imageOut = int16((imtemp-0.5)*(2^16/2-1));
            end
        case 'uint32'
            imageOut = uint32(imtemp*(2^32-1));
        case 'int32'
            if (max(imtemp(:))<=0.5)
                imageOut = int32(imtemp*(2^32-1));
            else
                imageOut = int32((imtemp-0.5)*(2^32/2-1));
            end
        case 'single'
            imageOut = single(imtemp);
        case 'double'
            imageOut = imtemp;
        case 'logical'
            imageOut = imtemp>0;
        otherwise
            error('Unkown type of image to convert to!');
    end
    clear imtemp
end
end

