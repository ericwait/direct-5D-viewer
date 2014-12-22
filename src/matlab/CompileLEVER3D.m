%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
%This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
%tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
%for details. LEVER 3-D is free software: you can redistribute it and/or modify
%it under the terms of the GNU General Public License as published by the Free
%Software Foundation, either version 3 of the License, or (at your option) any
%later version.
%LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
%WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
%A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
%You should have received a copy of the GNU General Public License along with
%LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function CompileLEVER3D()
totalTime = tic;
[vsStruct, comparch] = setupCompileTools();

bindir = '..\..\bin';
if ( strcmpi(comparch,'win64') )
    bindir = '..\..\bin64';
end

if ( ~exist(bindir,'dir') )
    mkdir(bindir);
end

depDirs = {'d:\Users\Eric.Bioimage29\Documents\Programming\cuda-image-processing-libarary\bin';...
    'd:\Users\Eric.Bioimage29\Documents\Programming\general-matlab-tools\src\MATLAB'};

copyDeps(depDirs);

outputFiles = {};

newOutput = compileMEX('lever-3d', vsStruct);
outputFiles = [outputFiles; {newOutput}];

newOutput = compileMEX('Tracker', vsStruct);
outputFiles = [outputFiles; {newOutput}];

[toolboxStruct, externalStruct] = GetExternalDependencies();
if ( ~isempty(externalStruct.deps) )
    fprintf('ERROR: Some local functions have external dependencies\n');
    for i=1:length(externalStruct.deps)
        fprintf('[%d]  %s\n', i, externalStruct.deps{i});
        for j=1:length(externalStruct.funcs{i})
            if ( ~isempty(externalStruct.callers{i}{j}) )
                for k=1:length(externalStruct.callers{i}{j})
                    localName = GetLocalName(externalStruct.callers{i}{j}{k});
                    fprintf('    %s calls: %s\n', localName, externalStruct.funcs{i}{j});
                end
            end
        end
        fprintf('------\n');
    end
    
    %         error('External dependencies cannot be packaged in a MATLAB executable');
    questionStr = sprintf('%s\n%s','Possible external dependencies were found in project, you should verify all these functions are local before continuing the build.','Are you sure you wish to continue?');
    result = questdlg(questionStr,'External Dependency Warning','Continue','Cancel','Cancel');
    if ( strcmpi(result,'Cancel') )
        return;
    end
end

% temporarily remove any startup scripts that would normally be run by matlabrc
enableStartupScripts(false);

addFiles = {'d3dcompiler_47.dll'};
compileMATLAB('lever3d', bindir, addFiles, toolboxStruct.deps);

system(['copy DefaultMeshShaders.fx ' fullfile(bindir,'.')]);
system(['copy ViewAlignedVertexShader.fx ' fullfile(bindir,'.')]);

enableStartupScripts(true);

fprintf('\n');

rmDeps(depDirs);

toc(totalTime)

end

function copyDeps(dirs)
for i=1:numel(dirs)
    dlist = dir(dirs{i});
    for j=1:numel(dlist)
        if (strcmpi(dlist(j).name,'.') || strcmpi(dlist(j).name,'..'))
            continue
        end
        if (exist(fullfile(dirs{i},dlist(j).name),'file'))
            copyfile(fullfile(dirs{i},dlist(j).name),'.');
        end
    end
end
end

function rmDeps(dirs)
for i=1:numel(dirs)
    dlist = dir(dirs{i});
    for j=1:numel(dlist)
        if (strcmpi(dlist(j).name,'.') || strcmpi(dlist(j).name,'..'))
            continue
        end
        if (exist(fullfile('.',dlist(j).name),'file'))
            delete(fullfile('.',dlist(j).name),'.');
        end
    end
end
end

function [vsStruct comparch] = setupCompileTools()
vsStruct.vstoolroot = getenv('VS120COMNTOOLS');
if ( isempty(vsStruct.vstoolroot) )
    error('Cannot compile without Visual Studio 2012');
