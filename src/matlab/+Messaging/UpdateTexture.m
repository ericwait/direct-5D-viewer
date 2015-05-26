function UpdateTexture( im, isOrg)
%UPDATETEXTURE Summary of this function goes here
%   Detailed explanation goes here
global imageData

sz = [imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize];

im = imageConvertNorm(im,'uint8',true);

if (~exist('isOrg','var') || isempty(isOrg) || true==isOrg)
    lever_3d('loadTexture',im,sz);
else
    lever_3d('loadTexture',im,sz,'processed');
end
end

