% A = gmtrain(test_filename, gml_classifier)
%
% This is a primitive mechanism for testing a GML classifier
% on output from ccICD. This script runs the GML classifier
% with a leave-one-out method. This script is old; the better
% way to test the classifier now is to run ccICD with the
% -gml option, then use plotroc directly to view the results.
%
% test_filename is a file produced from the
% extract_test_features_data script. gml_classifier is a 
% classifier object returned by gmtrain.
%
% This script produces a file called outdata. This filename
% may then be passed to plotroc to view the roc plot.
%
% Note that you must edit this script in order to choose
% which features are to be included in the testing.
%

function A=gmtest(file, A)

rawdata = dlmread(file);
data = rawdata(:, [  5 6 7 10 11 12 13 14 22 23 ]);

data(1,:)
[m feature_count] = size(data);

truepos = (rawdata([find(rawdata(:,4)==3) ; find(rawdata(:,4)==2)],[...
								    4]));
falsepos = (rawdata(find(rawdata(:,4)==0),[4]));

[nod_sample_count n] = size(truepos);
[nonnod_sample_count n] = size(falsepos);

nonnod_sample_count
nod_sample_count
m

for i=1:m
  if mod(i,1000) == 0
    i
  end
  
  data_norm_nod = data(i, :) - A.tpmean;
  data_norm_nonnod = data(i, :) - A.fpmean;
  
  dMahalSqr_n = data_norm_nod * A.tpcov * data_norm_nod';
  dMahalSqr_nn = data_norm_nonnod * A.fpcov * data_norm_nonnod';
  
  n_result = 0.5 * dMahalSqr_n + 0.5 * A.tplndet;
  nn_result = 0.5 * dMahalSqr_nn + 0.5 * A.fplndet;

  % if it's a nodule, change the n_result
  if true
  if(rawdata(i, 4) == 2 | rawdata(i, 4) == 3)
    n_result = n_result + compute_loo_adjustment(nod_sample_count, ...
						 dMahalSqr_n, feature_count);
  elseif(rawdata(i,4) == 0)
    nn_result = nn_result + compute_loo_adjustment(nonnod_sample_count, ...
						   dMahalSqr_nn, feature_count);
  end
  end
  
  n_result3 = 0.5 * n_result + 0.5 * A.tplndet;
  nn_result3 = 0.5 * nn_result + 0.5 * A.fplndet;

  n_p = exp(-n_result3);
  nn_p = exp(-nn_result3);

  p = [n_p nn_p];
  psum = sum(p');
  p(:,1) = (p(:,1)' ./ psum)';
  p(:,2) = (p(:,2)' ./ psum)';

  outdata(i, :) = [rawdata(i,1) rawdata(i,2) p(:,1) rawdata(i,4)];



end

dlmwrite('outdata', outdata, ' ');
