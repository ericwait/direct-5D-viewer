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

function distance = SiblingDistance(cell1HullID,cell2HullID,maxCenterOfMassDistance,maxPixelDistance)

global Hulls

if 0 == cell1HullID || 0 == cell2HullID
    distance = Inf;
    return;
end

if Hulls(cell1HullID).frame ~= Hulls(cell2HullID).frame
    distance = Inf;
    return
end

xCell1 = Hulls(cell1HullID).pixels(:,1);
yCell1 = Hulls(cell1HullID).pixels(:,2);
zCell1 = Hulls(cell1HullID).pixels(:,3);
xCell2 = Hulls(cell2HullID).pixels(:,1);
yCell2 = Hulls(cell2HullID).pixels(:,2);
zCell2 = Hulls(cell2HullID).pixels(:,3);

minPixelDistance = Inf;

for i=1:length(yCell1)
    distanceSqr = (yCell2 - yCell1(i)).^2 + (xCell2-xCell1(i)).^2 + (zCell2-zCell1(i)).^2;
    curMinDistance = sqrt(min(distanceSqr));
    if(curMinDistance < minPixelDistance)
        minPixelDistance = curMinDistance;
    end
end

distanceCenterOfMass = norm(Hulls(cell1HullID).centerOfMass - Hulls(cell2HullID).centerOfMass);

distance = distanceCenterOfMass + 1000 * minPixelDistance;

if(distanceCenterOfMass > maxCenterOfMassDistance ||...
        minPixelDistance > maxPixelDistance)
    distance = inf;
end
end
