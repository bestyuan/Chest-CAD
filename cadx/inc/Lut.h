//
//  (c) Copyright Eastman Kodak Company 
//  $Date: 2002/06/11 21:09:15 $
//  343 State Street
//  Rochester, NY 14650
//  All rights reserved
//
//  RCS: $Id: Lut.h,v 1.1.1.1 2002/06/11 21:09:15 L615076 Exp $
//

#ifndef LUT_H
#define LUT_H


template<class EType> class JEntry; 



template <class LType>
class Lut
{

	protected:

    LType *Data;          // Points the position of Lut in memory.
    LType **BandPtr;      // Array of pointers to Lut bands in memory.
    long  Min_i;
    long  Max_i;
    long  NoLevels;         
    long  NoBands;
    long  Bytes;  
    bool  Own; 

	public:
	// Constructors.
    Lut() {
        Initialize();
    }

    Lut(long _Min_i, long _Max_i, long _NoBands);

    Lut(const Lut<LType>& lut);

// Initializes parameters.
    void Initialize();

// Allocates memory for Lut.
// Sets ErrorCode to JUTIL_OK if successful and to some other
// error code if not.
    void Allocate_Memory();

// Returns class members.
    long Get_NoLevels() const {
        return NoLevels;
    }

    long Get_NoBands() const {
        return NoBands;
    }

    long Get_MinIndex() const {
        return Min_i;
    }

    long Get_MaxIndex() const {
        return Max_i;
    }

  
// Destructor.
    ~Lut() {
        Free();
    }

// Free memory.
    void Free() {
        if (NULL != Data && Own) {
            delete[] Data;
            Data = NULL;
        }

        if (NULL != BandPtr) {
            delete[] BandPtr;
            BandPtr = NULL;
        }

        Initialize();
    }

// Allocates memory for an object and returns a pointer to it.
    static Lut* Set_Pointer(long _Min_i, long _Max_i, long _NoBands);

// Allocates memory for an existing Lut.
    void Create(long _Min_i, long _Max_i, long _NoBands);

	// Copies a Lut.
	void Copy(const Lut<LType>& lut) {

        if (NULL == Data) {
            return;
        }
        memcpy((void *)Data, (void *)lut.Data, Bytes);
    }

	// Sets first Lut equal to second.
	Lut<LType>& operator=(const Lut<LType>& lut);

	// Sets all elements of the mLut to a zero.
    void Set0() {
        if (NULL != Data) {
            memset((void *)Data, 0, Bytes);
        }
    }

	// Reads Lut from file.
	short Read_File(const char *const FileName);

	// Writes Lut to file
	short Write_File(const char *const FileName);

	// Writes Lut to screen.
	void Dump();

	// Used to assign or retrieve value.
	LType* Entry(long k, long b) {
        return BandPtr[b] + k;
    }

	LType Get(long k, long b) {
		if (k < Min_i || k > Max_i || b < 1 || b > NoBands) return 0;
		else return *Entry(k, b);
    }

    // Const version. Can retrieve a reference to value in a 'const' LUT
    // This reference can't be assigned to (can't be an l-value)
    const LType& Val(long k, long b) const {
        return *(BandPtr[b] + k);
    }

    // Normal version. Can retrieve a reference to value in a 'non const' LUT
    // This reference CAN be assigned to (CAN be an l-value)
    LType& Val(long k, long b) {
        return *(BandPtr[b] + k);
    }

// Returns 0 if the index is out of range.
//
// LType& Lut<LType>::Safe_Val(long k, long b)
// KRD changed to return LType, not LType &
//     returning LType is was returning a value on the call stack,
//     a bad thing as the reference will refer to 'nothing' as soon as
//     the call returns.
//
// Also, moved implementation to this header, from Lut.cpp; to get the
//     function inlined.
//
    LType Safe_Val(long k, long b) {
       if (k < Min_i)
           return 0;
       else if (k > Max_i)
           return 0;
       else
           return Val(k, b);  // Val() returns a reference.
                              // Will convert to object via copy construct.
    }

    const LType Safe_Val(long k, long b) const {
       if (k < Min_i)
           return 0;
       else if (k > Max_i)
           return 0;
       else
           return Val(k, b);  // Val() returns a reference.
                              // Will convert to object via copy construct.
    }

	// Returns interpolated value.
    LType Interpolate(double v, long b);

	// Multiplies all entries of Lut by C.
    void Scale(double C, long b);

	// Returns maximum Lut entry.
    LType Max(long b); 

	// Combines two Luts.  
    static Lut<LType>& Combine(Lut<LType>& lut1, Lut<LType>& lut2, long b);

	// Sorts entrys in decreasing values of band Key.
	void Sort(long Key);
    void Sort_Ascending(long Key, long Min, long Max);
    void Sort_Descending(long Key, long Min, long Max);

	// Adds a new entry into an ascending order sorted LUT.
	void Merge_Ascending(long Key, long Min, long Max, JEntry<LType>& newEntry);  
	
	// Adds a new entry into a descending order sorted LUT.
	void mergeDescending(long Key, long Min, long Max, JEntry<LType>& newEntry);

	// Moves a block of NoEntries enties.
    void Move_Block(long From, long To, long NoEntries);

	// Switches two entries.
    void Switch(long j, long k);

	// Replaces entry j with entry k.
    void Replace(long j, long k);

	// Invert the index with respect to band Key.
    void Invert(long Key);

	// Makes a Lut with a trapazoidal shape.
    void Trapazoid(long *Node, LType Max, long b);

	// Makes a LUT for applying a gamma to an image;
	void Gamma(double g, double MaxValue, long b);
};


template <class EType>
class JEntry 
{
	protected:
    EType *Data; 
    EType *Offset;                 
    long  NoBands;          
    long  Bytes;       

    public:
	// Constructors.
    JEntry() {
        Initialize();
    }

	JEntry(long _NoBands);

	// Destructor.
    ~JEntry() {

       if (NULL != Data) {
           delete[] Data;
           Data = NULL;
       }
    }

    void Initialize();

    const EType &Val(long b) const {
        return *(Offset + b);
    }

    EType& Val(long b) {
       return *(Offset + b);
    }

	void Allocate_Memory();
};

#if defined(__GNUC__) || defined(WIN32)
#include "Lut.impl"
#endif

#endif

