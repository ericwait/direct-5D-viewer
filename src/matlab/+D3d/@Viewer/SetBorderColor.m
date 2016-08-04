% SetBorderColor - This will set the color of the border box.
%    Viewer.SetBorderColor(color)
%    	Color - This should be three doubles between [0,1] that represent (r,g,b) of the desired background color.
function SetBorderColor(color)
    D3d.Viewer.Mex('SetBorderColor',color);
end
