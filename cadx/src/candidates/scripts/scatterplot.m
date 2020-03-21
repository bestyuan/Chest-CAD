function A=scatterplot(file, f1, f2)

rawdata = dlmread(file);
truepos = (rawdata([find(rawdata(:,4)==3) ; find(rawdata(:,4)==2)], :));
								    
falsepos = (rawdata(find(rawdata(:,4)==0), :));
					   
f1 = f1 + 4;
f2 = f2 + 4;


hold off
plot(falsepos(:,f1), falsepos(:,f2), 'r.');
hold on
plot(truepos(:,f1), truepos(:,f2), 'bx');

legend('Non-nodule', 'Nodule', 4);
