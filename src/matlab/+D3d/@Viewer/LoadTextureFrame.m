% LoadTextureFrame - this will load the image into texture buffers for display in the D3d viewer.
%    Viewer.LoadTextureFrame(Image,Frame,BufferType)
%    	Image -- This should be an matrix up to 4 dimensions in the order (y,x,z,channel).
%    	Frame -- The frame in the sequence at which to load this texture data.
%    	BufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to load images into.
function LoadTextureFrame(Image,Frame,BufferType)
    D3d.Viewer.Mex('LoadTextureFrame',Image,Frame,BufferType);
end
