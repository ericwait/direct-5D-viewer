% SetWorldRotation - This will rotate the current view by the desired angle.
%    Viewer.SetWorldRotation(rotationVector_xyz,deltaAngle)
%    	RotationVector_xyz - This is the vector that will be rotated relative to the world coordinate system (around the current widget).
%    	DeltaAngle - This is the angle in degrees to rotate around the given vector.
function SetWorldRotation(rotationVector_xyz,deltaAngle)
    D3d.Viewer.Mex('SetWorldRotation',rotationVector_xyz,deltaAngle);
end
