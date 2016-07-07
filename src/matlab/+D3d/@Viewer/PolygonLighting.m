% PolygonLighting - This allows for the toggling the light on the polygons.
%    Viewer.PolygonLighting(lightOn)
%    	LightOn - This is a double, (0,1) where a zero means no light and a one means there is a directional light.
function PolygonLighting(lightOn)
    D3d.Viewer.Mex('PolygonLighting',lightOn);
end
