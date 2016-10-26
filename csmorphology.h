#ifndef CSMORPHOLOGY_H
#define CSMORPHOLOGY_H
#include<opencv/cv.h>
#include<opencv/cxcore.h>
#include<opencv/highgui.h>

#define LH_MORP_TYPE_BINARY            0
#define LH_MORP_TYPE_UNCONSTRAIN	1
#define LH_MORP_TYPE_CONSTRAIN		2
class CSMorphology
{
public:
    CSMorphology();
    //functions
   IplConvKernel* csStructuringElementCopy(IplConvKernel* se);
   IplConvKernel* csStructuringElementNot(IplConvKernel* se);
   int csStructuringElementCard(IplConvKernel* se);
   IplConvKernel* csStructuringElementMap(IplConvKernel* se);
   IplConvKernel* csStructuringElementLine(int angle, int len);
   void csMorpOpen( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpClose( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpGradient( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpGradientIn( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpGradientOut( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpGradientDir( IplImage* src, IplImage* dst, int angle, int len );
   void csMorpWhiteTopHat( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpBlackTopHat( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpQTopHat( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpEnhance( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpHMTB( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg );
   void csMorpHMTU( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg );
   void csMorpHMTC( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg );
   void csMorpHMT( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg , int type=LH_MORP_TYPE_BINARY);
   void csMorpHMTOpen( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg , int type=LH_MORP_TYPE_BINARY);
   void csMorpThin( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg , int type=LH_MORP_TYPE_BINARY);
   void csMorpThinFit( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg , int type=LH_MORP_TYPE_BINARY);
   void csMorpThick( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg , int type=LH_MORP_TYPE_BINARY);
   void csMorpThickMiss( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg , int type=LH_MORP_TYPE_BINARY);
   int csImageCmp( IplImage* img1,  IplImage* img2);
   void csMorpRDilate( IplImage* src,  IplImage* msk, IplImage* dst, IplConvKernel* se=NULL, int iterations=-1);
   void csMorpRErode( IplImage* src,   IplImage* msk, IplImage* dst, IplConvKernel* se=NULL, int iterations=-1);
   void csMorpROpen( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpRClose( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpRWTH( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpRBTH( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations=1);
   void csMorpRSelfDual( IplImage* src,  IplImage* msk, IplImage* dst, IplConvKernel* se, int iterations=-1);
   void csMorpRMin( IplImage* src, IplImage* dst, IplConvKernel* se);
   void csMorpRMax( IplImage* src, IplImage* dst, IplConvKernel* se);
   void csMorpHMax( IplImage* src, IplImage* dst,  char h, IplConvKernel* se);
   void csMorpHMin( IplImage* src, IplImage* dst,  char h, IplConvKernel* se);
   void csMorpHConcave( IplImage* src, IplImage* dst,  char h, IplConvKernel* se);
   void csMorpHConvex( IplImage* src, IplImage* dst,  char h, IplConvKernel* se);
   void csMorpEMax( IplImage* src, IplImage* dst,  char h, IplConvKernel* se);
   void csMorpEMin( IplImage* src, IplImage* dst,  char h, IplConvKernel* se);
   void csMorpRankFilterB( IplImage* src, IplImage* dst, IplConvKernel* se, int rank = 0);
   void csMorpRemoveBoderObj( IplImage* src, IplImage* dst);
   void csMorpFillHole( IplImage* src, IplImage* dst);
};

#endif // CSMORPHOLOGY_H
