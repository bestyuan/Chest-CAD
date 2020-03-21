//
//  Module: NLMRA.h
//
//    Date: 06/26/2002
//
//  Author: Jim Sehnert, Ph.D., Eastman Kodak HI Research Lab
//
// Purpose: Define various multi-resolution analysis classes
//

#ifndef __NLMRA_H_INCL__
#define __NLMRA_H_INCL__

#ifndef __NUMLIB_H_INCL__
#error "You must include NumLib.h before including NLInterpolate.h"
#endif

//Forward declarations
class CMRANode;
 
  class CPyramid;

  class CBiorthogonalWavelet;
    class CBiorthogonalWaveletChild;
      class CBiorthogonalWaveletChildLL;
      class CBiorthogonalWaveletChildLH;
      class CBiorthogonalWaveletChildHL;
      class CBiorthogonalWaveletChildHH;

  class CDyadicWavelet;
    class CDyadicWaveletChild;
  

class CMRAManager;

///////////////////////////////////////////////////////////////////////////////
//
// C L A S S  CMRANode:
//
// Base multiresolution analysis/synthesis node class. When decomposed (anaysis)
// forms a tree structure from finest resolution to coarsest resolution.
//
class CMRANode {

friend class CMRAManager;
friend class CPyramid;
friend class CDyadicPyramid;
friend class CBiorthogonalWavelet;
friend class CBiorthogonalWaveletChild;
friend class CDyadicWavelet;
friend class CDyadicWaveletChild;

protected://Variables
  CMRANode* m_pParent;      //Next (finer) level up in the MRA
  C2DArr<float>* m_pData;   //MRA data at this level
  CMRANode* m_pChild;       //Next (coarser) level down in the MRA
  int m_iFilterTaps;        //Enumerator for filtering coefficients
  
protected://Functions/Methods
  virtual bool InputRootData( const C2DArr<unsigned short>& input);
  virtual bool InputRootData( const C2DArr<unsigned short>& input, const float* pfLut);
  virtual bool InputRootData( const C2DArr<float>& input);
  
  virtual void Free();
  bool AllocData( int iWidth, int iHeight);

  virtual bool DoAnalysis()=0;  //Actual analysis implementation
  virtual bool DoSynthesis()=0; //Actual synthesis implementation

  virtual bool DoNoiseSuppression() { return true;}
  virtual bool DoEnhancement() { return true;}

  //Constructor protected, since all public interface must go through manager
  CMRANode( CMRANode* pParent, bool bAllocData=true);

public://Functions/Methods
  CMRANode();
  virtual ~CMRANode() {Free();}

  virtual bool Copy( CMRANode*) {return false;}

  bool IsValid() const {return 0!=m_pData;}

  bool Analyze();    //Decompose to next level 
  bool Synthesize(); //Synthesize to this level

  CMRANode* Child() {return m_pChild;}
  CMRANode* Parent() {return m_pParent;}

  C2DArr<float>* Data() { _ASSERT(this); return m_pData;}
  int Level() const;
  int FilterTaps();
  int Width() const  {_ASSERT(this); return (0!=m_pData) ? m_pData->Width():0;}
  int Height() const {_ASSERT(this); return (0!=m_pData) ? m_pData->Height():0;}

  virtual int ChildWidth( int ii=0) const  {return (Width()+1)>>1;}
  virtual int ChildHeight( int ii=0) const {return (Height()+1)>>1;}
};//class CMRANode

///////////////////////////////////////////////////////////////////////////////
//
// C L A S S  CMRAManager:
//
// Class for performing various multi-resolution decompositions. Decomposition
// method driven by the type of class assigned to the root node m_pRoot.
//
class CMRAManager {

protected://Variables
  CMRANode *m_pRoot;      //Root (base level) of the MRA

protected://Methods
  void Free();

public: //Functions/Methods
  CMRAManager()  {m_pRoot=0;}
  ~CMRAManager() {Free();}

  template <class T> bool 
  Initialize( CMRANode* pRoot, const C2DArr<T>& input, int iFilterTaps=0)
  {
    Free();
    m_pRoot = pRoot;
    m_pRoot->m_iFilterTaps = iFilterTaps;
    return m_pRoot->InputRootData( input);
  }//bool CMRAManager::Initialize( CMRANode*, const C2DArr<T>&, int)

