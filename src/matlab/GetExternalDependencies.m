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

function [toolboxStruct externalStruct calledNames callGraph] = GetExternalDependencies(chkPath)
    if ( ~exist('chkPath', 'var') )
        chkPath = pwd();
    end
    
    [localNames localFileNames] = getLocalNames(chkPath);
    [fullNames calledFrom] = recursiveGetDeps(localFileNames);
    
    bIsLocal = cellfun(@(x)(~isempty(strfind(x,chkPath))), fullNames);
    localNodeList = find(bIsLocal);
    
    [toolboxes toolboxMap] = getMatlabToolboxes(fullNames);
    toolboxFuncs = transpose(arrayfun(@(x)(fullNames(toolboxMap == x)), 1:length(toolboxes), 'UniformOutput',0));
    
    bIsMatlab = (toolboxMap > 0);
    [externalDeps externalMap] = getOtherDeps(fullNames, bIsMatlab, bIsLocal);
    externalFuncs = transpose(arrayfun(@(x)(fullNames(externalMap == x)), 1:length(externalDeps), 'UniformOutput',0));
    
    fullGraph = createCallGraph(calledFrom);
    bIsCalled = findCalledNodes(localNodeList, fullGraph);
    
    toolboxCallers = getToolboxCallers(localNodeList, toolboxes, toolboxMap, fullGraph, fullNames);
    externalCallers = getToolboxCallers(localNodeList, externalDeps, externalMap, fullGraph, fullNames);
    
%     calledNames = fullNames(bIsCalled);
%     callGraph = fullGraph(bIsCalled,bIsCalled);
    callToolboxMap = toolboxMap(bIsCalled);
    callExternalMap = externalMap(bIsCalled);
    
%     [squashNames squashGraph squashMap] = squashToolboxNodes(calledNames, callGraph, toolboxes, callToolboxMap);
    
%     usedToolboxes = unique(callToolboxMap(callToolboxMap>0));
%     toolboxes = toolboxes(usedToolboxes);
%     toolboxFuncs = toolboxFuncs(usedToolboxes);
%     toolboxCallers = toolboxCallers(usedToolboxes);
    
    usedExternal = unique(callExternalMap(callExternalMap>0));
    externalDeps = externalDeps(usedExternal);
    externalFuncs = externalFuncs(usedExternal);
    externalCallers = externalCallers(usedExternal);
    
    toolboxStruct = struct('deps', {toolboxes}, 'funcs', {toolboxFuncs}, 'callers',{toolboxCallers});
    externalStruct = struct('deps', {externalDeps}, 'funcs', {externalFuncs}, 'callers',{externalCallers});
end

function [toolboxes toolboxMap] = getMatlabToolboxes(funcNames)
    bIsMatlab = strncmpi(matlabroot, funcNames, length(matlabroot));
    matlabIdx = find(bIsMatlab);
    matlabNames = funcNames(bIsMatlab);
    
    % Add default matlab dependencies first, these will always be used
    toolboxes = {'matlab';'local';'shared'};
    toolboxMap = zeros(length(funcNames),1);
    for i=1:length(matlabNames)
        toolboxPattern = '(?<=^.+[\\/]toolbox[\\/])[^\\/]+';
        matchToolbox = regexp(matlabNames{i}, toolboxPattern, 'match', 'once');
        if ( isempty(matchToolbox) )
            continue;
        end
        
        if ( any(strcmpi(matchToolbox,toolboxes)) )
            toolboxMap(matlabIdx(i)) = find(strcmpi(matchToolbox,toolboxes));
            continue;
        end
        
        toolboxes = [toolboxes; {matchToolbox}];
        toolboxMap(matlabIdx(i)) = length(toolboxes);
    end
end

