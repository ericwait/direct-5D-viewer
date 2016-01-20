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

function [faces, verts, normals] = MakeArrow(heightCylinder, radiusCylinder, radiusCone, nArrowFaces)
cylVerts = [];
cylFaces = [];
capVerts = [];
capFaces = [];

if (heightCylinder>1 || heightCylinder<=0)
    error('All arguments must be on the interval (0,1]');
end

for i=1:nArrowFaces
    theta = (2*pi/(nArrowFaces))*[(i-1) i];
    costh = cos(theta);
    sinth = sin(theta);
    quadVerts = [radiusCylinder*costh(1) radiusCylinder*sinth(1) 0
        radiusCylinder*costh(1) radiusCylinder*sinth(1) heightCylinder
        radiusCylinder*costh(2) radiusCylinder*sinth(2) heightCylinder
        radiusCylinder*costh(2) radiusCylinder*sinth(2) 0];
    
    capTri = [0 0 0;
        radiusCylinder*costh(2) radiusCylinder*sinth(2) 0;
        radiusCylinder*costh(1) radiusCylinder*sinth(1) 0];
    
    capIdx = zeros(1,3);
    for k=1:3
        [capIdx(k), capVerts] = addVert(capTri(k,:), capVerts);
    end
    capFaces = [capFaces; capIdx];
    
    quadIdx = zeros(1,4);
    for k=1:4
        [quadIdx(k), cylVerts] = addVert(quadVerts(k,:), cylVerts);
    end
    
    cylFaces = [cylFaces; quadIdx(1) quadIdx(3) quadIdx(2)];
    cylFaces = [cylFaces; quadIdx(1) quadIdx(4) quadIdx(3)];
    
    faceNorm = cross(cylVerts(quadIdx(3),:)-cylVerts(quadIdx(1),:), cylVerts(quadIdx(2),:)-cylVerts(quadIdx(1),:));
    faceNorm = faceNorm / norm(faceNorm);
end

cylFaces = [cylFaces; capFaces + size(cylVerts,1)];
cylVerts = [cylVerts; capVerts];
cylNormals = calcVertNormals(cylVerts, cylFaces);


coneVerts = [];
coneFaces = [];
for i=1:nArrowFaces
    theta = (2*pi/(nArrowFaces))*[(i-1) i];
    costh = cos(theta);
    sinth = sin(theta);
    triVerts = [0 0 1; radiusCone*costh(1) radiusCone*sinth(1) heightCylinder
        radiusCone*costh(2) radiusCone*sinth(2) heightCylinder];
    
    triIdx = zeros(1,3);
    for k=1:3
        [triIdx(k), coneVerts] = addVert(triVerts(k,:), coneVerts);
    end
    coneFaces = [coneFaces; triIdx(1) triIdx(2) triIdx(3)];
end
coneNormals = calcVertNormals(coneVerts, coneFaces);

hookupVerts = [];
hookupFaces = [];
for i=1:nArrowFaces
    theta = (2*pi/(nArrowFaces))*[(i-1) i];
    costh = cos(theta);
    sinth = sin(theta);
    quadVerts = [radiusCylinder*costh(1) radiusCylinder*sinth(1) heightCylinder
        radiusCone*costh(1) radiusCone*sinth(1) heightCylinder
        radiusCone*costh(2) radiusCone*sinth(2) heightCylinder
        radiusCylinder*costh(2) radiusCylinder*sinth(2) heightCylinder];
    
    quadIdx = zeros(1,4);
    for k=1:4
        [quadIdx(k), hookupVerts] = addVert(quadVerts(k,:), hookupVerts);
    end
    
    hookupFaces = [hookupFaces; quadIdx(1) quadIdx(3) quadIdx(2)];
    hookupFaces = [hookupFaces; quadIdx(1) quadIdx(4) quadIdx(3)];
end
hookupNormals = calcVertNormals(hookupVerts, hookupFaces);

faces = [cylFaces; coneFaces + size(cylVerts,1); hookupFaces + size(cylVerts,1) + size(coneVerts,1)];
verts = [cylVerts; coneVerts; hookupVerts];
normals = [cylNormals; coneNormals; hookupNormals];
end

function vertNormals = calcVertNormals(vertList, faceList)
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
    vertNormals = vertNormals ./ repmat(vertLengths,1,3);
end

function [vertIdx, vertList] = addVert(newVert, vertList)
    if ( isempty(vertList) )
        vertList = newVert;
        vertIdx = 1;
        return;
    end
    
    dist = ((vertList(:,1) - newVert(1)).^2 + (vertList(:,2) - newVert(2)).^2 + (vertList(:,3) - newVert(3)).^2);
    findIdx = find(dist < 1e-8);
    
    if ( ~isempty(findIdx) )
        vertIdx = findIdx;
        return;
    end
    
    vertList = [vertList; newVert];
    vertIdx = size(vertList,1);
end
