function [ymin, ymax] = CalcMidRange(minX, maxX)
x1 = minX;
y1 = 0;
x3 = maxX;
y3 = 1;

x2 = max(0,min(1,(x1+x3)/2));
y2 = 0:0.01:1;

a = ((y2-y1)*(x1-x3) + (y3-y1)*(x2-x1)) ./ ((x1-x3)*(x2.^2-x1.^2) + (x2-x1)*(x3.^2-x1.^2));
b = ((y2-y1) - a*(x2.^2-x1.^2)) ./ (x2-x1);

mm = (2*a*x1+b>=0)&(2*a*x3+b>=0);
ymin = y2(find(mm,1,'first'));
ymax = y2(find(mm,1,'last'));
end