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

function SetDistances(distanceImage)
global Hulls imageData

for i=1:length(Hulls)
    if (~isempty(Hulls(i).pixelsOrg))
        idx = sub2ind([imageData.YDimension,imageData.XDimension,imageData.ZDimension,1,imageData.NumberOfFrames],...
            Hulls(i).pixelsOrg(:,2),Hulls(i).pixelsOrg(:,1),Hulls(i).pixelsOrg(:,3),ones(size(Hulls(i).pixelsOrg,1),1),...
            repmat(Hulls(i).frame,size(Hulls(i).pixelsOrg,1),1));
        distances = distanceImage(idx);
        Hulls(i).distances(1) = max(distances(:));
        Hulls(i).distances(2) = min(distances(:));
        Hulls(i).distances(3) = distanceImage(round(Hulls(i).centerOfMassOrg(1)),round(Hulls(i).centerOfMassOrg(2)),...
            round(Hulls(i).centerOfMassOrg(3)),1,Hulls(i).frame);
    else
        Hulls(i).distances(1) = -1;
        Hulls(i).distances(2) = -1;
        Hulls(i).distances(3) = -1;
    end
end
end

