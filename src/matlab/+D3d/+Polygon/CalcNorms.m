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

function vertNormals = CalcNorms(vertList, faceList)
    vertNormals = zeros(size(vertList,1),3);
    for i=1:size(faceList,1)
        face = faceList(i,:);
        edges = [vertList(face(2),:)-vertList(face(1),:);
                 vertList(face(3),:)-vertList(face(1),:);
                 vertList(face(3),:)-vertList(face(2),:)];
        
        edgeLen = sqrt(sum(edges.^2,2));
        edges = edges ./ repmat(edgeLen,1,3);
        
        faceDir = [dot(edges(1,:),edges(2,:));
                   dot(edges(3,:),-edges(1,:));
                   dot(edges(2,:),edges(3,:))];
        
        faceAngles = acos(faceDir);
        
        faceVec = cross(edges(1,:), edges(2,:));
        faceNorm = faceVec / norm(faceVec);
        
        for k=1:3
            vertNormals(face(k),:) = vertNormals(face(k),:) + faceAngles(k)*faceNorm;
        end
    end
    vertLengths = sqrt(sum(vertNormals.^2,2));
    vertNormals = -(vertNormals ./ repmat(vertLengths,1,3));
end