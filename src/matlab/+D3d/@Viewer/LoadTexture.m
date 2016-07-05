% LoadTexture - this will load the image into texture buffers for display in the D3d viewer.
%    Viewer.LoadTexture(Image,AnisotropicVoxelDimension,BufferType)
%    	Image -- This should be an matrix up to five dimensions in the order (y,x,z,channel,time).
%    	AnisotropicVoxelDimension -- This is the dimensions of a voxel (3D pixel) in microns. This will ensure that the displayed voxels are isotropic and that the scale bar displays properly.
%    	BufferType -- this can either be 'original' or 'processed' and corresponds to the first and second texture buffer available to load images into.
function LoadTexture(Image,AnisotropicVoxelDimension,BufferType)
    D3d.Viewer.Mex('LoadTexture',Image,AnisotropicVoxelDimension,BufferType);
end
