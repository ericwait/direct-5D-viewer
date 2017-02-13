% ClearAllTextures - ClearAllTextures(BufferType) 	BufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to clear from.
function ClearAllTextures(BufferType)
    D3d.Viewer.Mex('ClearAllTextures',BufferType);
end
