
#ifndef MATRIXMULTIPLY_H
#define MATRIXMULTIPLY_H

IemMatrix matrixMultiply(const IemMatrix &lhMat, const IemMatrix &rhMat);
int matrixMultiply(const IemMatrix &lhMat,const IemMatrix &rhMat, IemMatrix &preAllocatedOutput);

#endif
