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