  template <class T> bool 
  Initialize( CMRANode* pRoot, const C2DArr<T>& input, const float* pfLut, int iFilterTaps=0)
  {
    Free();
    m_pRoot = pRoot;
    m_pRoot->m_iFilterTaps = iFilterTaps;
    return m_pRoot->InputRootData( input, pfLut);
  }//bool CMRAManager::Initialize( CMRANode*, const C2DArr<T>&, float*, int)

  bool Analyze( int iLevels);
  bool Synthesize();          //Synthesize back up to root level

  CMRANode* Root() {return m_pRoot;}

  CMRANode* SetMRA( CMRANode* pRoot) 
    {CMRANode* pRet=m_pRoot; m_pRoot=pRoot; return pRet;}
};//class CMRAManager

///////////////////////////////////////////////////////////////////////////////
//
// S P E C I F I C    M R A    C L A S S E S
//

///////////////////////////////////////////////////////////////////////////////
//
// L A P L A C I A N    P Y R A M I D
//
// NOTE: The analysis/synthesis have not been optimized, but are still fairly
//       efficient.
// 
class CPyramid : public CMRANode {

protected: //Functions/Methods
  virtual bool AllocChild();
  bool CreateChild( C2DArr<float>* pBuffer);
  void UpsampleChildCols( C2DArr<float>* pBuffer, float *pfBuf1, float* pfBuf2);

  virtual bool DoAnalysis();
  virtual bool DoSynthesis();

  //Constructor for non-root level nodes
  CPyramid( CPyramid* pParent) : CMRANode( pParent, true) {;}

public:
  CPyramid( const CPyramid&); //Copy constructor
  CPyramid() : CMRANode() {;}

  virtual bool Copy( CMRANode*);
};//class CPyramid



//Basic operations for performing the downsampling and upsample operations for
//the Laplacian pyramid analysis/synthesis
void PyramidDownsample(float* pfDst, float* pfSrc, int iSrcLength, int iTaps);
void PyramidUpsample  (float* pfDst, float* pfSrc, int iDstLength, int iTaps);

///////////////////////////////////////////////////////////////////////////////
// Biorthogonal wavelet subband type enumerator
typedef enum {
  BWAV_SUB_LL = 0,
  BWAV_SUB_LH = 1,
  BWAV_SUB_HL = 2,
  BWAV_SUB_HH = 3,
  BWavSubbandEOL = 4
}BWavSubband;

///////////////////////////////////////////////////////////////////////////////
//
// B I O R T H O G O N A L    W A V E L E T
//
class CBiorthogonalWavelet : public CMRANode {

protected: //Variables
  BWavSubband m_enSubband;            //Which subband are we?
  CBiorthogonalWavelet* m_apChildren[4]; //Array of children

protected: //Functions/Methods
  virtual void Free();
  void FreeChildren();
  bool CreateChildren();

  CBiorthogonalWavelet( CBiorthogonalWavelet* pParent, BWavSubband enSB);
  
  virtual bool DoAnalysis();
  virtual bool DoSynthesis();

public: //Functions/Methods
  CBiorthogonalWavelet();
  virtual ~CBiorthogonalWavelet() {Free();}

  BWavSubband GetSubbandType() const {return m_enSubband;}
  CBiorthogonalWavelet* Child( BWavSubband enSB) {return m_apChildren[(int)enSB];}

  virtual int ChildWidth( int ii=0) const  
    {
      if( ii==(int)BWAV_SUB_LL || ii==(int)BWAV_SUB_HL )
        return (Width()+1)>>1;
      else
        return Width()>>1;
    }
  virtual int ChildHeight( int ii=0) const
    {
      if( ii==(int)BWAV_SUB_LL || ii==(int)BWAV_SUB_LH )
        return (Height()+1)>>1;
      else
        return Height()>>1;
    }
};

//Child classes for analysis
class CBiorthogonalWaveletChild : public CBiorthogonalWavelet {

  friend class CBiorthogonalWavelet;

protected:
  float **m_ppfRows;
  virtual void Free();
  virtual void SetRowPtrs();

