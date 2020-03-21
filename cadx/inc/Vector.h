//
//  (c) Copyright Eastman Kodak Company 
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//
//

#ifndef VECTOR_H
#define VECTOR_H


    

template <class VType>
class Vector
{
private:  
    VType *Data; 
    VType *Offset;              
    VType *Marker;
    long  Min_x;
    long  Max_x;    
    long  Width;          
    long  Own; 
    long  Size;
    long  Bytes;       

public:
// Constructors.
    Vector()
    {
        Initialize();
    }

    Vector(long _Min_x, long _Max_x);

// copy constructor
    Vector(const Vector<VType>& V);

// Destructor.
    ~Vector()
    {
        Free();
    }

// Functions to get variables.
    VType* Get_Data()
    {
        return Data;
    }

    VType Get_Value()
    {
        return *Marker;
    }

    long Get_Width()
    {
        return Width;
    }

    long Get_Min_x()
    {
        return Min_x;
    }

    long Get_Max_x()
    {
        return Max_x;
    }

// Returns 1 if the Vector memory has been assigned or successfully allocated
    short Valid()
    {
        if (NULL == Data)
            return 0;
        else
            return 1;
    }

// Initialize all variables.
    void Initialize();

// Allocates memory image. Returns 0 if okay, -1 on allocation error
    short Allocate_Memory();

// Creates an object. Returns 0 if okay and -1 if not.
    short Create(long _Min_x, long _Max_x);

// Returns a copy of the vector.
    Vector<VType>& Clone();

// Overloaded = operator.
    Vector<VType>& operator=(const Vector<VType>& V);

// Allocates memory for an object and returns a pointer to it.
    static Vector* Set_Pointer(long _Min_x, long _Max_x);

// Frees allocated memory. Initializes back to initial values.
    void Free();

// Increments the Marker.
    void Next()
    {
        Marker++;
    }

// Resets the Marker.
    void Reset()
    {
        Marker = Data;
    }

// Used to get a pointer to an element.
    VType* Element(long x)
    {
        return Offset + x;
    }

    VType Get(long x)
    {
        return *(Offset + x);
    
    }
    VType& Val(long x)
    {
        return *(Offset + x);
    }

    VType& Idx(long x)
    {
        return Val(x);
    }

// Increment.
    void Increment(long x)
    {
        *(Offset + x) += 1;
    }

// Sets all elements of the matrix to a value.
    void Set(VType V);
    void Set0()
    {
        memset((void *)Data, 0, Bytes);
    }

// Returns mean value.
    double Mean();

// Copies an image I.
    void Copy(const Vector<VType>& V);

// Table of squares.
    void Make_Square_Table();

// Calculates the unweighted inner product of two vectors.
    double Inner_Product(Vector<VType>& V);

// Calculates the square of the distance between the two vectors.
    double Distance2(Vector<VType>& V);

// Returns TRUE if the two vectors are the same otherwise return FALSE.
    short SameAs(Vector<VType>& V);

// Smooths the vector using the kernal 1, 1, 1.
    double Smooth();

// Reads Vector from a file;
    short  Read_File(const char *FileName);
    short  Read_Binary_File(const char *FileName);

// Writes Vector to a file;
    short Write_File(const char *FileName);
    short Write_Binary_File(const char *FileName);

//   friend ostream& operator <<(ostream& Out, Vector<VType>& V);

//   friend istream& operator >>(istream& In, Vector<VType>& V);

};

template <class VType>
ostream& operator <<(ostream& Out, Vector<VType>& V);

template <class VType>
istream& operator >>(istream& In, Vector<VType>& V);


#if defined(__GNUC__) || defined(WIN32)
#include "Vector.impl"
#endif

#endif

