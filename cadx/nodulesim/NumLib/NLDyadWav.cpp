//
//  Module: NLDyadWav.cpp
//
//    Date: 06/28/2002
//
//  Author: Jim Sehnert, Ph.D., Eastman Kodak HI Research Lab
//
// Purpose: Implementation of the dyadic wavelet multi-resolution
//          analysis class
//

#include "NLInternal.h"

//#error //NEED TO WORK ON THE FILTERING CODE...

//Local prototypes
namespace {
  const int FIRST_PASS=1;
  const int SECOND_PASS=2;

  void ComputeGradient( float* pfDst, const float* pfSrc, int iLength, int iPass);
  void LoPassFwd( float* pfDst, const float* pfSrc, int iLength);
  void LoPassInv( float* pfDst, const float* pfSrc, int iLength);

  void KFilter( float* pfDst, const float* pfSrc, int iLength);
  void LFilter( float* pfDst, const float* pfSrc, int iLength);
};

///////////////////////////////////////////////////////////////////////////////
// C D Y A D I C W A V E L E T    M e m b e r s
//

//Constructor for root
CDyadicWavelet::
CDyadicWavelet() : CMRANode(), m_pGx(0), m_pGy(0), m_bDoNoiseSupression(false),
                   m_bDoEnhancement(false)
{
  memset(m_apChildren,0x00,sizeof(m_apChildren));
}


//Constructor for children
CDyadicWavelet::
CDyadicWavelet( CDyadicWavelet* pParent) : CMRANode( pParent, false),
                                           m_pGx(0),m_pGy(0),
                                           m_bDoNoiseSupression(false)
{
  _ASSERT(pParent);
  m_iFilterTaps = pParent->m_iFilterTaps;
  memset(m_apChildren,0x00,sizeof(m_apChildren));
}

//*****************************************************************************
// bool CDyadicWavelet::CreateChildren()
//
bool CDyadicWavelet::CreateChildren()
{
  int iWidth=-1,iHeight=-1;

  if( 0==m_pParent )
  {
    //Have to get dimens from the input data
    if( m_pData && m_pData->IsValid() )
    {
      iWidth = m_pData->Width();
      iHeight = m_pData->Height();
    }
  }
  else
  {
    iWidth = m_pParent->Width()>>1;
    iHeight = m_pParent->Height()>>1;
  }
  
  if( -1 == iWidth )
    return false;

  _ASSERT( 0==m_pChild );

  m_pGx = new C2DArr<float>( iWidth, iHeight);
  m_pGy = new C2DArr<float>( iWidth, iHeight);
  
  if( !m_pGx || !m_pGy || !m_pGx->IsValid() || !m_pGy->IsValid() )
  {
    FreeChildren();
    return false;
  }
 
  //Create the lo-pass child in four tiles for easy maintenance
  m_apChildren[0] = new CDyadicWaveletChild( this, DWAV_SUB_L1);
  m_apChildren[1] = new CDyadicWaveletChild( this, DWAV_SUB_L2);
  m_apChildren[2] = new CDyadicWaveletChild( this, DWAV_SUB_L3);
  m_apChildren[3] = new CDyadicWaveletChild( this, DWAV_SUB_L4);

  m_pChild = m_apChildren[0];

  return true;
}//bool CDyadicWavelet::CreateChildren()

//*****************************************************************************
// void CDyadicWavelet::Free()
//
// Frees all allocated memory, including the child subbands.
//
void CDyadicWavelet::
Free()
{
  FreeChildren();
  CMRANode::Free();
}//void CDyadicWavelet::Free()

//*****************************************************************************
// bool CDyadicWavelet::FreeChildren()
//
// Free the children classes - typically done after synthesys.
//
void CDyadicWavelet::FreeChildren()
{
  delete m_pGx;
  m_pGx=0;
  delete m_pGy;
  m_pGy=0;

  for(int i=0; i<4; i++)
  {
    delete m_apChildren[i];
    m_apChildren[i] = 0;
  }

  m_pChild = 0;
}//void CDyadicWavelet::FreeChildren()

//*****************************************************************************
// void CDyadicWavelet::DoAnalysis()
//
// Decompose this subband into three channels - x,y edges and Gaussian smoothed.
//
bool CDyadicWavelet::
DoAnalysis()
{
  if( !CreateChildren() )
    return false;

  if( !DoHiPassAnalysis() || !DoLoPassAnalysis() )
  {
    return false;
  }

  return true;
}//bool CDyadicWavelet::DoAnalysis()

//*****************************************************************************
// void CDyadicWavelet::Analyze()
//
// Performs a full decomposition of the image data
//
bool CDyadicWavelet::
Analyze(int iLevels)
{
  if( iLevels <= 0 )
  {
    return true;
  }

  //Decompose this
  if( DoAnalysis() )
  {
    //Now decompose children if required
    int iNextLev = iLevels - 1;
    if( iNextLev > 0 )
    {
      for( int i=0; i<4; i++)
      {
        bool bRes = m_apChildren[i]->Analyze( iNextLev);
        if( !bRes )
          return false;
      }
    }
  }

  return true;
}//bool CDyadicWavelet::Analyze(int iLevels)

