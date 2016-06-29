% Help - Help on a specified command.
%    Viewer.Help(command)
%    Print detailed usage information for the specified command.
function Help(command)
    curPath = which('Cuda');
    curPath = fileparts(curPath);
    mutexfile = fullfile(curPath,sprintf('device%02d.txt',device));
    while(exist(mutexfile,'file'))
        pause(1);
    end
    f = fopen(mutexfile,'wt');
    fclose(f);

    Viewer.Mex('Help',command);

    delete(mutexfile);
end
