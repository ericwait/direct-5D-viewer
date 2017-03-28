function [ poly ] = MakeEmptyStruct()
%[ poly ] = D3d.Polygon.MakeEmptyStruct()

poly = struct('index',{}, 'frame',{}, 'label',{}, 'color',{}, 'faces',{}, 'verts',{}, 'norms',{}, 'CenterOfMass',{});
end

