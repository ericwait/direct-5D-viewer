curPath = pwd();
D3dPath = fileparts(which('D3d.Open'));
cd(D3dPath)
D3d.BuildMexObject('..\..\c\Mex.mexw64','Viewer');
cd(curPath)
