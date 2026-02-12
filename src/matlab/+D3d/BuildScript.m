%BuildScript A script to rebuild the D3d.Viewer class from the compiled Mex file.
%
%   This script assumes the C++ project has been built and the mex file exists
%   in the expected relative path. It regenerates the @Viewer class folder.

curPath = pwd();
D3dPath = fileparts(which('D3d.Open'));
cd(D3dPath)
D3d.BuildMexObject('..\..\c\Mex.mexw64','Viewer');
if ( ~exist('@Viewer/Shaders','dir') )
    mkdir('@Viewer/Shaders');
end
copyfile('../../c/Shaders/','@Viewer/Shaders/');
cd(curPath)
