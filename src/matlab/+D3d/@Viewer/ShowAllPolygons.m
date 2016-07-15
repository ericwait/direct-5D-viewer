% ShowAllPolygons - This will toggle the polygons on and off.
%    Viewer.ShowAllPolygons(on)
%    	On - If this is set to one, then all of the polygons will be on. If this is 0, all of the polygons will be off.
%    		If only a subset of the polygons should be displayed, use 'DisplayPolygons'.
function ShowAllPolygons(on)
    D3d.Viewer.Mex('ShowAllPolygons',on);
end