//*****************************************************************************
// bool CDyadicWavelet::DoSynthesis()
//
// Recompose this subband from it lpass + hipass data.
//
bool CDyadicWavelet::
DoSynthesis()
{
  if( 0==m_pChild )
    return true;

  for( int i=1; i<4; i++ )
  {
    if( !m_apChildren[i]->Synthesize() )
      return false;
  }

  //Prepare lo pass and hi pass bands for summation
  if( !PrepareLoPassSynthesis() ||
      !DoEnhancement() ||
      !PrepareHiPassSynthesis() )
  {
    return false;
  }

  //Combine the results
  int iHeight = m_pData->Height();
  int iWidth = m_pData->Width();
  float** ppfGx = m_pGx->Access();
  float** ppfGy = m_pGy->Access();
  float** ppfData = m_pData->Access();

  for( i=0; i<iHeight; i++)
  {
    float* pfData = ppfData[i];
    float* pfGx = ppfGx[i];
    float* pfGy = ppfGy[i];

    for( int j=0; j<iWidth; j++, pfData++)
      *pfData = pfData[0] + (*pfGx++) + (*pfGy++);
  }

  return true;
}//bool CDyadicWavelet::DoSynthesis()

//*****************************************************************************
// void CDyadicWavelet::DoLoPassAnalysis()
//
// Compute the 4 subsampled lo-pass subbands - memory already allocated.
//
bool CDyadicWavelet::DoLoPassAnalysis()
{
  int iHeight = m_pData->Height();
  int iWidth = m_pData->Width();
  float** ppfData = m_pData->Access();
  int iMax = Max(iHeight, iWidth);
  CMemBlock<float> buf(2*iMax);
  float* pfBuf1 = buf.Access();
  float* pfBuf2 = pfBuf1 + iMax;

  //Lo-pass on the rows of the image
  size_t sizeCpy = iWidth*sizeof(float);
  for( int i=0; i<iHeight; i++)
  {
    memcpy( pfBuf1, ppfData[i], sizeCpy);
    LoPassFwd(ppfData[i], pfBuf1, iWidth);
  }

  //Lo-pass on the cols of the image
  for( i=0; i<m_pData->Width(); i++)
  {
    //Copy the col into buffer 1
    for( int j=0; j<iHeight; j++)
      pfBuf1[j] = ppfData[j][i];

    //Process into buffer2
    LoPassFwd(pfBuf2, pfBuf1, iHeight);

    //Copy back to buffer2
    for( j=0; j<iHeight; j++)
      ppfData[j][i] = pfBuf2[j];
  }

  return true;
}

//*****************************************************************************
// void CDyadicWavelet::DoHiPassAnalysis()
//
// Compute the x,y edge bands - memory already allocated.
//
bool CDyadicWavelet::DoHiPassAnalysis()
{
  //Compute the gradients first, preparing for noise suppression
  ComputeXGradFirstPass();
  ComputeYGradFirstPass();

  //Do any noise suppression
  if( this->m_bDoNoiseSupression )
    DoNoiseSuppression();

  //Compute final gradient to finish the hi-pass filter
  ComputeXGradSecondPass();
  ComputeYGradSecondPass();

  return true;
}//bool CDyadicWavelet::DoAnalysisEdgeBands()

//*****************************************************************************
// void CDyadicWavelet::ComputeXGradFirstPass()
//
// Filter to the x-grad, transferring parent buffer into Gx buffer
//
void CDyadicWavelet::ComputeXGradFirstPass()
{
  const float** m_ppfPar = (const float**) m_pData->Access();
  float** ppfGx = m_pGx->Access();
  for( int i=0; i<m_pGx->Height(); i++)
  {
    ComputeGradient(ppfGx[i], m_ppfPar[i], m_pGx->Width(), FIRST_PASS);
  }
}

//*****************************************************************************
// void CDyadicWavelet::ComputeYGradFirstPass()
//
// Filter to the y-grad, transferring parent buffer into Gy buffer
//
void CDyadicWavelet::ComputeYGradFirstPass()
{
  int iHeight = m_pGy->Height();
  const float** m_ppfPar = (const float**) m_pData->Access();
  float** ppfGy = m_pGy->Access();
  
  CMemBlock<float> buf(iHeight*2);
  _ASSERT( buf.IsValid());
  float* pfBuf1 = buf.Access();
  float* pfBuf2 = pfBuf1 + iHeight;

  for( int i=0; i<m_pGy->Width(); i++)
  {
    //Copy parent->buffer
    for( int j=0; j<iHeight; j++)
    {
      pfBuf1[j] = m_ppfPar[j][i];
    }

    //Transform buffer1->buffer2
    ComputeGradient(pfBuf2, pfBuf1, iHeight, FIRST_PASS);

    //Copy buffer2->grady
    for( j=0; j<iHeight; j++)
    {
      ppfGy[j][i] = pfBuf2[j] ;
    }
  }
}

//*****************************************************************************
// void CDyadicWavelet::ComputeXGradSecondPass()
//
// Filter to the x-grad, in-place
//
void CDyadicWavelet::ComputeXGradSecondPass()
{
  int iWidth = m_pGx->Width();
  float** ppfGx = m_pGx->Access();
  CMemBlock<float> buf(iWidth);
  _ASSERT(buf.IsValid());
  float* pfBuf = buf.Access();
  size_t sizeCpy = iWidth * sizeof(float);

  for( int i=0; i<m_pGx->Height(); i++)
  {
    memcpy(pfBuf, ppfGx[i], sizeCpy);
    ComputeGradient(ppfGx[i], pfBuf, iWidth, SECOND_PASS);
  }
}

