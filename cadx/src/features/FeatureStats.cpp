//
//  (c) Copyright Eastman Kodak Company 
//
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//



#include "FeatureStats.h"   
#include "Util.h"    
#include "Tokenizer.h" 


using namespace CADX_SEG;



FeatureStats::FeatureStats(char* plotFileName) {    
	
	initialize();  

	strcpy(negFileName, "./FeatureStatsNeg.dat");
	strcpy(posFileName, "./FeatureStatsPos.dat");
	
	negFile.open(negFileName, ios_base::out);
	posFile.open(posFileName, ios_base::out);
	
	plotFile.open(plotFileName);
	jmpFile.open("Q:/schildkr/tmp/jmp.txt");
}  

FeatureStats::~FeatureStats() {
	
	if(posMean != NULL) delete [] posMean;        
	if(negMean != NULL) delete [] negMean;    
	
	if(posSigma != NULL) delete [] posSigma;
	if(negSigma != NULL) delete [] negSigma;

	if(posSum != NULL) delete [] posSum;        
	if(negSum != NULL) delete [] negSum;  
	
	if(posSum2 != NULL) delete [] posSum2;
	if(negSum2 != NULL) delete [] negSum2;  
	
	if(posSum2 != NULL) delete [] posMin;
	if(negSum2 != NULL) delete [] negMin;     
	
	if(posSum2 != NULL) delete [] posMax;
	if(negSum2 != NULL) delete [] negMax;
		
	if(label != NULL) {
		for(long k = 0; k < nFeatures; k++) delete[] label[k];
		delete[] label;
	}
   
	if(posCov != NULL) {
		for(long k = 0; k < nFeatures; k++) delete[] posCov[k];
		delete[] posCov;
	}

	if(negCov != NULL) {
		for(long k = 0; k < nFeatures; k++) delete[] negCov[k];
		delete[] negCov;
	}
}
	
void FeatureStats::addRoi(Roi& roi, Hit &hit) {  
	
	if(label == NULL) {
		allocate(roi.getNFeatures());
		for(long k = 0; k < nFeatures; k++) strcpy(label[k], roi.getFeatureLabel(k));	

		jmpFile << "class, name, index";
		for(long i = 0; i < nFeatures; i++) jmpFile << ", " << roi.getFeatureLabel(i);
	}    

	long noduleIndex;
	roi.setGroundtruth(hit.getGroundtruth(roi.getAbsCandidateCol(), roi.getAbsCandidateRow(), noduleIndex));

	long gt = roi.getGroundtruth();

	if(gt == Util.GROUNDTRUTH_NEGATIVE) {

		nNegCandidates++;

		if(roi.getNBoundaryPoints() == 0) return;
			
		nNegVectors++;

		negFile << "\n" << setw(40) << roi.getName();
		plotFile << "\n" << setw(6) << 0 << setw(40) << roi.getName() << setw(7) << roi.getIndex();
		jmpFile << "\n0, " << roi.getName() << ", " << roi.getIndex();
			
		for(long k = 0; k < nFeatures; k++) {
				       
			double value = roi.getFeatureValue(k);
				
			negFile << setw(40) << roi.getFeatureLabel(k) << setw(15) << value << flush;
			plotFile << setw(40) << roi.getFeatureLabel(k) << setw(15) << value << flush;
			jmpFile << ", " << value << flush;

			negSum[k] += value;
			negSum2[k] += Util::square(value);

			if(value < negMin[k]) negMin[k] = value;
			if(value > negMax[k]) negMax[k] = value;
            
            	for(long j = 0; j < nFeatures; j++) {
               	negCov[k][j] += roi.getFeatureValue(k) * roi.getFeatureValue(j);
            	}
		}
	} 
	else if(gt == Util.GROUNDTRUTH_DEFINITE || gt == Util.GROUNDTRUTH_PROBABLE) {    
	
		nPosCandidates++;
		
		hit.setDetected(true, noduleIndex);
	
		if(roi.getNBoundaryPoints() == 0) return;
			
		nPosVectors++;
			
		posFile << "\n" << setw(40) << roi.getName();
		plotFile << "\n" << setw(6) << 1 << setw(40) << roi.getName() << setw(7) << roi.getIndex();
		jmpFile << "\n1, " << roi.getName() << ", " << roi.getIndex();
			
		roi.compareRegionToGroundtruth(hit);
			
		groundtruthUndergrow += roi.getGroundtruthUndergrow();
		groundtruthOvergrow += roi.getGroundtruthOvergrow();
		groundtruthFit += roi.getGroundtruthFit();
			
		for(long k = 0; k < nFeatures; k++) {
				
			double value = roi.getFeatureValue(k);
				
			posFile << setw(40) << roi.getFeatureLabel(k) << setw(15) << value << flush;
			plotFile << setw(40) << roi.getFeatureLabel(k) << setw(15) << value << flush;
			jmpFile << ", " << value << flush;
				
			posSum[k] += value;
			posSum2[k] += Util::square(value);
				
			if(value < posMin[k]) posMin[k] = value;
			if(value > posMax[k]) posMax[k] = value;
            
            	for(long j = 0; j < nFeatures; j++) {
              		posCov[k][j] += roi.getFeatureValue(k) * roi.getFeatureValue(j);
           	}
		}
	}
													
} 
                                     
