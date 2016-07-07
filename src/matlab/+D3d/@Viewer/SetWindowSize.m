% SetWindowSize - This set the viewable space to the number of pixels specified.
%    Viewer.SetWindowSize(width,height)
%    	Width - This is how many pixels the viewer should be in width.
%    	Height - This is how many pixels the viewer should be in width.
function SetWindowSize(width,height)
    D3d.Viewer.Mex('SetWindowSize',width,height);
end
