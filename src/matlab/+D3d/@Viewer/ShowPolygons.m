% ShowPolygons - This will show only the listed polygons.
%    Viewer.ShowPolygons(polygonIdxList)
%    	PolygonIdxList - This is a list of polygon indices that correspond to the index field passed to the AddPolygon command.
%    		Only the polygons in this list will be displayed.
function ShowPolygons(polygonIdxList)
    D3d.Viewer.Mex('ShowPolygons',polygonIdxList);
end
