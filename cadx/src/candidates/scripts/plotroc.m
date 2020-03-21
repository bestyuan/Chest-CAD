% plotroc(filename, totalnodules, linestyle)
%
% This function plots an ROC curve, given output from
% the extract_testing_data, extract_test_features_data
% scripts or the gmtest matlab function.
%
% filename should be the file produced by one of the
% above scripts. totalnodules is the total number of
% probable and possible nodules in the ground truth
% (i.e. 58 for the 60 image set and 115 for the 77 image
% set). style is a line style (see PLOT) that is used
% to plot the ROC curve. (e.g. 'k-')
%

function roc=plotroc(filename, totalnodules, style)

field=3;

d=dlmread(filename, ' ');

ds = sortrows(d, field);

[m n]=size(ds);

roc = size(m, field);

ds2 = ds(m:-1:1, :);

for i=1:m,
  this_ds2 = ds2(1:i, :);
  [truepos n] = size(this_ds2(this_ds2(:,4) >=2));
  [falsepos n] = size(this_ds2(this_ds2(:,4) < 1));
  % don't count possible (==1) on purpose
  roc(i, 1) = ds2(i, field);
  roc(i, 3) = truepos / totalnodules;
  roc(i, 2) = falsepos;
end

plot(roc(:, 2), roc(:, 3), style);