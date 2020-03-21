// Classifier class implementation
//
// These are mostly barebones implementations of methods that simply throw
// a cerrNotImplemented exception. These methods are really ever
// meant to be called; they are only called when someone calls a method that
// the derived class didn't implement. The exception allows for debugging.
//
// "Useful" default implementations of test() and getClassCount() are 
// provided, because these methods are classifier-invariant.
// Actually, the two classify() methods could probably be generalized enough
// to be implemented in Classifier also.

#include "Classifier.h"

/*
 * test() : test a classifier
 *
 * arguments:
 *  - feature_set : an array of feature vectors.
 *  - correct_labels : an array of labels, each one corresponding to a
 *                     vector in feature_set.
 *  - vector_count : the number of feature vectors in feature_set.
 *
 * returns:
 * A confusion matrix of size class_count x class_count. Correct labels
 * correspond to rows and classified labels correspond to columns.
 * 
 * throws:
 *  - cerrNotReady : the network has not been initialized (e.g. by calling
 *                   load_net).
 *
 * caveats:
 * Note that here, Label is assumed to be an integer type, and the class
 * labels are assumed to range from [0,class_count). This is not assumed
 * nor enforced elsewhere.
 */
double **Classifier::test(const double **feature_set, 
                          const Label *correct_labels, 
                          unsigned int vector_count)
{
    if(!initialized)
        throw ClassifierError(ClassifierErrorType::cerrNotReady);

    double **confusion_matrix=new double *[class_count];

    unsigned int i;
    for(i=0; i<class_count; i++)
        confusion_matrix[i]=new double[class_count];

    for(i=0; i<vector_count; i++) {
        Label result=classify(feature_set[i]);

        confusion_matrix[correct_labels[i]][result]++;
    }

    return(confusion_matrix);
}

/*
 * getClassCount() : return the total number of possible classes
 *
 * returns:
 * The number of possible unique class labels that the classifier can return.
 * 
 * throws:
 *  - cerrNotReady : the network has not been initialized (e.g. by calling
 *                   load_net).
 */
unsigned int Classifier::getClassCount(void)
{
    if(!initialized)
        throw ClassifierError(ClassifierErrorType::cerrNotReady);

    return class_count;
}


// rest are dummy implementations

void Classifier::loadFromFile(const char *filename) 
{
    throw ClassifierError(ClassifierErrorType::cerrNotImplemented, "Classifier::loadFromFile()");
}

void Classifier::train(const double **feature_set, 
                       const Label *correct_labels, 
                       unsigned int vector_count, 
                       unsigned int feature_count) 
{
    throw ClassifierError(ClassifierErrorType::cerrNotImplemented, "Classifier::train()");
}

Label Classifier::classify(const double *feature_vector) 
{
    throw ClassifierError(ClassifierErrorType::cerrNotImplemented, "Classifier::classify()");
}

Label Classifier::classify(const double *feature_vector, 
                           const double *thresholds) 
{
    throw ClassifierError(ClassifierErrorType::cerrNotImplemented, "Classifier::classify()");;
}

double *Classifier::getBeliefs(const double *feature_vector)
{
    throw ClassifierError(ClassifierErrorType::cerrNotImplemented, "Classifier::getBeliefs()");
}

double *Classifier::getNativeBeliefs(const double *feature_vector)
{
    throw ClassifierError(ClassifierErrorType::cerrNotImplemented, "Classifier::getNativeBeliefs()");
}

unsigned int Classifier::getDimensionality(void)
{
    throw ClassifierError(ClassifierErrorType::cerrNotImplemented, "Classifier::getDimensionality()");
}

void Classifier::writeToFile(const char *filename)
{
    throw ClassifierError(ClassifierErrorType::cerrNotImplemented, "Classifier::writeToFile()");
}

Classifier::Classifier()
{
    initialized=false;
}

/*
 * Classifier exceptions
 */

const ClassifierErrorType ClassifierErrorType::cerrCannotOpenFile("Classifier cannot open file");
const ClassifierErrorType ClassifierErrorType::cerrBadFileFormat("Classifier file has bad or unrecognized format");
const ClassifierErrorType ClassifierErrorType::cerrUnsupportedFileVersion("Unsupported classifier file format version");
const ClassifierErrorType ClassifierErrorType::cerrUnsupportedFeature("Classifier file requests an unsupported feature");
const ClassifierErrorType ClassifierErrorType::cerrNotImplemented("Abstract method not implemented by this classifier");
const ClassifierErrorType ClassifierErrorType::cerrBadArgument("Bad argument passed to classifier");
const ClassifierErrorType ClassifierErrorType::cerrNotReady("Classifier is not ready (has it been initialized?)");
const ClassifierErrorType ClassifierErrorType::cerrWrongClassifier("Classifier file is for a different kind of classifier");

std::ostream &operator<<(std::ostream &os, const ClassifierError &ce)
{
    os << ce.error_type.error_msg << std::endl;
    os << "Exception details: " << ce.user_text << std::endl;

    return os;
}






