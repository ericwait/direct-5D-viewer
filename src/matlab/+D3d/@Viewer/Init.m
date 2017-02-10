% Init - This initializes the D3d viewing window.
%    Viewer.Init(pathStr)
%    	PathStr -- This is the path where the render will find the shader files and is typically in the same directory as the mex dll
function Init(pathStr)
    D3d.Viewer.Mex('Init',pathStr);
end
