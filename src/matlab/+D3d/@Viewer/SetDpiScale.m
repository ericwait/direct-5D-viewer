% SetDpiScale - This sets the amount to enlarge each window pixel (larger scale percentages improve render speed but can blur the rendering).
%    Viewer.SetDpiScale(scalePct)
%    	ScalePct - The percentage to 'enlarge' each pixel in the window (default 150%).
function SetDpiScale(scalePct)
    D3d.Viewer.Mex('SetDpiScale',scalePct);
end
