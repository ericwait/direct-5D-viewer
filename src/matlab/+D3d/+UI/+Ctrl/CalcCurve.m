function [x, y, a, b, c] = CalcCurve(vis,minX, midY, maxX)
x1 = minX;
y1 = 0;
x3 = maxX;
y3 = 1;
x2 = max(0,min(1,(x1+x3)/2));
y2 = midY;

a = ((y2-y1)*(x1-x3) + (y3-y1)*(x2-x1)) / ((x1-x3)*(x2.^2-x1.^2) + (x2-x1)*(x3.^2-x1.^2));
b = ((y2-y1) - a*(x2.^2-x1.^2)) / (x2-x1);
c = y1 - a*x1.^2 - b*x1;
x = minX:0.01:maxX;
y = a*x.^2 + b*x + c;

end