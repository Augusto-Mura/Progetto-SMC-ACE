%M = readmatrix('basic_matrix.txt')
%M = readmatrix('led01.csv')
%M = readmatrix('led02.csv','text')
I = readtable('led01.csv');
I = I(:, 3);
%M = M(219:230)
I = I(204:end,1);

%stackedplot(I)

V = (0:0.005:4)';
I = Array(I);

figure
plot(V,I);
