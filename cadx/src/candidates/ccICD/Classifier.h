/*
 * Classifier.h : Definition of abstract Classifier class
 *
 * This is an attempt at an abstract Classifier class. 
 * It is based on Bob Senn's Classifier java interface.
 *
 * Issues:
 * - API should be independent of the specific classifier actually being used.
 *
 * - But we also want to allow access to classifier-specific functionality.
 *
 * - Different types of classifiers require different parameters, output
 *   different confidence metrics (probabilities, distances), etc.
 *
 * - "Semantic gap" between output of a classifier and class label. e.g. 
 *   a neural network output might represent a distance, a probability, 
 *   a class label, etc. depending on how it was trained.
 *
 * - Of what type should class labels be? (int, string, templates...?)
 *   For right now they are ints, but can be changed easily.
 *
 *
 * Philosophies behind this class:
 * 
 * - The Classifier class itself just defines a common API interface.
 *   Derived classes must provide the implementations.
 *
 * - There is a set of "lowest common denominator" methods that any derived
 *   classifier should implement. 
 *
 * - This means a classifier may not be given all the information it needs
 *   to do something. The classifier should then use some defaults in order
 *   to accomplish the task anyway.
 *
 * - However, derived classes may include additional, classifier-specific
 *   methods.
 * 
 * - All classifiers must produce a measure of belief in the range [0, 1.0] 
 *   for each possible class. [But applications may also access the
 *   "native" belief values.]
 *
 * - A classifier object is either completely initialized or not initialized
 *   at all.
 *
 * - A classifier may be initialized either by loading from a file or by
 *   training.
 *
 * - The Classifier class is general-purpose; it knows nothing about images. 
 *   We could create another class (e.g. PixelwiseClassifier) as a convenience.
 * 
 * - Error conditions cause exceptions, a la IEM.
 * 
 * - A derived class may choose not to implement all of the "lowest common
 *   denominator" methods: e.g. a class may not support training. Calls
 *   to unimplemented methods cause exceptions.
 *
 * - Obviously, details of derived class implementation are of no concern to 
 *   the abstract Classifier class. However, in general the derived class will
 *   probably not be a classifier itself, but a thin software layer that maps 
 *   between the Classifier class API and the classifier. It is this thin layer
 *   that closes the "semantic gap" mentioned above. Different derived classes
 *   can use  the same type of classifier but interpret the output in 
 *   different ways. 
 *
 *   For example, the meaning of the output of a neural network depends on how
 *   it was trained. NNet2Classifier is a derived class of Classifier
 *   that works with 2-class problems, assumes the neural network has exactly
 *   one output, and assumes that the output is a probability between 0 and 1.
 *   A different mapping mechanism would be required for N-class problems that
 *   have N network outputs. This derived class (e.g. NNetNClassifier) could
 *   still use the same neural network implementation, but would implement a 
 *   slightly different mapping between the network and the Classifier API:
 *
 *
 *        +---------------+                +---------------+
 *        | Classifier    |                | Classifier    |
 *        +---------------+                +---------------+
 *                |                                |
 *                v                                v
 *        +---------------+                +---------------+
 *        |NNet2Classifier|                |NNetNClassifier|
 *        +---------------+                +---------------+
 *                |                                |
 *                v                                v
 *        +---------------+                +---------------+
 *        |  ffneuralnet  |                |  ffneuralnet  |
 *        +---------------+                +---------------+
 *
 *
 *   Similarly if a network were trained on distances instead of probabilities,
 *   different mapping code would be required.
 */

#ifndef _CLASSIFIER_H_
#define _CLASSIFIER_H_

#include <string.h>
#include <iostream>
#include <string>

// Possible exceptions thrown by the classifier.

// All classifier errors should be derived from this.
class ClassifierErrorType {
    public:
          // constructor to make strings into errors
        ClassifierErrorType(const char *s) 
        {
            error_msg = new char[strlen(s)+1];
            strcpy(error_msg, s);
        }

        ~ClassifierErrorType()
        {
            if(error_msg)
                delete[] error_msg;
        }


          // File-related exceptions

