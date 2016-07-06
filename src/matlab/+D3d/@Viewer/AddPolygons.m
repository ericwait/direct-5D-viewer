% AddPolygons - This loads polygon data into the viewer to be integrated into the texture display.
%    Viewer.AddPolygons(polygonsStruct)
%    	PolygonsStruct is an array of polygon structures.
%    		Each polygon needs to have the following fields:
%    			faces - This is an ordered list of verticies where each group of three represent a triangle face.
%    			verts - This is a list of verticies that represent the exterior of polygon.
%    			norms - This is a list where each group of three make up a vector that represents the normal of the corresponding face.
%    			color - This is three doubles in the range [0,1] that represent the (r,g,b) color.
%    				This can also be three times as long as the vert list where each triple corresponds to a color for each vertex.
%    			frame - This is the frame in which the polygon will be displayed on.
%    			index - This is a unique identifier and will error if a polygon with the same index exist.
%    				This index can be used to show only certain polygons.  When a polygon is right clicked on, this is the number that will be returned.
%    			label - This is a string that will be displayed next to the polygon for reference.
function AddPolygons(polygonsStruct)
    D3d.Viewer.Mex('AddPolygons',polygonsStruct);
end
