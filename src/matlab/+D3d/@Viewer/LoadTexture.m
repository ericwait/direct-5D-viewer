% LoadTexture - this will load the image into texture buffers for display in the D3d viewer.
%    Viewer.LoadTexture(Image,BufferType)
%    	Image -- This should be an matrix up to five dimensions in the order (y,x,z,channel,time).
%    	BufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to load images into.
function LoadTexture(Image,BufferType)
    D3d.Viewer.Mex('LoadTexture',Image,BufferType);
end