void FeatureStats::allocate(long _nFeatures) {    
	
	nFeatures = _nFeatures;
	
	label = new char*[nFeatures];	    
	
	posMean = new double[nFeatures];
	negMean = new double[nFeatures]; 
	   
	posSigma = new double[nFeatures];
	negSigma = new double[nFeatures]; 
	
	posSum = new double[nFeatures];    
	negSum = new double[nFeatures];  
	
	posSum2 = new double[nFeatures];    
	negSum2 = new double[nFeatures];     
	
	posMin = new double[nFeatures];    
	negMin = new double[nFeatures];  
	
	posMax = new double[nFeatures];    
	negMax = new double[nFeatures];
	
	posCov = new double*[nFeatures];
	negCov = new double*[nFeatures];
			
	for(long k = 0; k < nFeatures; k++) {	
		label[k] = new char[256];
			
		posMean[k] = 0.0;
		negMean[k] = 0.0;     
		
		posSigma[k] = 0.0;
		negSigma[k] = 0.0; 
	
		posSum[k] = 0.0;    
		negSum[k] = 0.0;   
		
		posSum2[k] = 0.0;    
		negSum2[k] = 0.0; 	
		
		posMin[k] = DBL_MAX;    
		negMin[k] = DBL_MAX; 	
		
		posMax[k] = -DBL_MAX;
		negMax[k] = -DBL_MAX;
	}
	
	for(long i = 0; i < nFeatures; i++) {
		posCov[i] = new double[nFeatures];
      	negCov[i] = new double[nFeatures];
      	for(long j = 0; j < nFeatures; j++) {
	     	posCov[i][j] = 0.0;
	     	negCov[i][j] = 0.0;
      	}
	}
	
	groundtruthUndergrow = 0.0;	    
	groundtruthOvergrow = 0.0;	
	groundtruthFit = 0.0;				
}  

void FeatureStats::initialize() {
	nFeatures = 0;
	label = NULL;

	nPosVectors = 0;
	nNegVectors = 0;
	
	nPosCandidates = 0;
	nNegCandidates = 0;
	
	posMean = NULL;
	negMean = NULL;    
	posSigma = NULL;
	negSigma = NULL; 
	
	posSum = NULL;    
	negSum = NULL; 
	posSum2 = NULL;    
	negSum2 = NULL;  
	
	posCov = NULL;
	negCov = NULL;
}

double FeatureStats::getDiscriminant(long k) {
	
	double d;   
		
	d = Util::abs(posMean[k] - negMean[k]) / (0.5 * (posSigma[k] + negSigma[k]));
			
	return d;	
}

void FeatureStats::calculate() {

	for(long k = 0; k < nFeatures; k++) {
		negMean[k] = negSum[k] / (double)nNegVectors;
		negSigma[k] = sqrt(negSum2[k] / (double)nNegVectors - Util::square(negMean[k]));
		posMean[k] = posSum[k] / (double)nPosVectors;
		posSigma[k] = sqrt(posSum2[k] / (double)nPosVectors - Util::square(posMean[k]));	
	}	
	
	for(long i = 0; i < nFeatures; i++) {
		for(long j = 0; j < nFeatures; j++) {
        		negCov[i][j] = (negCov[i][j] / (double)nNegVectors - negMean[i] * negMean[j]) / (negSigma[i] * negSigma[j]);
         		posCov[i][j] = (posCov[i][j] / (double)nPosVectors - posMean[i] * posMean[j]) / (posSigma[i] * posSigma[j]);
		}
	}
	
	groundtruthUndergrow /= (double)nPosVectors;	    
	groundtruthOvergrow /= (double)nPosVectors;	
	groundtruthFit /= (double)nPosVectors;			
}    


