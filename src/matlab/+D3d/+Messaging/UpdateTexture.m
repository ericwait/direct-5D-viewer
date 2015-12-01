function UpdateTexture( im, isOrg)
%UPDATETEXTURE Summary of this function goes here
%   Detailed explanation goes here
global imageData

im = imageConvertNorm(im,'uint8',true);

if (~exist('isOrg','var') || isempty(isOrg) || true==isOrg)
    lever_3d('loadTexture',im,imageData.PixelPhysicalSize);
else
    lever_3d('loadTexture',im,imageData.PixelPhysicalSize,'processed');
end
end

