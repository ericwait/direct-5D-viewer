function [sphereFaces,sphereVerts,sphereNormals] = MakeSphere(radiusSphere, nSphereFaces)
    sphereFaces = [];
    sphereVerts = [];

    if (radiusSphere<0 || radiusSphere>1)
        error('All arguments must be on the interval [0,1]');
    end
    
    [x,y,z] = sphere(nSphereFaces);
    for i=1:(size(x,1)-1)
        for j=1:(size(x,2)-1)
            quadVerts = [x(i,j) y(i,j) z(i,j); x(i+1,j) y(i+1,j) z(i+1,j);
                        x(i+1,j+1) y(i+1,j+1) z(i+1,j+1); x(i,j+1) y(i,j+1) z(i,j+1)];
            
            quadVerts = radiusSphere * quadVerts;
            quadIdx = zeros(1,4);
            for k=1:4
                [quadIdx(k), sphereVerts] = addVert(quadVerts(k,:), sphereVerts);
            end
            
            if ( quadIdx(1) == quadIdx(4) )
                sphereFaces = [sphereFaces; quadIdx(1) quadIdx(3) quadIdx(2)];
                continue;
            end
            
            if ( quadIdx(2) == quadIdx(3) )
                sphereFaces = [sphereFaces; quadIdx(1) quadIdx(4) quadIdx(2)];
                continue;
            end
            
            sphereFaces = [sphereFaces; quadIdx(1) quadIdx(3) quadIdx(2)];
            sphereFaces = [sphereFaces; quadIdx(1) quadIdx(4) quadIdx(3)];
        end
    end
    sphereNormals = calcVertNormals(sphereVerts, sphereFaces);
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
