% ToggleWireframe - This will toggle wireframe on or off for the polygons.
%    Viewer.ToggleWireframe(wireFrameOn)
%    	WireFrameOn - This is a double that is either a zero or a one, where a zero means filled polygons and a one means wire framed polygons.
function ToggleWireframe(wireFrameOn)
    D3d.Viewer.Mex('ToggleWireframe',wireFrameOn);
end
