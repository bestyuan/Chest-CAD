
#include "fit.h"
#include "nrutil.h"     




void Fit::function(float, float [], int) {
	
	
	
	
	
	
}
     

/*
Given a set of data points x[1..ndat], y[1..ndat] with individual standard deviations
sig[1..ndat], use É‘2 minimization to .t for some or all of the coe.cients a[1..ma] of
a function that depends linearly on a, y = i ai Å~ afunci(x). The input array ia[1..ma]
indicates by nonzero entries those components of a that should be .tted for, and by zero entries
those components that should be held .xed at their input values. The program returns values
for a[1..ma], É‘2 = chisq, and the covariance matrix covar[1..ma][1..ma]. (Parameters
held .xed will return zero covariances.)Th e user supplies a routine funcs(x,afunc,ma) that
returns the ma basis functions evaluated at x = x in the array afunc[1..ma].
*/

void Fit::lfit(float x[], float y[], float sig[], int ndat, float a[], int ia[],
int ma, float **covar, float *chisq, void (*funcs)(float, float [], int))
{

	void gaussj(float **a, int n, float **b, int m);        
	
	int i,j,k,l,m,mfit=0;
	float ym,wt,sum,sig2i,**beta,*afunc;
	beta=matrix(1,ma,1,1);
	afunc=vector(1,ma);  
	
	for (j=1;j<=ma;j++)
		if (ia[j]) mfit++;
		if (mfit == 0) nrerror("lfit: no parameters to be fitted");
		for (j=1;j<=mfit;j++) { // Initialize the (symmetric)mat rix.
		for (k=1;k<=mfit;k++) covar[j][k]=0.0;
		beta[j][1]=0.0;
	}    
	
	
	for (i=1;i<=ndat;i++) { // Loop over data to accumulate coe.cients of the normal equations.

		(*funcs)(x[i],afunc,ma);
		ym=y[i];
		if (mfit < ma) { // Subtract o. dependences on known pieces of the .tting function. 
			for (j=1;j<=ma;j++)
			if (!ia[j]) ym -= a[j]*afunc[j];
		}  
		
		sig2i=1.0/SQR(sig[i]);
		for (j=0,l=1;l<=ma;l++) {
			if (ia[l]) {
				wt=afunc[l]*sig2i;
				for (j++,k=0,m=1;m<=l;m++)
				if (ia[m]) covar[j][++k] += wt*afunc[m];
				beta[j][1] += ym*wt;
			}
		}
	}        
	
	
	for (j=2;j<=mfit;j++) // Fill in above the diagonal from symmetry.
		for (k=1;k<j;k++)
			covar[k][j]=covar[j][k];  
		
	gaussj(covar,mfit,beta,1); // Matrix solution. 
	
	for (j=0,l=1;l<=ma;l++)
		if (ia[l]) a[l]=beta[++j][1]; // Partition solution to appropriate coecients          
		
	*chisq=0.0;   
	
	for (i=1;i<=ndat;i++) { // Evaluate chi2 of the fit.
		(*funcs)(x[i],afunc,ma);
		for (sum=0.0,j=1;j<=ma;j++) sum += a[j]*afunc[j];
		*chisq += SQR((y[i]-sum)/sig[i]);
	}     
	
	covsrt(covar,ma,ia,mfit); // Sort covariance matrix to true order of fitting coecients. 
	free_vector(afunc,1,ma);
	free_matrix(beta,1,ma,1,1);
}  

/*
That last call to a function covsrt is only for the purpose of spreading the
covariances back into the full ma Å~ ma covariance matrix, in the proper rows and
columns and with zero variances and covariances set for variables which were
held frozen.   
*/

#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}  

/*
Expand in storage the covariance matrix covar, so as to take into account parameters that are
being held .xed. (For the latter, return zero covariances.)
*/

void Fit::covsrt(float **covar, int ma, int ia[], int mfit) { 
	
	int i,j,k;
	float swap;
	for (i=mfit+1;i<=ma;i++)
	for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
	k=mfit;
	for (j=ma;j>=1;j--) {
		if (ia[j]) {
			for (i=1;i<=ma;i++) SWAP(covar[i][k],covar[i][j])
			for (i=1;i<=ma;i++) SWAP(covar[k][i],covar[j][i])
			k--;
		}
	}
}