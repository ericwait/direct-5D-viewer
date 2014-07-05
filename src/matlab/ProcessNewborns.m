function ProcessNewborns(maxCenterOfMassDistance,maxPixelDistance)
global Families Tracks Hulls Costs

minFamilyTimeFrame = 3;
minParentCandidateTimeFrame = 5;
maxFrameDifference = 1;
minParentFuture = 3;
minParentHistoryTimeFrame = 5;

families = 1:length(Families);

tStart = 2;

rootHull = 0;
% if ( isfield(Figures, 'tree') &&  Figures.tree.familyID>0 ...
%         && Figures.tree.familyID<=length(Families))
%     rootTrackID = Families(Figures.tree.familyID).rootTrackID;
%     if ( ~isempty(rootTrackID) )
%         rootHull = Tracks(rootTrackID).hulls(1);
%     end
% end

% GlobalPatching();

%costMatrix = GetCostMatrix();
costMatrix = Costs;

planes = [];

size = length(families);
for i=1:size
    if ( isempty(Families(families(i)).startFrame) )
        continue;
    end
    
    if ( Families(families(i)).startFrame < tStart )
        continue;
    end
    
    %The root of the track to try to connect with another track
    childTrackID = Families(families(i)).rootTrack;
    familyTimeFrame = Families(families(i)).endFrame - Families(families(i)).startFrame;

    %Get all the possible hulls that could have been connected
    childHullID = Tracks(childTrackID).hulls(1);
    if(childHullID>length(costMatrix) || childHullID==0),continue,end
    parentHullCandidates = find(costMatrix(:,childHullID));

    % Don't consider deleted hulls as parents
%     bDeleted = [Hulls(parentHullCandidates).deleted];
%     parentHullCandidates = parentHullCandidates(~bDeleted);
    
    if(isempty(parentHullCandidates)),continue,end
    if(minFamilyTimeFrame >= familyTimeFrame),continue,end

    %Get the costs of the possible connections
    parentCosts = costMatrix(parentHullCandidates,childHullID);
    
    %Massage the costs a bit
    for j=1:length(parentHullCandidates)
        %Get the length of time that the parentCandidate exists
        parentTrackID = Hulls(parentHullCandidates(j)).track;
        if(isempty(parentTrackID)),continue,end
        parentTrackTimeFrame = Tracks(parentTrackID).endFrame - Tracks(parentTrackID).startFrame;

        %Change the cost of the candidates
        if(minParentCandidateTimeFrame >= parentTrackTimeFrame)
            parentCosts(j) = Inf;
        elseif(maxFrameDifference < abs(Tracks(childTrackID).startFrame - Hulls(parentHullCandidates(j)).frame))
            parentCosts(j) = Inf;
        elseif(minParentFuture >= Tracks(parentTrackID).endFrame - Hulls(parentHullCandidates(j)).frame)
            parentCosts(j) = Inf;
%         elseif(~isempty(GetTimeOfDeath(parentTrackID)))
%             parentCosts(j) = Inf;
        else
            siblingHullIndex = Hulls(childHullID).frame - Tracks(parentTrackID).startFrame + 1;
            % ASSERT ( siblingHullIndex > 0 && <= length(hulls)
            sibling = Tracks(parentTrackID).hulls(siblingHullIndex);
            parentCosts(j) = parentCosts(j) + SiblingDistance(childHullID,sibling,maxCenterOfMassDistance,maxPixelDistance);
%         end
%         if ( GraphEdits(parentHullCandidates(j),childHullID) > 0 )
%             parentCosts(j) = costMatrix(parentHullCandidates,childHullID);
        end
    end

    %Pick the best candidate
    parentCosts = full(parentCosts);
    [minCost index] = min(parentCosts(find(parentCosts)));
    if(isinf(minCost)),continue,end
    
    % Do not allow reconnect of removed edges
%     if ( GraphEdits(parentHullCandidates(index),childHullID) < 0 )
%         continue;
%     end
    
    parentHullID = parentHullCandidates(index);
    %Make the connections
    parentTrackID = Hulls(parentHullID).track;
    
    if(isempty(parentTrackID))
        continue
%         try
%             ErrorHandeling(['GetTrackID(' num2str(parentHullID) ') -- while in ProcessNewborns'],dbstack);
%             return
%         catch errorMessage2
%             fprintf('%s',errorMessage2);
%             return
%         end
    end
    
    % If the parent future is long enough create a mitosis
    connectTime = Hulls(parentHullID).frame+1;
    if(minParentHistoryTimeFrame < abs(Tracks(childTrackID).startFrame - Tracks(parentTrackID).startFrame))
        ChangeTrackParent(parentTrackID,connectTime,childTrackID);
        CreateCleavagePlane(GetRootHull(Tracks(parentTrackID).childrenTracks(1)),GetRootHull(Tracks(parentTrackID).childrenTracks(2)));
    end
end
end


