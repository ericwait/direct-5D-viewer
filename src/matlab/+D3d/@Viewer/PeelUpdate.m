% PeelUpdate - This will set the percent of the volume to render before rendering the polygons in the same area.
%    Viewer.PeelUpdate(PeelSize)
%    	PeelSize -- This is the percentage of the volume to render before rendering polygons in the same space.
function PeelUpdate(PeelSize)
    D3d.Viewer.Mex('PeelUpdate',PeelSize);
end
