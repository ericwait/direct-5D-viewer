% ShowPolygonList - This will show only the listed polygons.
%    Viewer.ShowPolygonList(polygonIndices)
%    	PolygonIdxList - This is a list of polygon indices that correspond to the index field passed to the AddPolygon command.
%    		Only the polygons in this list will be displayed.
function ShowPolygonList(polygonIndices)
    D3d.Viewer.Mex('ShowPolygonList',polygonIndices);
end
