function [ inds, verts] = CreatePlane(norm, point, dimX, dimY)
%CreatePlane takes the normal and a point in the plane. dimX and dimY are
%the sizes of the orginal plane in the x,y plane.
%   :

verts = zeros(20*20,3);

numpts = 1;
for x=-floor(10):ceil(10)
    for y=-floor(10):ceil(10)
        xVert = x * dimX / 20;
        yVert = y * dimY / 20;
        verts(numpts,:) = [xVert yVert 0];
        numpts = numpts+1;
    end
end

dt = delaunayTriangulation(verts(:,1:2));

inds = dt.ConnectivityList;

unitNorm = norm./sqrt(sum(norm.^2));

tX = acos(unitNorm(3)); %Theta around the X axis
% tY = atan2(unitNorm(2),unitNorm(1)); %Theta around the Y axis
tZ = atan2(unitNorm(1),unitNorm(2)); %Theta around the Z axis

mX = [
    1       0           0           0;
    0       cos(tX)     sin(tX)    0;
    0       -sin(tX)     cos(tX)     0;
    0       0           0           1];

% mY = [
%     cos(tY) 0           -sin(tY)     0;
%     0       1           0           0;
%    sin(tY) 0           cos(tY)     0;
%     0       0           0           1];

mZ = [
    cos(tZ) sin(tZ)    0           0;
    -sin(tZ) cos(tZ)     0           0;
    0       0           1           0;
    0       0           0           1];

translateMatrix = [
    1 0 0 point(1);
    0 1 0 point(2);
    0 0 1 point(3);
    0 0 0 1;];

transformMatrix = translateMatrix*mZ*mX;

verts = [verts ones(size(verts,1),1)]';

transVertList = transformMatrix*verts;
verts = transVertList';

end