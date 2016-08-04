% SetBoarderColor - This will set the color of the boarder box.
%    Viewer.SetBoarderColor(color)
%    	Color - This should be three doubles between [0,1] that represent (r,g,b) of the desired background color.
function SetBoarderColor(color)
    D3d.Viewer.Mex('SetBoarderColor',color);
end
