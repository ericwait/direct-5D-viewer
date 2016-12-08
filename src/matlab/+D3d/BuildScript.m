curPath = pwd();
D3dPath = fileparts(which('D3d.Open'));
cd(D3dPath)
D3d.BuildMexObject('..\..\c\Mex.mexw64','Viewer');
if ( ~exist('@Viewer/Shaders','dir') )
    mkdir('@Viewer/Shaders');
end
copyfile('../../c/Shaders/','@Viewer/Shaders/');
cd(curPath)
