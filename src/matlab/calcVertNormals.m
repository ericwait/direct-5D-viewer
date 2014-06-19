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