% TextureLighting - This allows for the toggling the light on the texture.
%    Viewer.TextureLighting(lightOn)
%    	LightOn - This is a double, (0,1) where a zero means no light and a one means there is a directional light.
function TextureLighting(lightOn)
    D3d.Viewer.Mex('TextureLighting',lightOn);
end
