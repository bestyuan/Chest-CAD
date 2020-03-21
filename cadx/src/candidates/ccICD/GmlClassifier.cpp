#include "GmlClassifier.h"
#include <fstream>
#include <math.h>

#ifdef WIN32
// This hideously ugly hack is required because Microsoft Visual C++ 6.0
// does not properly implement the ANSI C++ standard regarding variables
// declared within 'for' statements.
#define for if(0) ; else for
#endif

// This is an implementation of the Gaussian Maximum-Likelihood classifier.
// 
// It is based on the abstract Classifier class framework (Classifier.h).
// See Classifier.h for more details.
//
// crandall, 6/2003
//

using namespace std;

GmlClassifier::GmlClassifier()
{
      // initialize everything to 0
    means = 0;
    inv_covars = 0;
    class_beliefs = 0;
    ln_det = 0;
    sample_count = 0;
}

// load GML parameters from a file
//
void GmlClassifier::loadFromFile(const char *filename)
{
    fstream fs;

    fs.open(filename, ios::in);
    if(!fs.good())
        throw string("Can't open GML parameter file.");
    class_count = feature_count = 0;
    int cur_class = 0;

    while(!fs.eof())
    {
        string str;
        fs >> str;
        
        if(str == "class_count:")
        {
            fs >> class_count;

            means = new DMatrix[class_count];
            inv_covars = new DMatrix[class_count];
            class_beliefs = new double[class_count];
            ln_det = new double[class_count];
            sample_count = new int[class_count];
        }
        else if(str == "feature_count:")
        {
            fs >> feature_count;

        }
        else if(str == "begin_class")
        {
            fs >> cur_class;

            means[cur_class] = DMatrix(1, feature_count);
            
            inv_covars[cur_class] = DMatrix(feature_count, feature_count);
            
        }
        else if(str == "means:")
        {
            fs >> means[cur_class];
        }
        else if(str == "inv_covars:")
        {
            fs >> inv_covars[cur_class];
        }
        else if(str == "ln_det:")
            fs >> ln_det[cur_class];
        else if(str == "sample_count:")
            fs >> sample_count[cur_class];
    }

    fs.close();

    initialized = true;
}

unsigned int GmlClassifier::getDimensionality()
{
    if(!initialized)
        throw ClassifierError(ClassifierErrorType::cerrNotReady);

    return feature_count;
}

unsigned int GmlClassifier::getClassCount()
{
    if(!initialized)
        throw ClassifierError(ClassifierErrorType::cerrNotReady);

    return class_count;
}

// perform a hard classification of feature_vector
//
// returns the Label associated with the resultant class. Labels for
// each class are defined in the .gml file that was loaded.
//
Label GmlClassifier::classify(const double *feature_vector)
{
    if(!initialized)
        throw ClassifierError(ClassifierErrorType::cerrNotReady);

    double beliefs_sum = 0;
      // standard GML classification procedure
    for(unsigned int i=0; i<class_count; i++)
    {
        DMatrix data_norm = (DMatrix(1, feature_count, feature_vector) - means[i]);

        double dMahalSqr =  data_norm * inv_covars[i] * data_norm.transpose();

        double result = 0.5 * dMahalSqr + 0.5 * ln_det[i];

        class_beliefs[i] = exp(-result);

        beliefs_sum += class_beliefs[i];
    }

      // normalize beliefs so that they sum to 1.0 across all classes
      // then figure out which class has highest belief
    int max_class = 0;
    double max_belief = -1;
    for(unsigned int i=0; i<class_count; i++)
    {
        class_beliefs[i] /= beliefs_sum;
        if(class_beliefs[i] > max_belief)
        {
            max_class = i;
            max_belief = class_beliefs[i];
        }
    }
    
    return(max_class);
}

// run feature_vector through classifier, and return a posteriori 
// probabilities for each class
//
double *GmlClassifier::getBeliefs(const double *feature_vector)
{
    classify(feature_vector);

    return class_beliefs;
}        

// this function is equivalent to getBeliefs for the GML classifier, because
// the logical output of a GML classifier is a probability. 
//
double *GmlClassifier::getNativeBeliefs(const double *feature_vector)
{
    return getBeliefs(feature_vector);
}

// These functions classify using the "L" method (leave-one-out), which is fair way
// of testing and training on the same set of data. When a given feature vector is
// to be classified, the classifier parameters are perturbed such that
// it will be as if the classifier had not seen this feature vector during training
// (even though it did).
//
double GmlClassifier::compute_L_method_adjustment(int samples_in_class, double dist_sqr)
{
      // This code stolen from J. Revelli's GmlClassifier matlab classes.
      // (looAdjustment.m)

    double a=(samples_in_class*samples_in_class-3.0*samples_in_class+1.0)*dist_sqr / (samples_in_class-1.0);
    double b=samples_in_class*dist_sqr*dist_sqr;
    double c=(samples_in_class-1.0)*(samples_in_class-1.0)-samples_in_class*dist_sqr;
    double d=0.5*feature_count*log(1.0+1.0/(samples_in_class-2.0));
    double e=0.5*log(1.0-samples_in_class*dist_sqr/((samples_in_class-1.0)*(samples_in_class-1.0)));
    double g=0.5*(a+b)/c + d + e;

    return g;
}

Label GmlClassifier::classify_L_method(const double *feature_vector, unsigned int correct_class)
{
    if(!initialized)
        throw ClassifierError(ClassifierErrorType::cerrNotReady);

    double beliefs_sum = 0;
      // normal GML computation
    for(unsigned int i=0; i<class_count; i++)
    {
        DMatrix data_norm = (DMatrix(1, feature_count, feature_vector) - means[i]);

        double dMahalSqr =  data_norm * inv_covars[i] * data_norm.transpose();
        double result = 0.5 * dMahalSqr + 0.5 * ln_det[i];

          // if this is the correct class, add in the L-method adjustment
        if(correct_class == i)
            result += compute_L_method_adjustment(sample_count[i], dMahalSqr);

        class_beliefs[i] = exp(-result);

        beliefs_sum += class_beliefs[i];
    }

      // normalize beliefs so that they sum to 1.0 across all classes
      // then figure out which class has highest belief
    int max_class = 0;
    double max_belief = -1;
    for(unsigned int i=0; i<class_count; i++)
    {
        class_beliefs[i] /= beliefs_sum;
        if(class_beliefs[i] > max_belief)
        {
            max_class = i;
            max_belief = class_beliefs[i];
        }
    }
    
    return(max_class);
}


double *GmlClassifier::getBeliefs_L_method(const double *feature_vector, unsigned int correct_class)
{
    classify_L_method(feature_vector, correct_class);

    return class_beliefs;
}
