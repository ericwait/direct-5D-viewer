%LOADIMAGE D3d.LoadImage( im, bufferNum, frameNumber, nonNormalized)

function im8 = LoadImage( im, bufferNum, frameNumber, normalize, prctSat )
    global D3dIsOpen
    if (isempty(D3dIsOpen) || ~D3dIsOpen)
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
    
    if (~exist('nonNormalized','var') || isempty(normalize))
        normalize = true;
    end
    if (~exist('prctSat','var'))
        prctSat = [];
    end
    
    bufferType = 'original';
    if (bufferNum==2)
        bufferType = 'processed';
    end
   
    % if this is a 2D image, we will put time in the z dimension
    if (size(im,3)==1 && ndims(im)>2)
        im = permute(im,[1,2,5,4,3]);
    end
    
    if (~normalize)
        im8 = ImUtils.ConvertType(im,'uint8',false);
    else
        im8 = ImUtils.BrightenImages(im,'uint8',prctSat);
    end
    if (isempty(frameNumber))
        D3d.Viewer.LoadTexture(im8,bufferType);
    else
        D3d.Viewer.LoadTextureFrame(im8,frameNumber,bufferType);
    end
    
    D3d.UI.Ctrl.EnableBuffer(bufferNum);
end
