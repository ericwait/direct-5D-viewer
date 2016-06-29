% Init - This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.
%    Viewer.Init(arrowFaces,arrowVerts,arrowNorms,sphereFaces,sphereVerts,shereNorms,pathStr)
%    	ArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.
%    	ArrowVerts -- This is a list of vertices that make up the polygon for each axis.
%    	ArrowNorms -- This is the normal directions that correspond to the faces for each axis.
%    	SphereFaces -- This is an ordered list of vertices that make up the facets (triangles) for an object that represents the axis origin.
%    	SphereVerts -- This is a list of vertices that make up the polygon for an object that represents the axis origin.
%    	ShereNorms -- This is the normal directions that correspond to the faces for an object that represents the axis origin.
%    	PathStr -- This is the path where the render will find the shader files and is typically in the same directory as the mex dll
function Init(arrowFaces,arrowVerts,arrowNorms,sphereFaces,sphereVerts,shereNorms,pathStr)
    curPath = which('Cuda');
    curPath = fileparts(curPath);
    mutexfile = fullfile(curPath,sprintf('device%02d.txt',device));
    while(exist(mutexfile,'file'))
        pause(1);
    end
    f = fopen(mutexfile,'wt');
    fclose(f);

    Viewer.Mex('Init',arrowFaces,arrowVerts,arrowNorms,sphereFaces,sphereVerts,shereNorms,pathStr);

    delete(mutexfile);
end
