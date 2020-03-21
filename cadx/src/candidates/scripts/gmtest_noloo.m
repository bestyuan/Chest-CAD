function A=gmtest(file, A)

rawdata = dlmread(file);
data = rawdata(:, [3 5 6 7 8 9 10 11 12 13 14 15 16 17]);

[m n] = size(data);

data_norm_nod = data - ones(m, 1) * A.tpmean;
data_norm_nonnod = data - ones(m, 1) * A.fpmean;

nod_result = data_norm_nod * A.tpcov * data_norm_nod';
nonnod_result = data_norm_nonnod * A.fpcov * data_norm_nonnod';

n_result2 = diag(nod_result, 0);
nn_result2 = diag(nonnod_result, 0);

n_result3 = 0.5 * n_result2 + 0.5 * A.tplndet;
nn_result3 = 0.5 * nn_result2 + 0.5 * A.fplndet;

n_p = exp(-n_result3);
nn_p = exp(-nn_result3);

p = [n_p nn_p];
psum = sum(p');
p(:,1) = (p(:,1)' ./ psum)';
p(:,2) = (p(:,2)' ./ psum)';

outdata = [rawdata(:,1) rawdata(:,2) p(:,1) rawdata(:,4)];

dlmwrite('outdata', outdata, ' ');