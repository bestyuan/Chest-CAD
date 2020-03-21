// This is an implementation of the Gaussian Maximum-Likelihood classifier.
// 
// It is based on the abstract Classifier class framework (Classifier.h).
// See Classifier.h for more details.
//
// crandall, 6/2003

#ifndef __GMLCLASSIFER_H_
#define __GMLCLASSIFIER_H_

#include "Classifier.h"
#include "DMatrix.h"

class GmlClassifier : public Classifier
{
    public:
          // constructor
        GmlClassifier();
        virtual ~GmlClassifier() {};

        virtual void loadFromFile(const char *filename);
        
          // "classification" methods
        virtual Label classify(const double *feature_vector);

          // these methods return an array of beliefs, specifying the probability
          // for each possible class.
          // In this classifier, there is no difference between
          // getBeliefs and getNativeBeliefs.
        virtual double *getBeliefs(const double *feature_vector);
        virtual double *getNativeBeliefs(const double *feature_vector);

          // "informational" methods
          // returns the dimensionality of the feature space
        virtual unsigned int getDimensionality(void);
          // returns the number of classes
        virtual unsigned int getClassCount(void);

          // GML-specific methods

          // These function classify using the "L" method (leave-one-out), which is fair way
          // of testing and training on the same set of data. When a given feature vector is
          // to be classified, the classifier parameters are perturbed such that
          // it will be as if the classifier had not seen this feature vector during training
          // (even though it did).
        Label classify_L_method(const double *feature_vector, unsigned int correct_class);
        double *getBeliefs_L_method(const double *feature_vector, unsigned int correct_class);

    protected:
        double compute_L_method_adjustment(int samples_in_class, double dist_sqr);

    protected:
        unsigned int feature_count;
        bool initialized;

          // mean and inverse covariance matrices, for each class
        DMatrix *means;
        DMatrix *inv_covars;

          // ln of the determinants for each class
        double *ln_det;

        double *class_beliefs;
        int *sample_count;
};

#endif
