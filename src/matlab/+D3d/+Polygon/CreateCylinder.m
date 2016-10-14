function [faces, verts, norms] = CreateCylinder(betweenPts_xy,width)
    if (~exist('width','var') || isempty(width))
        width = 0.025;
    end

    [arrowFaces, arrowVerts, arrowNorms] = D3d.Polygon.MakeArrow(1.0,width,0.01,40);

    v = betweenPts_xy(1,:)-betweenPts_xy(2,:);
    vU = v./norm(v);

    vCz = cross(vU,[0,0,1]);
    theta = acos(vU(3));
    vCz = vCz ./ norm(vCz);

    c = cos(theta);
    s = sin(theta);
    C = 1-c;

    rotMatrix = [vCz(1)^2*C+c,             vCz(1)*vCz(2)*C-vCz(3)*s, vCz(1)*vCz(3)*C+vCz(2)*s;
                 vCz(2)*vCz(1)*C+vCz(3)*s, vCz(2)^2*C+c,             vCz(2)*vCz(3)*C-vCz(1)*s;
                 vCz(3)*vCz(1)*C-vCz(2)*s, vCz(3)*vCz(2)*C+vCz(1)*s, vCz(3)^2*C+c];

    verts = arrowVerts*rotMatrix;
    norms = arrowNorms*rotMatrix;

    verts = verts*norm(v);
    verts = verts+repmat(betweenPts_xy(2,:),size(verts,1),1);

    faces = arrowFaces;
end