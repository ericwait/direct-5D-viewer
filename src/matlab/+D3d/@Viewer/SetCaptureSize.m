% SetCaptureSize - This sets the pixel dimensions of capture images.
%    Viewer.SetCaptureSize(width,height)
%    	Width - This is how many pixels captured images should be in width.
%    	Height - This is how many pixels captured images should be in width.
function SetCaptureSize(width,height)
    D3d.Viewer.Mex('SetCaptureSize',width,height);
end
