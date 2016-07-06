% SetViewOrigin - This will set the view origin (where the camera looks).
%    Viewer.SetViewOrigin(viewOrigin)
%    	ViewOrigin - this is three values that correspond to the point that you would like the camera to point at.
function SetViewOrigin(viewOrigin)
    D3d.Viewer.Mex('SetViewOrigin',viewOrigin);
end
