% SetFrontClip - This will set the front clipping distance.
%    Viewer.SetFrontClip(FrontClipDistance)
%    	FrontClipDistance - The distance in front of the camera to clip.
function SetFrontClip(FrontClipDistance)
    D3d.Viewer.Mex('SetFrontClip',FrontClipDistance);
end
