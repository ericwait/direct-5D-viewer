function Update()
%Update Forces a render update in the D3d Viewer.
%   D3d.Update()
%
%   This function should be called after changing image data or other
%   properties that don't automatically trigger a refresh.
    D3d.Viewer.UpdateRender();
end
