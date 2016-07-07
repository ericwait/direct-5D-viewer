% SetBackgroundColor - This will set the color of the background.
%    Viewer.SetBackgroundColor(color)
%    	Color - This should be three doubles between [0,1] that represent (r,g,b) of the desired background color.
function SetBackgroundColor(color)
    D3d.Viewer.Mex('SetBackgroundColor',color);
end
