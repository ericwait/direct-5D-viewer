%LOADIMAGE D3d.LoadImage( im, bufferNum, frameNumber)

function LoadImage( im, bufferNum, frameNumber )
    global D3dIsOpen
    if (~D3dIsOpen || isempty(D3dIsOpen))
        error('You need to open the viewer before you can load images! Call D3d.Open first.');
    end
    
    if (~exist('bufferNum','var') || isempty(bufferNum))
        bufferNum = 1;
    else
        if (~bufferNum==1 && ~bufferNum==2)
            error('bufferType can only be 1 or 2!');
        end
    end
    
    if (~exist('frameNumber','var'))
        frameNumber = [];
    elseif (numel(frameNumber)>1)
        error('You can only load one frame at a time!');
    end

    bufferType = 'original';
    if (bufferNum==2)
        bufferType = 'processed';
    end
   
    % if this is a 2D image, we will put time in the z dimension
    if (size(im,3)==1 && ndims(im)>2)
        im = permute(im,[1,2,5,4,3]);
    end
    
    im8 = ImUtils.ConvertType(im,'uint8',true);
    if (isempty(frameNumber))
        D3d.Viewer.LoadTexture(im8,bufferType);
    else
        D3d.Viewer.LoadTextureFrame(im8,frameNumber,bufferType);
    end
    
    D3d.UI.Ctrl.EnableBuffer(bufferNum);
end
