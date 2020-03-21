% A = gmtrain(training_filename)
%
% This is a primitive mechanism for training a GML classifier,
% based on data from ccICD, as processed through
% extract_training_data. 
%
% After training, the GML parameters are written to a file
% called trained.gml in the current directory.
%
% Note that you must edit this script in order to choose
% which features are to be included in the training.
%


function A=gmtrain(file)

rawdata = dlmread(file);

% must edit the numbers in the next two statements to choose which 
% features to use for training.
truepos = (rawdata([find(rawdata(:,4)==3) ; find(rawdata(:,4)==2)], ...
		   [ 5 6 7 10 11 12 13 14 22 23 ])); 
%		   [ 5 18 19 20 21 22 23 ])); 

falsepos = (rawdata(find(rawdata(:,4)==0),  [ 5 6 7 10 11 12 13 14 ...
					      22 23 ]));
%falsepos = (rawdata(find(rawdata(:,4)==0),  [ 5 18 19 20 21 22 23 ...
%					      ]));



fpmean = mean(falsepos);
fpcov = inv(cov(falsepos));
fpc = (cov(falsepos));
fplndet = log(abs(det(cov(falsepos))));

tpmean = mean(truepos);
tpcov = inv(cov(truepos));
tpc = (cov(truepos));
tplndet = log(abs(det(cov(truepos))));

A = struct('fpmean', fpmean, 'fpcov',  fpcov, 'tpmean',  tpmean, ...
	   'tpcov', tpcov, 'fplndet', fplndet, 'tplndet', tplndet, ...
	   'fpc', fpc, 'tpc', tpc);

fp = fopen('trained.gml', 'w');
fprintf(fp, '# These are parameters for a GML classifier\n');

fprintf(fp, 'class_count: 2\n');
[m n] = size(truepos);
fprintf(fp, 'feature_count: %d\n', n);

fprintf(fp, 'begin_class 0\n');
fprintf(fp, 'class_name: nodule\n');
fprintf(fp, 'sample_count: %d\n', m);
fprintf(fp, 'means: ');
fprintf(fp, '%0.10e ', tpmean);
fprintf(fp, '\ninv_covars: ');
fprintf(fp, '%0.10e ', tpcov);
fprintf(fp, '\nln_det: ');
fprintf(fp, '%0.10e ', tplndet);
fprintf(fp, '\n#\n');
fprintf(fp, 'begin_class 1\n');
fprintf(fp, 'class_name: non-nodule\n');
[m n] = size(falsepos);
fprintf(fp, 'sample_count: %d\n', m);
fprintf(fp, 'means: ');
fprintf(fp, '%0.10e ', fpmean);
fprintf(fp, '\ninv_covars: ');
fprintf(fp, '%0.10e ', fpcov);
fprintf(fp, '\nln_det: ');
fprintf(fp, '%0.10e ', fplndet);
