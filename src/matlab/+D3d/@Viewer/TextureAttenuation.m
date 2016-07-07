% TextureAttenuation - This allows for the toggling the attenuation of intensities on the texture.
%    Viewer.TextureAttenuation(on)
%    	On - This is a double, [0,1] where a zero means no attenuation and a one means there is a attenuation applied to the texture.
function TextureAttenuation(on)
    D3d.Viewer.Mex('TextureAttenuation',on);
end