  CBiorthogonalWaveletChild( CBiorthogonalWavelet* pParent, BWavSubband enSB);

public:
  virtual ~CBiorthogonalWaveletChild() {Free();}
};//class CBiorthogonalWaveletChild

//Basic operations for performing the filter/downsample and upsample/filter operations 
//for the biorthogonal wavelet analysis/synthesis
void FwdBiorthogWavTransform(float* pfLo, float* pfHi, float* pfSrc, int iSrcLen, int iTaps);
void InvBiorthogWavTransform(float* pfDst, float* pfLo, float* pfHi, int iDstLen, int iTaps);

///////////////////////////////////////////////////////////////////////////////
// Dyadic wavelet subband type enumerator
typedef enum {
  DWAV_SUB_L1 = 0,
  DWAV_SUB_L2 = 1,
  DWAV_SUB_L3 = 2,
  DWAV_SUB_L4 = 3,
  DWavSubbandEOL = 4
}DWavSubband;

///////////////////////////////////////////////////////////////////////////////
// D Y A D I C    P Y R A M I D - PYRAMID WITH NO SUBSAMPLING 
//
class CDyadicPyramid : public CMRANode {

protected: //Functions/Methods
  virtual bool AllocChild();
  bool CreateChild();

  virtual bool DoAnalysis();
  virtual bool DoSynthesis();

  //Constructor for non-root level nodes
  CDyadicPyramid( CDyadicPyramid* pParent) : CMRANode( pParent, true) {;}

public:
  CDyadicPyramid( const CDyadicPyramid&); //Copy constructor
  CDyadicPyramid() : CMRANode() {;}

  virtual bool Copy( CMRANode*);

  virtual int ChildWidth( int ii=0) const  {return Width();}
  virtual int ChildHeight( int ii=0) const {return Height();}
};

///////////////////////////////////////////////////////////////////////////////
// D Y A D I C    W A V E L E T
//
class CDyadicWavelet : public CMRANode {

  friend class CDyadicWaveletChild;

protected: //Variables
  bool m_bDoNoiseSupression;
  bool m_bDoEnhancement;

  C2DArr<float> *m_pGx, *m_pGy;
  CDyadicWaveletChild* m_apChildren[4];

protected: //Methods
  virtual void Free();
  virtual void FreeChildren();
  bool CreateChildren();
  virtual bool DoAnalysis();
  virtual bool DoSynthesis();
 
  bool DoHiPassAnalysis();
  bool DoLoPassAnalysis();

  void ComputeXGradFirstPass();
  void ComputeYGradFirstPass();
  void ComputeXGradSecondPass();
  void ComputeYGradSecondPass();

  bool UpsampleLoPass();
  bool PrepareLoPassSynthesis();
  bool PrepareHiPassSynthesis();

  CDyadicWavelet( CDyadicWavelet* pParent);

  //Enhancement/noise suppression
  bool DoEnhancement();
  bool DoNoiseSuppression();

  void CreateLaplacian(C2DArr<float>* pimgLap, C2DArr<float>* pimgLx);
  float ComputeEnhancementScale(C2DArr<float>* pimgLap);
  void EnhanceLaplacian( C2DArr<float>* pimgLap, float fScale, float fEnhance);
  void ReconstructEnhancement(C2DArr<float>* pimgLap, C2DArr<float>* pimgLx);

public: //Methods
  CDyadicWavelet();
  virtual ~CDyadicWavelet() {Free();}

  //We have to specially control the multi-stage decomposition
  bool Analyze( int iLevels);

  //Because this has yet to be put under control of the manager, we shall
  //make the following public
  bool InputRootData( const C2DArr<float>& input) {return CMRANode::InputRootData( input);}

};

//Child classes for analysis
class CDyadicWaveletChild : public CDyadicWavelet {

  friend class CDyadicWavelet;

protected:
  float **m_ppfRows;
  virtual void Free();
  virtual void SetRowPtrs(DWavSubband enSB);

  CDyadicWaveletChild( CDyadicWavelet* pParent, DWavSubband enSB);

public:
  virtual ~CDyadicWaveletChild() {Free();}
};//class CBiorthogonalWaveletChild

///////////////////////////////////////////////////////////////////////////////
//
// T E M P L A T E    M E M B E R    D E F S
//
  
#endif
