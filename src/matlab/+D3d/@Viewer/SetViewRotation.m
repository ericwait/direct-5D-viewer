% SetViewRotation - This will rotate the current view by the desired angle.
%    Viewer.SetViewRotation(rotationVector_xyz,deltaAngle)
%    	RotationVector_xyz - This is the vector that will be rotated relative to the view aligned coordinate system.
%    	DeltaAngle - This is the angle in degrees to rotate around the given vector.
function SetViewRotation(rotationVector_xyz,deltaAngle)
    D3d.Viewer.Mex('SetViewRotation',rotationVector_xyz,deltaAngle);
end