end

setenv('MATLAB_DIR', matlabroot());

comparch = computer('arch');
if ( strcmpi(comparch,'win64') )
    vsStruct.buildbits = '64';
    vsStruct.buildenv = fullfile(vsStruct.vstoolroot,'..','..','vc','bin','amd64','vcvars64.bat');
    vsStruct.buildplatform = 'x64';
else
    error('Only windows 64-bit builds are currently supported');
end

system(['"' vsStruct.buildenv '"' ]);
clear mex;
end

function outputFile = compileMEX(projectName, vsStruct)
compileTime = tic();
outputFile = [projectName '.mexw64' vsStruct.buildbits];
fprintf('\nVisual Studio Compiling: %s...\n', outputFile);

projectRoot = fullfile('..','c',projectName);

result = system(['"' fullfile(vsStruct.vstoolroot,'..','IDE','devenv.com') '"' ' /rebuild "Release|' vsStruct.buildplatform '" "' projectRoot '.sln"']);
if ( result ~= 0 )
    error([projectName ': MEX compile failed.']);
end

%system(['copy ' fullfile(projectRoot, ['Release_' vsStruct.buildplatform], [projectName '.dll']) ' ' fullfile('.', [projectName '.mexw64' vsStruct.buildbits])]);
fprintf('Done %s\n\n', printTime(toc(compileTime)));
end

function outputFile = compileMATLAB(projectName, bindir, extrasList, toolboxList)
compileTime = tic();

outputFile = [upper(projectName) '.exe'];

if ( ~exist('extrasList','var') )
    extrasList = {};
end
extrasList = vertcat({'LEVER3D_logo.png'}, extrasList);

extraCommand = '';
if ( ~isempty(extrasList) )
    extraElems = cellfun(@(x)([' -a ' x]),extrasList, 'UniformOutput',0);
    extraCommand = [extraElems{:}];
end

if ( ~exist('toolboxList','var') )
    toolboxList = {};
end

toolboxAddCommand = '';
if ( ~isempty(toolboxList) )
    toolboxElems = cellfun(@(x)([' -p "' x '"']), toolboxList, 'UniformOutput',0);
    toolboxAddCommand = ['-N' toolboxElems{:}];
end

fprintf('\nMATLAB Compiling: %s...\n', outputFile);
result = system(['mcc -v -R -startmsg -m ' projectName '.m ' toolboxAddCommand extraCommand]);
if ( result ~= 0 )
    error([projectName ': MATLAB compile failed.']);
end

system(['copy ' projectName '.exe ' fullfile(bindir,'.')]);

fprintf('Done %s\n', printTime(toc(compileTime)));
end

function enableStartupScripts(bEnable)
searchPrefix = '';
renamePrefix = 'disabled_';
if ( bEnable )
    searchPrefix = 'disabled_';
    renamePrefix = '';
end

searchName = [searchPrefix 'startup.m'];
newName = [renamePrefix 'startup.m'];

startupScripts = findFilesInPath(searchName, userpath);
for i=1:length(startupScripts)
    scriptPath = fileparts(startupScripts{i});
    movefile(startupScripts{i}, fullfile(scriptPath,newName));
end
end

function fullNames = findFilesInPath(filename, searchPaths)
fullNames = {};

chkPaths = [];
while ( ~isempty(searchPaths) )
    [newPath remainder] = strtok(searchPaths, pathsep);
    if ( isempty(newPath) )
        searchPaths = remainder;
        continue;
    end
    
    chkPaths = [chkPaths; {newPath}];
    searchPaths = remainder;
end

for i=1:length(chkPaths)
    chkFullPath = fullfile(chkPaths{i}, filename);
    if ( exist(chkFullPath, 'file') )
        fullNames = [fullNames; {chkFullPath}];
    end
end
end