void FeatureStats::histograms() {    
	
	long i;  
	char token[1024];
	
	plotFile << "\n\n\n***Histograms***" << endl;

	negFile.close();
	posFile.close();
		
	negFile.open(negFileName, ios_base::in);
	posFile.open(posFileName, ios_base::in);
	

	long nHistBins = 40;
	double* posHist = new double[nHistBins];   
	double* negHist = new double[nHistBins]; 
	
	

	
	for(long k = 0; k < nFeatures; k++) {	
		
		long nPos = 0, nNeg = 0;
		
		plotFile << "\n\n" << label[k];  
		
		double min = Util::min(posMin[k], negMin[k]);  
		double max = Util::max(posMax[k], negMax[k]);
		
		double delta =  (max - min) / (double)(nHistBins - 1);  

		
		for(i = 0; i < nHistBins; i++) {posHist[i] = 0; negHist[i] = 0;} 
		
		negFile.clear();    
		negFile.seekg(0, ios_base::beg);      
		
		Tokenizer tokenizer1(negFile);     
		
		while(!tokenizer1.EndOfFile()) {    

			strcpy(token, tokenizer1.NextToken());

			if(strcmp(token, label[k]) == 0) {  
							
				double value = atof(tokenizer1.NextToken()); 

				long index = (long)((value - min) / delta);

				if(index >= 0 && index < nHistBins) {
					negHist[(long)((value - min) / delta)] += 1.0;   
					nNeg++;
				}
			} 		
		}  
		
		posFile.clear();   
		posFile.seekg(0, ios_base::beg);  

		Tokenizer tokenizer2(posFile);     
		
		while(!tokenizer2.EndOfFile()) {    

			strcpy(token, tokenizer2.NextToken());   

			if(strcmp(token, label[k]) == 0) {  
							
				double value = atof(tokenizer2.NextToken()); 
			
				posHist[(long)((value - min) / delta)] += 1.0;
				nPos++;
			} 		
		}  
						

		for(i = 0; i < nHistBins; i++) {  
			
			double x = min + delta * (double)i;    
			
			plotFile << "\n" << setw(15) << x 
			 << setw(15) << posHist[i] / (double)nPos
			 << setw(15) << negHist[i] / (double)nNeg;
		}
	}		
		
	delete[] posHist;		
	delete[] negHist;			
}

void FeatureStats::write(ostream& s) {      
	
	calculate();   
	
	s << "\nFeatureStats:" << endl;
	
	s << "Number of candidates" << ", # Pos: " << nPosCandidates << ", # Neg: " << nNegCandidates << endl;

	s << "Number of candidates with region boundary" << ", # Pos: " << nPosVectors << ", # Neg: " << nNegVectors << endl;

	s << "\n\nStatistics for candidates with region boundary:" << endl;

	s << "\nGroundtruth overgrow: " << groundtruthOvergrow
	 << ", undergrow: " << groundtruthUndergrow
	 << ", fit: " << groundtruthFit << endl;
	
	for(long k = 0; k < nFeatures; k++) {
		s << "\n\n" << label[k]
		 << "\n  Pos class mean, sigma, min, max: " 
		 << setw(15) << posMean[k] << setw(15) << posSigma[k] 
		 << setw(15) << posMin[k] << setw(15) << posMax[k]
		 << "\n  Neg class mean, sigma, min, max: " 
		 << setw(15) << negMean[k] << setw(15) << negSigma[k]
		 << setw(15) << negMin[k] << setw(15) << negMax[k]
		 << "\n  Discriminant: " << setw(15) << getDiscriminant(k);			  	    
	}

	s << "\n\nCovariance Matrix" << endl;

	for(long i = 0; i < nFeatures; i++) {
      s << "\n\n" << label[i];
      for(long j = 0; j < nFeatures; j++) {
         s << "\n\t" << "^" << label[j] << "=" << posCov[i][j] << "(pos)" << " " << negCov[i][j] << "(neg)   ";
	}}
	
	s << endl;
}

ostream& operator << (ostream& s, FeatureStats& stats) {      
	
	stats.write(s);
					
	return s;	
}
