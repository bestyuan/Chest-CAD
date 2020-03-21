function g = compute_loo_adjustment(samples_in_class, dist_sqr, feature_count)

a=(samples_in_class*samples_in_class-3.0*samples_in_class+1.0)*dist_sqr / (samples_in_class-1.0);
b=samples_in_class*dist_sqr*dist_sqr;
c=(samples_in_class-1.0)*(samples_in_class-1.0)-samples_in_class*dist_sqr;
d=0.5*feature_count*log(1.0+1.0/(samples_in_class-2.0));
e=0.5*log(1.0-samples_in_class*dist_sqr/((samples_in_class-1.0)*(samples_in_class-1.0)));
g=0.5*(a+b)/c + d + e;