% InitVolume - this will initialize the viewer with data necessary for loading volume textures.
%    Viewer.InitVolume(ImageDims,PhysicalUnits)
%    	Image -- This should be a vector up to five elements indicating the volume size in the order (y,x,z,channel,time).
%    	PhysicalUnits -- A 3-element vector representing physical units of each voxel dimension (e.g. in microns/voxel).
function InitVolume(ImageDims,PhysicalUnits)
    D3d.Viewer.Mex('InitVolume',ImageDims,PhysicalUnits);
end
