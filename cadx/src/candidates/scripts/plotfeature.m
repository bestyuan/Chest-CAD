function A=plotfeature(file,f)

rawdata = dlmread(file);
truepos = (rawdata([find(rawdata(:,4)==3) ; find(rawdata(:,4)==2)],[3 ...
								    5 6 7 8 9 10 11 12 13 14 15 16 17]));
falsepos = (rawdata(find(rawdata(:,4)==0),[3 5 6 7 8 9 10 11 12 13 ...
					   14 15 16 17]));

[Y,X] = hist(truepos(:,f), 10);
figure(1)
plot(X, Y, 'r-')

[Y,X] = hist(falsepos(:,f), 100);
figure(2)
plot(X, Y, 'b-')