//*****************************************************************************
// void CDyadicWavelet::ComputeYGradSecondPass()
//
// Filter to the y-grad, in-place
//
void CDyadicWavelet::ComputeYGradSecondPass()
{
  int iHeight = m_pGy->Height();
  float** ppfGy = m_pGy->Access();
  
  CMemBlock<float> buf(iHeight*2);
  _ASSERT( buf.IsValid());
  float* pfBuf1 = buf.Access();
  float* pfBuf2 = pfBuf1 + iHeight;

  for( int i=0; i<m_pGy->Width(); i++)
  {
    //Copy parent->buffer
    for( int j=0; j<iHeight; j++)
    {
      pfBuf1[j] = ppfGy[j][i];
    }

    //Transform buffer1->buffer2
    ComputeGradient(pfBuf2, pfBuf1, iHeight, SECOND_PASS);

    //Copy buffer2->grady
    for( j=0; j<iHeight; j++)
    {
      ppfGy[j][i] = pfBuf2[j] ;
    }
  }
}

//*****************************************************************************
// bool CDyadicWavelet::UpsampleLoPass()
//
// Upample the tiled lo pass data
//
bool CDyadicWavelet::UpsampleLoPass()
{
  int iHeight = m_pData->Height();
  int iWidth = m_pData->Width();
  float** ppfData = m_pData->Access();
  int iMax = Max(iHeight, iWidth);
  CMemBlock<float> buf(2*iMax);
  if( !buf.IsValid() )
    return false;
  float* pfBuf1 = buf.Access();
  float* pfBuf2 = pfBuf1 + iMax;

  //Do the rows first
  for( int i=0; i<iHeight; i++)
  {
    memcpy( pfBuf1, ppfData[i], iWidth*sizeof(float));
    float* pfData = ppfData[i];

    float *pf1 = pfBuf1, *pf2=pfBuf1+(iWidth>>1);
    for( int j=0,k=0; j<iWidth; j+=2,k++)
    {
      pfData[j] = pf1[k];
      pfData[j+1] = pf2[k];
    }
  }

  //Now do the cols
  for( i=0; i<iWidth; i++)
  {
    for( int j=0; j<iHeight; j++)
      pfBuf1[j] = ppfData[j][i];

    float *pf1 = pfBuf1, *pf2=pfBuf1+(iHeight>>1);
    int k;
    for( j=0,k=0; j<iHeight; j+=2,k++)
    {
      pfBuf2[j]  = pf1[k];
      pfBuf2[j+1] = pf2[k];
    }

    for( j=0; j<iHeight; j++)
      ppfData[j][i] = pfBuf2[j];
  }

  return true;
}

//*****************************************************************************
// bool CDyadicWavelet::PrepareForLoPassSynthesis()
//
// Filter to the y-grad, in-place
//
bool CDyadicWavelet::PrepareLoPassSynthesis()
{
  //Upsample data to the normal configuration
  if( !UpsampleLoPass() )
    return false;

  int iHeight = m_pData->Height();
  int iWidth = m_pData->Width();
  float** ppfData = m_pData->Access();
  int iMax = Max(iHeight, iWidth);
  CMemBlock<float> buf(2*iMax);
  if( !buf.IsValid() )
    return false;
  float* pfBuf1 = buf.Access();
  float* pfBuf2 = pfBuf1 + iMax;

  //Lo-pass on the rows of the image
  size_t sizeCpy = iWidth*sizeof(float);
  for( int i=0; i<iHeight; i++)
  {
    memcpy( pfBuf1, ppfData[i], sizeCpy);
    LoPassInv(ppfData[i], pfBuf1, iWidth);
  }

  //Lo-pass on the cols of the image
  for( i=0; i<m_pData->Width(); i++)
  {
    //Copy the col into buffer 1
    for( int j=0; j<iHeight; j++)
      pfBuf1[j] = ppfData[j][i];

    //Process into buffer2
    LoPassInv(pfBuf2, pfBuf1, iHeight);

    //Copy back to buffer2
    for( j=0; j<iHeight; j++)
      ppfData[j][i] = pfBuf2[j];
  }

  return true;
}

