% SetRotation - This will rotate the current view by the desired angle.
%    Viewer.SetRotation(deltaXangle,deltaYangle,deltaZangle)
%    	DeltaXangle - This will rotate around the X axis by the given angle.
%    	DeltaYangle - This will rotate around the Y axis by the given angle.
%    	DeltaZangle - This will rotate around the Z axis by the given angle.
function SetRotation(deltaXangle,deltaYangle,deltaZangle)
    D3d.Viewer.Mex('SetRotation',deltaXangle,deltaYangle,deltaZangle);
end