function [externalDeps externalMap] = getOtherDeps(funcNames, bIsMatlab, bIsLocal)
    externalMap = zeros(length(funcNames),1);
    externalDeps = {};
    
    bExternal = (~bIsMatlab & ~bIsLocal);
    externalFuncs = funcNames(bExternal);
    
    tokenRemains = externalFuncs;
    tokenDelims = filesep;
    
    pred = [];
    bLeaves = false(0,0);
    
    pathNodes = {};
    nodeMap = ((1:length(externalFuncs)).');
    sharedPred = zeros(length(externalFuncs),1);
    
    depth = 1;
    nextPred = zeros(length(externalFuncs),1);
    bEmpty = false;
    while ( ~all(bEmpty) )
        [tokenNodes tokenRemains] = strtok(tokenRemains, tokenDelims);
        
        bEmpty = cellfun(@(x)(isempty(x)), tokenNodes);
        
        tokenNodes = tokenNodes(~bEmpty);
        tokenRemains = tokenRemains(~bEmpty);
        
        nextPred = nextPred(~bEmpty);
        nodeMap = nodeMap(~bEmpty);
        
        [newNodes ia ic] = unique(tokenNodes);
        
        bEmptyLeaf = cellfun(@(x)(isempty(x)), tokenRemains);
        bNewLeaves = bEmptyLeaf(ia);
        
        pred = [pred; nextPred(ia)];
        
        % As soon as we hit a leaf node, update shared predecessor for all
        % nodes. This will be condsidered the toolbox path.
        leafPreds = unique(nextPred(ia(bNewLeaves)));
        [bShared setPred] = ismember(nextPred, leafPreds);
        
        bUpdateShared = (sharedPred(nodeMap) == 0) & bShared;
        sharedPred(nodeMap(bUpdateShared)) = leafPreds(setPred(bUpdateShared));
        
        nextPred = ic + length(pathNodes);
        
        bLeaves = [bLeaves; bNewLeaves];
        pathNodes = [pathNodes; newNodes];
        
        depth = depth + 1;
    end
    
    [sharedPath ia ic] = unique(sharedPred);
    externalDeps = cell(length(sharedPath),1);
    for i=1:length(sharedPath)
        curIdx = sharedPath(i);
        while ( curIdx ~= 0 )
            externalDeps{i} = fullfile(pathNodes{curIdx}, externalDeps{i});
            curIdx = pred(curIdx);
        end
    end
    
    externalMap(bExternal) = ic;
end

function toolboxCallers = getToolboxCallers(localNodes, toolboxes, toolboxMap, fullGraph, fullNames)
    toolboxCallers = cell(length(toolboxes),1);
    for i=1:length(toolboxes)
        bInToolbox = (toolboxMap == i);
        [callerIdx funcIdx] = find(fullGraph(:,bInToolbox));
        toolboxCallers{i} = cell(max(funcIdx),1);
        for j=1:length(funcIdx)
            if ( ~ismember(callerIdx(j),localNodes) )
                continue;
            end
            
            toolboxCallers{i}{funcIdx(j)} = [toolboxCallers{i}{funcIdx(j)}; fullNames(callerIdx(j))];
        end
    end
end

function callGraph = createCallGraph(calledFrom)
    numEdges = cellfun(@(x)(length(x)), calledFrom);
    calledIdx = arrayfun(@(x,y)(y*ones(1,x)), numEdges, ((1:length(calledFrom)).'), 'UniformOutput',0);
    
    jIdx = ([calledIdx{:}]);
    iIdx = ([calledFrom{:}]);
    
    callGraph = sparse(iIdx,jIdx, ones(1,sum(numEdges)), length(calledFrom),length(calledFrom), sum(numEdges));
end

function bIsCalled = findCalledNodes(localFuncIdx, callGraph)
    bIsCalled = false(size(callGraph,1),1);
    bIsCalled(localFuncIdx) = 1;
    
    for i=1:length(localFuncIdx)
        [d pred] = dijkstra_sp(callGraph, localFuncIdx(i));
        
        bHasPath = ~isinf(d);
        bIsCalled = (bIsCalled | bHasPath);
    end
end

function [squashNames squashGraph squashMap] = squashToolboxNodes(funcNames, callGraph, toolboxes, toolboxMap)
    squashNames = funcNames;
    squashMap = toolboxMap;
    squashGraph = callGraph;
    
    for i=1:length(toolboxes)
        bInToolbox = (squashMap == i);
        
        tempRows = any(squashGraph(bInToolbox,:),1);
        squashRows = [tempRows(~bInToolbox) any(tempRows(bInToolbox))];
        
        tempCols = any(squashGraph(:,bInToolbox),2);
        squashCols = tempCols(~bInToolbox);
        
        squashGraph = [squashGraph(~bInToolbox,~bInToolbox) squashCols; squashRows];
        
        squashNames = [squashNames(~bInToolbox); toolboxes(i)];
        squashMap = [squashMap(~bInToolbox); i];
    end
end

function [deplist calledFrom] = recursiveGetDeps(checkNames, bFollowToolboxes, deplist, calledFrom)
    if ( ~exist('calledFrom','var') )
        calledFrom = {};
    end
    
    if ( ~exist('deplist','var') )
        deplist = {};
    end
    
    if ( ~exist('bFollowToolboxes','var') )
        bFollowToolboxes = 0;
    end
    
    if ( isempty(checkNames) )
        return;
    end
    
    % Get single-link dependencies
    try
        [newdeps, builtins, classes, prob_files, prob_sym, eval_strings, newCalledFrom, java_classes] = depfun(checkNames, '-toponly', '-quiet');
    catch excp
        newdeps = cell(0,1);
        newCalledFrom = cell(0,1);
    end
    [deplist calledFrom newEntries] = mergeLists(deplist, calledFrom, newdeps, newCalledFrom);
    
    % Remove any matlab toolbox entries from being recursively followed
    if ( ~bFollowToolboxes )
        bIsToolbox = strncmpi(matlabroot, newEntries, length(matlabroot));
        newEntries = newEntries(~bIsToolbox);
    end
    
    [deplist calledFrom] = recursiveGetDeps(newEntries, bFollowToolboxes, deplist, calledFrom);
    
end

function [deplist calledFrom newEntries] = mergeLists(deplist, calledFrom, newdeps, newCalledFrom)
    oldIdx = cellfun(@(x)(find(strcmpi(x,deplist))), newdeps, 'UniformOutput',0);
    bNew = cellfun(@(x)(isempty(x)), oldIdx);
    
    newEntries = newdeps(bNew);
    
    idxMap = zeros(1,length(newdeps));
    idxMap(~bNew) = [oldIdx{~bNew}];
    idxMap(bNew) = (1:length(newEntries)) + length(deplist);
    
    deplist = [deplist; newEntries];
    calledFrom = [calledFrom; cell(length(newEntries),1)];
    
    calledFrom(idxMap) = cellfun(@(x,y)(unique([x idxMap(y)])), calledFrom(idxMap), newCalledFrom, 'UniformOutput',0);
end

function [checkNames fullNames] = getLocalNames(dirName, packageString)
    if ( ~exist('packageString','var') )
        packageString = '';
    end

    matlabFiles = what(dirName);
    
    funcFileNames = vertcat(matlabFiles.m);
    funcFileNames = [funcFileNames; vertcat(matlabFiles.mex)];
    
    checkNames = cellfun(@splitFName, funcFileNames, 'UniformOutput',0);
    checkNames = cellfun(@(x)([packageString x]), checkNames, 'UniformOutput',0);
    fullNames = cellfun(@(x)(fullfile(dirName,x)), funcFileNames, 'UniformOutput',0);
    
    for i=1:length(matlabFiles.packages)
        nextPackageString = makePackageString(packageString, matlabFiles.packages{i});
        [pckgCheckNames pckgFullNames] = getLocalNames(fullfile(dirName, ['+' matlabFiles.packages{i}]), nextPackageString);
        
        checkNames = [checkNames; pckgCheckNames];
        fullNames = [fullNames; pckgFullNames];
    end
end

function packageString = makePackageString(packageString, nextPackage)
    if ( isempty(packageString) )
        packageString = [nextPackage '.'];
        return;
    end
    
    packageString = [packageString nextPackage '.'];
end

function name = splitFName(fileName)
    [path, name] = fileparts(fileName);
end
