function [polygon] = MakePlaneBetween(pt1_xy,pt2_xy,polygonListIndex,strLabel,frame,sideLength,depth)
	if (~exist('sideLength','var') || isempty(sideLength))
	    sideLength = 10;
	end
	if (~exist('depth','var') || isempty(depth))
	    depth = 0.25;
	end

	polygon = D3d.Polygon.MakeEmptyStruct();

	normVec = pt1_xy - pt2_xy;
	centerPoint = (pt1_xy + pt2_xy)./2;

	[ inds, verts] = D3d.Polygon.CreatePlane(normVec, centerPoint, sideLength, sideLength);

	polygon.faces = inds;
	polygon.verts = verts;
	polygon.norms = repmat(normVec,size(verts,1),1);

	polygon.CenterOfMass = centerPoint;

	polygon.color = [1,1,1,1];
	polygon.index = polygonListIndex;
	polygon.label = strLabel;
	polygon.frame = frame;
end
