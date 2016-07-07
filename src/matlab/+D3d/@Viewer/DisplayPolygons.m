% DisplayPolygons - This will show only the listed polygons.
%    Viewer.DisplayPolygons(polygonIndices)
%    	PolygonIdxList - This is a list of polygon indices that correspond to the index field passed to the AddPolygon command.
%    		Only the polygons in this list will be displayed.
function DisplayPolygons(polygonIndices)
    D3d.Viewer.Mex('DisplayPolygons',polygonIndices);
end
