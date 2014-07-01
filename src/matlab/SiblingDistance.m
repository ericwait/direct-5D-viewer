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