          // Can't open a file
        static const ClassifierErrorType cerrCannotOpenFile;

          // Format of classifier file is bad
        static const ClassifierErrorType cerrBadFileFormat;

          // File format version of specified classifier file is not supported
        static const ClassifierErrorType cerrUnsupportedFileVersion;

          // A feature in the specified classifier file is not supported
        static const ClassifierErrorType cerrUnsupportedFeature;

          // A requested abstract classifier method is not supported by this 
          // classifier
        static const ClassifierErrorType cerrNotImplemented;

          // A bad argument was passed to a method (e.g. probability <0 or 
          // >1.0).
        static const ClassifierErrorType cerrBadArgument;

          // The classifier is not ready to classify (i.e. it hasn't been 
          // initialized).
        static const ClassifierErrorType cerrNotReady;

          // A classifier file is not compatible with this classifier object.
        static const ClassifierErrorType cerrWrongClassifier;

        char *error_msg;

};

class ClassifierError
{
    public:
        ClassifierError(const ClassifierErrorType &_et, 
                        const char *_user_text=0) : 
                error_type(_et)
        {
            if(_user_text) 
            {
                user_text = _user_text;
            }
        }

        const ClassifierErrorType &error_type;
        std::string user_text;
};

std::ostream &operator<<(std::ostream &os, const ClassifierError &ce);


// Type of class labels
// For right now, this is an unsigned int.
// What should it really be?
// e.g. a string would be more flexible and more semantically
//      meaningful, but less efficient. 
// An attractive idea would be to make Classifier a template, in order to 
//  support arbitrary Label types. This would require extra code (e.g. for 
//  checking that class labels in a file are compatible with type of 
//  Classifier class) and
//  might actually diminish portability and reusability of classifiers (e.g.
//  Classifier<int>'s would not be compatible with Classifier<String>'s.), so
//  is it worth it?
typedef unsigned int Label;


// Classifier class definition
//
// Barebones implementations of these methods are in Classifier.cc. In most
// cases these methods just throw a CerrNotImplemented exception. Such methods
// should be overloaded in the derived classes. 
// "Useful" default implementations of test() and getClassCount() are 
// provided by Classifier.cc, because these methods are classifier-invariant.
// Actually, the two classify() methods could probably be generalized enough
// to be implemented in Classifier also.

class Classifier {
    public:
          // constructor
        Classifier();
        virtual ~Classifier() {};

          // "creation" methods
          // a call to one of these is required before any other method is
          // called.
          // These could be constructors instead.
        virtual void loadFromFile(const char *filename);
        virtual void train(const double **feature_set, 
                           const Label *correct_labels, 
                           unsigned int vector_count, 
                           unsigned int feature_count);

        
          // "classification" methods
          // given a feature vector, these methods perform a "hard"
          //  classification
          // and return the class label
          // the second overloaded version allows specifying a threshold for 
          // each class, between 0 and 1
        virtual Label classify(const double *feature_vector);
        virtual Label classify(const double *feature_vector, 
                               const double *thresholds);

          // these methods return an array of beliefs, specifying the belief 
          // for each possible class.
          // getBeliefs() returns the beliefs as normalized "probabilities" 
          // between 0 and 1. The number of elements is the same as the number
          // of classes.
          // getNativeBeliefs returns the raw confidence value computed by the 
          // underlying classifier. The number of elements depends on the 
          // specific classifier.
        virtual double *getBeliefs(const double *feature_vector);
        virtual double *getNativeBeliefs(const double *feature_vector);


          // "informational" methods
          // returns the dimensionality of the feature space
        virtual unsigned int getDimensionality(void);
          // returns the number of classes
        virtual unsigned int getClassCount(void);

          // writes out the classifier to a file, suitable for loading via 
          // loadFromFile()
        virtual void writeToFile(const char *filename);

          // Tests the classifier, given some feature vectors and the  
          // corresponding correct labels. Returns a confusion matrix 
          // (of size getClassCount() x getClassCount()).
        double **test(const double **feature_set, 
                      const Label *correct_labels, 
                      unsigned int vector_count);


    protected:
        unsigned int class_count;
        bool initialized;
};

#endif