//*****************************************************************************
// bool CDyadicWavelet::PrepareForHiPassSynthesis()
//
// Filter the the x and y gradients for synthesis
//
// Gx is filtered with Kx Ly
// Gy is filtered with Lx Ky
//
bool CDyadicWavelet::PrepareHiPassSynthesis()
{
  int iHeight = m_pData->Height();
  int iWidth = m_pData->Width();
  float** ppfGx = m_pGx->Access();
  float** ppfGy = m_pGy->Access();
  
  int iMax = Max(iHeight, iWidth);
  CMemBlock<float> buf(2*iMax);
  if( !buf.IsValid() )
    return false;
  float* pfBuf1 = buf.Access();
  float* pfBuf2 = pfBuf1 + iMax;

  //Do the row-filtering Kx on Gx and Lx on Gy
  size_t sizeCpy = iWidth*sizeof(float);
  for( int i=0; i<iHeight; i++)
  {
    memcpy( pfBuf1, ppfGx[i], sizeCpy);
    KFilter(ppfGx[i], pfBuf1, iWidth);

    memcpy( pfBuf1, ppfGy[i], sizeCpy);
    LFilter(ppfGy[i], pfBuf1, iWidth);
  }

  //Do the col-filtering Ly on Gx and Ky on Gy
  for( i=0; i<m_pData->Width(); i++)
  {
    //Copy the col into buffer 1
    for( int j=0; j<iHeight; j++)
      pfBuf1[j] = ppfGx[j][i];

    //Process into buffer2
    LFilter(pfBuf2, pfBuf1, iHeight);

    //Copy back to Gx
    for( j=0; j<iHeight; j++)
      ppfGx[j][i] = pfBuf2[j];

    //Copy the col into buffer 1
    for( j=0; j<iHeight; j++)
      pfBuf1[j] = ppfGy[j][i];

    //Process into buffer2
    KFilter(pfBuf2, pfBuf1, iHeight);

    //Copy back to Gx
    for( j=0; j<iHeight; j++)
      ppfGy[j][i] = pfBuf2[j];
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// C B I O R T H O G W A V E L E T C H I L D    M E T H O D S
//
///////////////////////////////////////////////////////////////////////////////

CDyadicWaveletChild::
CDyadicWaveletChild( CDyadicWavelet* pParent, 
                     DWavSubband enSB) : CDyadicWavelet( pParent),
                                         m_ppfRows(0)
{
  _ASSERT(m_pParent);
  int iWidth  = (pParent->Width()+1)>>1;
  if( ((int)1&pParent->Width()) && (enSB==DWAV_SUB_L2 || enSB==DWAV_SUB_L4) )
    iWidth--;
  int iHeight = (pParent->Height()+1)>>1;
  if( ((int)1&pParent->Height()) && (enSB==DWAV_SUB_L1 || enSB==DWAV_SUB_L3) )
    iHeight--;

  m_ppfRows = new float*[iHeight];
  if( m_ppfRows )
  {
    m_pData = new C2DArr<float>( m_ppfRows,iWidth,iHeight);
    SetRowPtrs(enSB);
  }
}

//*****************************************************************************
// void CDyadicWaveletChild::Free()
//
// Frees all allocated memory.
//
void CDyadicWaveletChild::
Free()
{
  CDyadicWavelet::Free();
  delete[] m_ppfRows;
  m_ppfRows=0;
}

//*****************************************************************************
// void CDyadicWaveletChild::SetRowPtrs()
//
// Set the appropriate adresses for the child subbands from the parent block
void CDyadicWaveletChild::
SetRowPtrs(DWavSubband enSB)
{
  _ASSERT( m_pParent);
  _ASSERT(m_ppfRows);

  //Calculate ptr offset within each row w/r/t parent data block
  int iRowOffset=0;
  if( enSB==DWAV_SUB_L2 || enSB==DWAV_SUB_L4 )
    iRowOffset = (m_pParent->Width()+1)>>1;

  float **ppfParent = m_pParent->m_pData->Access();
  if( enSB==DWAV_SUB_L3 || enSB==DWAV_SUB_L4 )
    ppfParent += (m_pParent->Height()+1)>>1;


  for( int i=0; i<Height(); i++)
  {
    m_ppfRows[i] = ppfParent[i] + iRowOffset;
  }
}//void CDyadicWaveletChild::SetRowPtrs()

//*****************************************************************************
// void CDyadicWaveletChild::DoEnhancement()
//
// Carry out a non-linear enhnacement routine...
bool CDyadicWavelet::DoEnhancement()
{
  //TESTING
  if(0)
  {
    int iLevel = Level();
    if( 2==iLevel )
    {      
      for( int i=0; i<m_pData->Height(); i++)
      {
        for(int j=0; j<m_pData->Width(); j++)
          m_pData->Access()[i][j] = 40.0f;
      }
    }
  }
  //END TESTING

  const float ENH_FACTOR = 20.0f;

  int iWidth=m_pData->Width(),iHeight=m_pData->Height();

  C2DArr<float> imgLap( iWidth, iHeight);
  C2DArr<float> imgLx(iWidth, iHeight);
  if( !imgLap.IsValid() | !imgLx.IsValid() )
    return false;

  CreateLaplacian( &imgLap, &imgLx);
  
  //TESTING
  //!!! Here we need to examine the edges !!! 
  if(0)
  {
    const int MAX_FILE_ELTS = Sqr(2048);
    const char* aFNames[]={"c:\\MyJunk\\Edge_Lev1.bin",
                       "c:\\MyJunk\\Edge_Lev2.bin",
                       "c:\\MyJunk\\Edge_Lev3.bin",
                       "c:\\MyJunk\\Edge_Lev4.bin"};
    int iLevel = Level();
    const char* szFName = aFNames[iLevel];
    bool bExists=false;
    FILE* pFile=fopen(szFName,"rb");
    if( pFile )
      bExists = true;
    
    float* pfExistingElts=new float[MAX_FILE_ELTS];
    long iExistingElts=0;
    
    if( bExists )
    {
      fread( &iExistingElts,1,sizeof(long),pFile);
      fread( pfExistingElts, 1, iExistingElts*sizeof(float), pFile);
      fclose( pFile);
    }
    
    float* pfNewElts = pfExistingElts + iExistingElts;
    long iNewElts=0;
    size_t sizeRow = imgLap.Width() * sizeof(float);
    for( int i=0; i<imgLap.Height(); i++)
    {
      memcpy( pfNewElts + iNewElts, imgLap.Access()[i], sizeRow);
      iNewElts += imgLap.Width();
    }

    pFile=fopen(szFName,"wb");
    _ASSERT(pFile);
    iExistingElts += iNewElts;
    fwrite( &iExistingElts, 1, sizeof(long), pFile);
    fwrite( pfExistingElts, 1, sizeof(float)*iExistingElts, pFile);
    delete pfExistingElts;
    fclose(pFile);
  }
  //!!! End of edge examination !!!
  //END TESTING

  float fScale = 20.0f;//ComputeEnhancementScale(&imgLap);
  fScale *= 0.1f;
  EnhanceLaplacian( &imgLap, fScale, ENH_FACTOR);

  //All done, reconstruct
  ReconstructEnhancement(&imgLap, &imgLx);

  return true;
}//bool CDyadicWavelet::DoEnhancement()


bool CDyadicWavelet::DoNoiseSuppression()
{
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// float CDyadicWavelet::ComputeEnhancementScale(C2DArr<float>* pimgLap)
//
// Compute a scale for carrying out an enhancement
//
float CDyadicWavelet::ComputeEnhancementScale(C2DArr<float>* pimgLap)
{
  //As a first cut here, simply return the max from a cropped, subsampled
  //version
  const int CROP = 5;
  const int SKIP = 3;

  int iWidth=m_pData->Width(),iHeight=m_pData->Height();
  C2DArr<float>* pimgCrop = pimgLap->Crop(CROP,CROP,iWidth-CROP, iHeight-CROP);
  float fScale = 0.0f;

  for( int i=0; i<pimgCrop->Height(); i+= SKIP )
  {
    float* pfCrop = pimgCrop->Access()[i];
    for( int j=0; j< pimgCrop->Width(); j+= SKIP )
    {
      float f = Abs( *pfCrop++);
      if( f > fScale )
        fScale = f;
    }
  }

  delete pimgCrop;
  return fScale;
}//float CDyadicWavelet::ComputeEnhancementScale(C2DArr<float>* pimgLap)

///////////////////////////////////////////////////////////////////////////////
//void CDyadicWavelet::CreateLaplacian( C2DArr<float>* pimgLap, C2DArr<float>* pimgLx)
//
// Create a 2D Laplacian-type image from the Gx and Gy Laplacian images
void CDyadicWavelet::
CreateLaplacian( C2DArr<float>* pimgLap, C2DArr<float>* pimgLx)
{
  const float fTiny = 1.0e-7f;  
  int iWidth=m_pData->Width(),iHeight=m_pData->Height();

  float** ppfLap = pimgLap->Access();
  float** ppfLx  = pimgLx->Access();
  float** ppfGx  = m_pGx->Access();
  float** ppfGy  = m_pGy->Access();

  for( int i=0; i<iHeight; i++)
  {
    float* pfLap= ppfLap[i];
    float* pfLx = ppfLx[i];
    float* pfGx = ppfGx[i];
    float* pfGy = ppfGy[i];

    for( int j=0; j<iWidth; j++, pfLap++, pfLx++, pfGx++, pfGy++)
    {
      float fSum = *pfGx + *pfGy;
      if( Abs(fSum) < fTiny )
      {
        *pfLap = 0.0f;
        *pfLx = 0;
      }
      else
      {
        *pfLap = fSum;
        *pfLx = *pfGx / fSum;
      }
    }
  }
}//bool CDyadicWavelet::CreateLaplacian();

///////////////////////////////////////////////////////////////////////////////
//void CDyadicWavelet::
//  ReconstructEnhancement( C2DArr<float>* pimgLap, C2DArr<float>* pimgLx)
// Recompose Gx and Gy from the enhanced laplacian
//
void CDyadicWavelet::
ReconstructEnhancement( C2DArr<float>* pimgLap, C2DArr<float>* pimgLx)
{
  int iWidth=m_pData->Width(),iHeight=m_pData->Height();

  float** ppfLap = pimgLap->Access();
  float** ppfLx  = pimgLx->Access();
  float** ppfGx  = m_pGx->Access();
  float** ppfGy  = m_pGy->Access();

  for( int i=0; i<iHeight; i++)
  {
    float* pfLap= ppfLap[i];
    float* pfLx = ppfLx[i];
    float* pfGx = ppfGx[i];
    float* pfGy = ppfGy[i];

    for( int j=0; j<iWidth; j++, pfLap++, pfLx++, pfGx++, pfGy++)
    {
      *pfGx = (*pfLap) * (*pfLx);
      *pfGy = (*pfLap) * (1.0f - *pfLx);
      if( 0)//0==Level() || 1==Level() )
      {//TESTING
        *pfGx = *pfGy = 0.0f;
      }
    }
  }
}//bool void CDyadicWavelet::ReconstructEnhancement();

///////////////////////////////////////////////////////////////////////////////
//void CDyadicWavelet::
//  EnhanceLaplacian( C2DArr<float>* pimgLap, float fScale, float fEnhance)
//  Do the actual image enhancement
//
// COMMENT: FOR EDA TESTING, WE SHALL UTILIZE AN ENHANCEMENT AIMED AT KEEPING THE
//          NOISE FROM EXCESSIVELY BEING ENHANCED. THIS IS DONE WITH A THREE PART
//          FUNCTION WITH BREAKPOINTS T1 and T2 with DELTA_T = T2-T1. THE FUNCTION 
//          IS DEFINED AS FOLLOWS:
//          F(X)=X if X<=T1                  //TOO MUCH NOISE BELOW T1
//          F(X)=T1+A*(X-T1) if X<=T2 (A>=1) //SUBTLE AREAS TO ENHANCE
//          F(X)=X+DELTA_T*(A-1)             //STRONG AREAS, NO ENHANCEMENT NEEDED
//
//          THE THRESHOLDS ARE BASED ON ESTIMATES OF THE NOISE IN THE LAPLACIAN SUBBAND
//
void CDyadicWavelet::
EnhanceLaplacian( C2DArr<float>* pimgLap, float fScale, float fEnhance)
{
  int iLevel = Level();
  //if(0==iLevel )
    //return;

  //TESTING
  if( 1 )
  {
    //The three part EDA-Enhancement attempt
    float af10xSigmas[] = {33.40f,12.565f,7.83f};
    float af1xSigmas[] = {74.43f,26.542f,17.195f};
    float fThresh1 = 0.7f*af1xSigmas[iLevel];;//af10xSigmas[iLevel];
    float fThresh2 = 5 * af1xSigmas[iLevel];
    float fDelta_T = fThresh2 - fThresh1;
    float fA = 10.0;
    if( iLevel==2 )
      fA/=2.0f;
    else if(iLevel==0 )
      fA/=9.0;

    float fOffset = fDelta_T * (fA-1);
    for( int i=0; i<pimgLap->Height(); i++)
    {
      float* pfLap = pimgLap->Access()[i];
      if( false&&0==iLevel )
      {
        memset(pfLap, 0x00, pimgLap->Width()*sizeof(float));
        continue;
      }

      for( int j=0; j<pimgLap->Width(); j++)
      {
        float fVal = pfLap[j];
        int iSign = 1;
        if( fVal < 0.0 ) iSign=-1;
        fVal = Abs(fVal);
        if( fVal > fThresh1 )
        {
          if( fVal < fThresh2 )
          {
            fVal = fThresh1 + fA*(fVal-fThresh1);
          }
          else
          {
            fVal += fOffset;
          }
        }

        fVal *=iSign;
        pfLap[j] = fVal;
      }
    }
     return;
  }

  int iWidth=m_pData->Width(),iHeight=m_pData->Height();
  float fOffset = fScale * (fEnhance - 1.0f);
  float** ppfLap = pimgLap->Access();

  for( int i=0; i<iHeight; i++)
  {
    float* pfLap= ppfLap[i];

    for( int j=0; j<iWidth; j++, pfLap++)
    {
      float fVal = Abs(*pfLap);
      if( fVal < fScale )
        fVal *= fEnhance;
      else
        fVal += fOffset;

      if( *pfLap < 0.0f )
        *pfLap = -1.0f * fVal;
      else
        *pfLap = fVal;
    }
  }
}//void CDyadicWavelet::EnhanceLaplacian()


//Local definitions
namespace {
  void ComputeGradient( float* pfDst, const float* pfSrc, int iLength, int iPass)
  {
    if( iPass==FIRST_PASS )
    {
      //First pass... set the first element
      pfDst[0] = (pfSrc[0]-pfSrc[1])*2.0f; //Due to mirror extension of data
    }
    else
    {
      //Second pass... set the last element and offset the output buffer - remember the
      //output of the first past yields antisymmetric half-sample symmetry
      pfDst[iLength-1] = (pfSrc[iLength-1]) * -4.0f;
      pfDst--;
    }

    for( int i=1; i<iLength; i++)
    {
      pfDst[i] = (pfSrc[i] - pfSrc[i-1]) * 2.0f;
    }
  }
  
  
  void LaplacianFilter( float* pfDst, float* pfSrc, int iLength, int iScale)
  {
    if( 1==iScale )
    {
      //Do the ordinary filtering
      pfDst[0]         = 2.0f * (pfSrc[1] - pfSrc[0]);
      pfSrc[iLength-1] = 2.0f * (pfSrc[iLength-2] - pfSrc[iLength-1]);

      pfSrc++;
      pfDst++;
      for( int i=1; i<iLength-1; i++,pfSrc++)
      {
        *pfDst++ = pfSrc[-1] - 2.0f * pfSrc[0] + pfSrc[1];
      }
      return;
    }

    //Otherwise, break data into equivalence classes
    int iMaxClassLen = (iLength + iScale/2) / iScale;
    CNumArr<float> buf(2*iMaxClassLen);
    float *pfBuf1 = buf.Access();
    float *pfBuf2 = pfBuf1 + iMaxClassLen;

    for( int iOffset=0; iOffset<iScale; iOffset++)
    {
      for( int j=iOffset,jj=0; j<iLength; j+=iScale,jj++)
      {
        pfBuf1[jj] = pfSrc[j];
      }

      int iClassLen = jj;
      LaplacianFilter( pfBuf2, pfBuf1, iClassLen, 1);
      for( j=iOffset,jj=0; j<iLength; j+=iScale,jj++)
      {
        pfDst[j] = pfBuf2[jj];
      }
    }
  }
};

//LOCAL DEFS
namespace {
  //The inverse K-filter
  void KFilter( float* pfOut, const float* pfIn, int iLen)
  {
    const float afKTaps[] = {-0.13671875f, -0.0458984375f, -0.009765625f, -0.0009765625f};

    //Handle the first 3 cases outside the mani loop
    pfOut[0] = afKTaps[0] * pfIn[0] + 
               afKTaps[1] * (pfIn[1] + pfIn[1]) + 
               afKTaps[2] * (pfIn[2] + pfIn[2]) +
               afKTaps[3] * (pfIn[3] + pfIn[3]);
    pfOut[1] = afKTaps[0] * pfIn[1] + 
               afKTaps[1] * (pfIn[2] + pfIn[0]) + 
               afKTaps[2] * (pfIn[3] + pfIn[1]) +
               afKTaps[3] * (pfIn[4] + pfIn[2]);
    pfOut[2] = afKTaps[0] * pfIn[2] + 
               afKTaps[1] * (pfIn[3] + pfIn[1]) + 
               afKTaps[2] * (pfIn[4] + pfIn[0]) +
               afKTaps[3] * (pfIn[5] + pfIn[1]);

    //The main loop
    int iMin = 3;
    int iMax = iLen-3;
    for( int i=iMin; i<iMax; i++)
    {
      pfOut[i] = afKTaps[0] * pfIn[i] + afKTaps[1] * (pfIn[i+1] + pfIn[i-1]) + 
                                        afKTaps[2] * (pfIn[i+2] + pfIn[i-2]) +
                                        afKTaps[3] * (pfIn[i+3] + pfIn[i-3]);
    }

    //Handle the last 3 cases outside the main loop
    pfOut[iLen-3] = afKTaps[0] * pfIn[iLen-3] +
                    afKTaps[1] * (pfIn[iLen-2] + pfIn[iLen-4]) + 
                    afKTaps[2] * (pfIn[iLen-1] + pfIn[iLen-5]) +
                    afKTaps[3] * (pfIn[iLen-2] + pfIn[iLen-6]);
    pfOut[iLen-2] = afKTaps[0] * pfIn[iLen-2] + 
                    afKTaps[1] * (pfIn[iLen-1] + pfIn[iLen-3]) + 
                    afKTaps[2] * (pfIn[iLen-2] + pfIn[iLen-4]) +
                    afKTaps[3] * (pfIn[iLen-3] + pfIn[iLen-5]);
    pfOut[iLen-1] = afKTaps[0] * pfIn[iLen-1] + 
                    afKTaps[1] * (pfIn[iLen-2] + pfIn[iLen-2]) + 
                    afKTaps[2] * (pfIn[iLen-3] + pfIn[iLen-3]) +
                    afKTaps[3] * (pfIn[iLen-4] + pfIn[iLen-4]);
  }

  //The inverse L-filter
  void LFilter( float* pfOut, const float* pfIn, int iLen)
  {
    const float afLTaps[] = {0.63671875f, 0.109375f, 0.0546875f, 0.015625f, 0.001953125f};

    //Handle the first 4 cases outside the mani loop
    pfOut[0] = afLTaps[0] * pfIn[0] + 
               afLTaps[1] * (pfIn[1] + pfIn[1]) + 
               afLTaps[2] * (pfIn[2] + pfIn[2]) +
               afLTaps[3] * (pfIn[3] + pfIn[3]) +
               afLTaps[4] * (pfIn[4] + pfIn[4]);
    pfOut[1] = afLTaps[0] * pfIn[1] + 
               afLTaps[1] * (pfIn[2] + pfIn[0]) + 
               afLTaps[2] * (pfIn[3] + pfIn[1]) +
               afLTaps[3] * (pfIn[4] + pfIn[2]) +
               afLTaps[4] * (pfIn[5] + pfIn[3]);
    pfOut[2] = afLTaps[0] * pfIn[2] + 
               afLTaps[1] * (pfIn[3] + pfIn[1]) + 
               afLTaps[2] * (pfIn[4] + pfIn[0]) +
               afLTaps[3] * (pfIn[5] + pfIn[1]) +
               afLTaps[4] * (pfIn[6] + pfIn[2]);
    pfOut[3] = afLTaps[0] * pfIn[3] + 
               afLTaps[1] * (pfIn[4] + pfIn[2]) + 
               afLTaps[2] * (pfIn[5] + pfIn[1]) +
               afLTaps[3] * (pfIn[6] + pfIn[0]) +
               afLTaps[4] * (pfIn[7] + pfIn[1]);

    //The main loop
    int iMin = 4;
    int iMax = iLen-4;
    for( int i=iMin; i<iMax; i++)
    {
      pfOut[i] = afLTaps[0] * pfIn[i] + afLTaps[1] * (pfIn[i+1] + pfIn[i-1]) + 
                                        afLTaps[2] * (pfIn[i+2] + pfIn[i-2]) +
                                        afLTaps[3] * (pfIn[i+3] + pfIn[i-3]) +
                                        afLTaps[4] * (pfIn[i+4] + pfIn[i-4]);
    }

    //Handle the last 4 cases outside the main loop
    pfOut[iLen-4] = afLTaps[0] * pfIn[iLen-4] +
                    afLTaps[1] * (pfIn[iLen-3] + pfIn[iLen-5]) + 
                    afLTaps[2] * (pfIn[iLen-2] + pfIn[iLen-6]) +
                    afLTaps[3] * (pfIn[iLen-1] + pfIn[iLen-7]) +
                    afLTaps[4] * (pfIn[iLen-2] + pfIn[iLen-8]);
    pfOut[iLen-3] = afLTaps[0] * pfIn[iLen-3] +
                    afLTaps[1] * (pfIn[iLen-2] + pfIn[iLen-4]) + 
                    afLTaps[2] * (pfIn[iLen-1] + pfIn[iLen-5]) +
                    afLTaps[3] * (pfIn[iLen-2] + pfIn[iLen-6]) +
                    afLTaps[4] * (pfIn[iLen-3] + pfIn[iLen-7]);
    pfOut[iLen-2] = afLTaps[0] * pfIn[iLen-2] + 
                    afLTaps[1] * (pfIn[iLen-1] + pfIn[iLen-3]) + 
                    afLTaps[2] * (pfIn[iLen-2] + pfIn[iLen-4]) +
                    afLTaps[3] * (pfIn[iLen-3] + pfIn[iLen-5]) +
                    afLTaps[4] * (pfIn[iLen-4] + pfIn[iLen-6]);
    pfOut[iLen-1] = afLTaps[0] * pfIn[iLen-1] + 
                    afLTaps[1] * (pfIn[iLen-2] + pfIn[iLen-2]) + 
                    afLTaps[2] * (pfIn[iLen-3] + pfIn[iLen-3]) +
                    afLTaps[3] * (pfIn[iLen-4] + pfIn[iLen-4]) +
                    afLTaps[4] * (pfIn[iLen-5] + pfIn[iLen-5]);
  }

  //Lo pass the array - the result is completely normal....
  void LoPassInv(float* pfOut, const float* pfIn, int iLen)
  {
    int iHlf = iLen/2;
    int iMin=2;
    int iMax = iLen-2;

    //Handle the first two cases outside the main loop using E11 symmetry
    pfOut[0] = 0.375f * pfIn[0] +   0.2500f * 2.0f * pfIn[1] + 
                                    0.0625f * 2.0f * pfIn[2];
    pfOut[1] = 0.375f * pfIn[1] + 0.2500f *  (pfIn[2] + pfIn[0]) + 
                                  0.0625f *(pfIn[3] + pfIn[1]);

    //The main loop
    for( int i=iMin; i<iMax; i++)
    {
      pfOut[i] = 0.375f * pfIn[i] + 0.2500f * (pfIn[i+1] + pfIn[i-1]) + 
                                    0.0625f * (pfIn[i+2] + pfIn[i-2]);
    }

    //Handle the last two cases outside the main loop using E11 symmetry 
    pfOut[iLen-2] = 0.375f * pfIn[iLen-2] + 0.2500f * (pfIn[iLen-1] + pfIn[iLen-3]) + 
                                    0.0625f * (pfIn[iLen-2] + pfIn[iLen-4]);
    pfOut[iLen-1] = 0.375f * pfIn[iLen-1] + 0.2500f * 2.0f * pfIn[iLen-2] + 
                                    0.0625f * 2.0f * pfIn[iLen-3];
  }

  //Lo pass the array - the result is two half-sample arrays...
  void LoPassFwd( float* pfOut, const float* pfIn, int iLen)
  {
    int iHlf = iLen/2;
    int iMin=2;
    int iMax = iLen-2;

    //Handle the first two cases outside the main loop using E11 symmetry
    pfOut[0] = 0.375f * pfIn[0] +   0.2500f * 2.0f * pfIn[1] + 
                                    0.0625f * 2.0f * pfIn[2];
    pfOut[iHlf] = 0.375f * pfIn[1] + 0.2500f *  (pfIn[2] + pfIn[0]) + 
                                    0.0625f *(pfIn[3] + pfIn[1]);

    //The main loop
    for( int i=iMin; i<iMax; i++)
    {
      pfOut[i/2] = 0.375f * pfIn[i] + 0.2500f * (pfIn[i+1] + pfIn[i-1]) + 
                                    0.0625f * (pfIn[i+2] + pfIn[i-2]);
      i++;
      pfOut[iHlf+i/2] = 0.375f * pfIn[i] + 0.2500f * (pfIn[i+1] + pfIn[i-1]) + 
                                    0.0625f * (pfIn[i+2] + pfIn[i-2]);
    }

    //Handle the last two cases outside the main loop using E11 symmetry 
    pfOut[iHlf-1] = 0.375f * pfIn[iLen-2] + 0.2500f * (pfIn[iLen-1] + pfIn[iLen-3]) + 
                                    0.0625f * (pfIn[iLen-2] + pfIn[iLen-4]);
    pfOut[iLen-1] = 0.375f * pfIn[iLen-1] + 0.2500f * 2.0f * pfIn[iLen-2] + 
                                    0.0625f * 2.0f * pfIn[iLen-3];
  }
};
