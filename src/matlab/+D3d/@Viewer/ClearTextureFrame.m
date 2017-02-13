% ClearTextureFrame - 	Frame -- The frame in the sequence at which to clear the texture data.
%    Viewer.ClearTextureFrame(Frame,BufferType)
%    	BufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to clear from.
function ClearTextureFrame(Frame,BufferType)
    D3d.Viewer.Mex('ClearTextureFrame',Frame,BufferType);
end
