#include "csmorphology.h"


CSMorphology::CSMorphology()
{
}
IplConvKernel* CSMorphology::csStructuringElementCopy(IplConvKernel* se)
{

        IplConvKernel* copy = cvCreateStructuringElementEx( se->nCols, se->nRows,
                        se->anchorX, se->anchorY, 0, NULL );

        copy->nShiftR = se->nShiftR;

        memcpy( copy->values, se->values, sizeof(int) * se->nRows * se->nCols );

        return copy;
}

IplConvKernel* CSMorphology::csStructuringElementNot(IplConvKernel* se)
{

        IplConvKernel* temp = cvCreateStructuringElementEx( se->nCols, se->nRows,
                        se->anchorX, se->anchorY, 0, NULL );

        temp->nShiftR = se->nShiftR;

        memcpy( temp->values, se->values, sizeof(int) * se->nRows * se->nCols );

        for(int i=0; i<temp->nRows * temp->nCols ; i++)
        {
                temp->values[i] = !temp->values[i] ;
        }

        return temp;
}
int CSMorphology::csStructuringElementCard(IplConvKernel* se)
{
        assert(se != NULL);
        int i, cnt = 0;

        for (i=0; i<se->nCols*se->nRows; i++)
        {
                cnt += se->values[i];
        }
        return cnt;

}

IplConvKernel* CSMorphology::csStructuringElementMap(IplConvKernel* se)
{
        CvMat *mat = cvCreateMat( se->nRows,  se->nCols, CV_32SC1);
        memcpy(mat->data.i, se->values, sizeof(int) * se->nRows * se->nCols );
        cvFlip(mat, NULL, -1);

        IplConvKernel* semap = cvCreateStructuringElementEx( se->nCols, se->nRows,
                        se->nCols-se->anchorX-1, se->nRows-se->anchorY-1, 0, NULL );

        semap->nShiftR = se->nShiftR;

        memcpy( semap->values, mat->data.i, sizeof(int) * se->nRows * se->nCols );

        cvReleaseMat(&mat);

        return semap;
}

IplConvKernel* CSMorphology::csStructuringElementLine(int angle, int len)
{
        assert(len > 2);
        angle = angle%180;

        CvPoint p1 = cvPoint(0,0);
        CvPoint p2 = cvPoint(0,0);
        int width = cvRound(len*cos((float)angle*CV_PI/180.0));
        int height = cvRound(len*sin((float)angle*CV_PI/180.0));

        height = height >= 1 ? height : 1;

        if (width < 0)
        {
                width = width <= -1 ? width : -1;
                p1.x = 0;
                p1.y = 0;
                p2.x = -width -1;
                p2.y = height -1;
        }
        else
        {
                width = width >= 1 ? width : 1;
                p1.x = 0;
                p1.y = height -1;
                p2.x = width -1;
                p2.y = 0;
        }
        CvMat *temp = cvCreateMat(height, width, CV_32SC1);
        cvZero(temp);
        cvLine(temp, p1, p2, cvScalar(1,1,1), 1, 4, 0);

        IplConvKernel* se = cvCreateStructuringElementEx( width, height,
                        (width-1)/2, (height-1)/2,  CV_SHAPE_CUSTOM, temp->data.i );

        cvReleaseMat(&temp);
        return se;

}


void CSMorphology::csMorpOpen(IplImage *src, IplImage *dst, IplConvKernel *se, int iterations)
{

    cvErode( src, dst, se, iterations );

        IplConvKernel* semap = csStructuringElementMap(se);

    cvDilate( dst, dst, semap, iterations );

        cvReleaseStructuringElement(&semap);

}

void CSMorphology::csMorpClose(IplImage *src, IplImage *dst, IplConvKernel *se, int iterations)
{

    cvDilate( src, dst, se, iterations );

        IplConvKernel* semap = csStructuringElementMap(se);

    cvErode( dst, dst, semap, iterations );

        cvReleaseStructuringElement(&semap);

}

void CSMorphology::csMorpGradient(IplImage *src, IplImage *dst, IplConvKernel *se, int iterations)
{
        assert(src != NULL && dst != NULL && src != dst);
        IplImage*  temp = cvCloneImage(src);
        cvErode( src, temp, se, iterations );
    cvDilate( src, dst, se, iterations );
    cvSub( dst, temp, dst );
        cvReleaseImage(&temp);
}

void CSMorphology::csMorpGradientIn( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL && dst != NULL && src != dst);
        cvErode( src, dst, se, iterations );
    cvSub( src, dst, dst );
}


void CSMorphology::csMorpGradientOut( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL && dst != NULL && src != dst);
        cvDilate( src, dst, se, iterations );
    cvSub(dst, src, dst );
}

void CSMorphology::csMorpGradientDir( IplImage* src, IplImage* dst, int angle, int len )
{
        assert(src != NULL && dst != NULL && src != dst);
        IplConvKernel* se = csStructuringElementLine(angle, len);
        csMorpGradient(src, dst, se);
        cvReleaseStructuringElement(&se);
}

void CSMorphology::csMorpWhiteTopHat( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL && dst != NULL && src != dst);
        csMorpOpen(src, dst, se, iterations );
    cvSub( src, dst, dst );
}


void CSMorphology::csMorpBlackTopHat( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL && dst != NULL && src != dst);
        csMorpClose(src, dst, se, iterations );
    cvSub(dst, src, dst );

}

void CSMorphology::csMorpQTopHat( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL && dst != NULL && src != dst);
        IplImage*  temp = cvCloneImage(src);
    csMorpClose( src, temp, se, iterations );
    csMorpOpen( src, dst, se, iterations );
    cvSub(temp, dst, dst );
        cvReleaseImage(&temp);
}

void CSMorphology::csMorpEnhance( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL && dst != NULL && src != dst);

            IplImage*  temp = cvCloneImage(src);
        csMorpWhiteTopHat( src, temp, se, iterations );
        csMorpBlackTopHat( src, dst, se, iterations );
            cvAdd(src, temp, temp);
        cvSub(temp, dst, dst );
            cvReleaseImage(&temp);

}

void CSMorphology::csMorpHMTB( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg)
{
        assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

        if (sebg == NULL)
        {
                sebg = csStructuringElementNot(sefg);

        }
        IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
        IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);

        //P104 (5.2)
        cvErode( src, temp1, sefg);
        cvNot(src, temp2);
        cvErode( temp2, temp2, sebg);
        cvAnd(temp1, temp2, dst);


        cvReleaseImage(&temp1);
        cvReleaseImage(&temp2);

        cvReleaseStructuringElement(&sebg);

}


void CSMorphology::csMorpHMTU( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg)
{
        assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

        if (sebg == NULL)
        {
                sebg = csStructuringElementNot(sefg);

        }

        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);
        IplImage*  mask = cvCreateImage(cvGetSize(src), 8, 1);
        cvZero(mask);

        //P106 (5.4)
        cvErode( src, temp, sefg);
        cvDilate(src, dst, sebg);
        cvCmp(temp, dst, mask, CV_CMP_GT);

        cvSub(temp, dst, dst, mask);
        cvNot(mask, mask);
        cvSet(dst, cvScalar(0), mask);

        //cvCopy(dst, mask);
        //cvSet(dst, cvScalar(255), mask);
        cvReleaseImage(&mask);
        cvReleaseImage(&temp);

        cvReleaseStructuringElement(&sebg);
}

void CSMorphology::csMorpHMTC( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg)
{
        assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

        if (sebg == NULL)
        {
                sebg = csStructuringElementNot(sefg);

        }

        IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
        IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);
        IplImage*  temp3 = cvCreateImage(cvGetSize(src), 8, 1);
        IplImage*  temp4 = cvCreateImage(cvGetSize(src), 8, 1);

        IplImage*  mask1 = cvCreateImage(cvGetSize(src), 8, 1);
        IplImage*  mask2 = cvCreateImage(cvGetSize(src), 8, 1);
        IplImage*  mask3 = cvCreateImage(cvGetSize(src), 8, 1);
        IplImage*  mask4 = cvCreateImage(cvGetSize(src), 8, 1);

        cvZero(mask1);
        cvZero(mask2);
        cvZero(mask3);
        cvZero(mask4);

        cvZero(dst);

        //P107 (5.5)
        cvErode( src, temp1, sebg);
        cvDilate( src, temp2, sebg);
        cvErode( src, temp3, sefg);
        cvDilate( src, temp4, sefg);

        cvCmp(src, temp3, mask1, CV_CMP_EQ);
        cvCmp(temp2, src,  mask2, CV_CMP_LT);
        cvAnd(mask1, mask2, mask2);

        cvCmp(src, temp4, mask3 , CV_CMP_EQ);
        cvCmp(temp1, src, mask4 , CV_CMP_GT);
        cvAnd(mask3, mask4, mask4);

        cvSub(src, temp2, dst, mask2);
        cvSub(temp1, src, dst, mask4);




        cvReleaseImage(&mask1);
        cvReleaseImage(&mask2);
        cvReleaseImage(&mask3);
        cvReleaseImage(&mask4);

        cvReleaseImage(&temp1);
        cvReleaseImage(&temp2);
        cvReleaseImage(&temp3);
        cvReleaseImage(&temp4);

        cvReleaseStructuringElement(&sebg);

}



void CSMorphology::csMorpHMT( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{
        switch(type)
        {
        case LH_MORP_TYPE_BINARY:
                {
                        csMorpHMTB(src, dst, sefg, sebg);
                        break;
                }

        case LH_MORP_TYPE_UNCONSTRAIN:
                {
                        csMorpHMTU(src, dst, sefg, sebg);
                        break;
                }

        case LH_MORP_TYPE_CONSTRAIN:
                {
                        csMorpHMTC(src, dst, sefg, sebg);
                        break;
                }

        default:
                break;

        }

}

void CSMorphology::csMorpHMTOpen( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{
        assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

        IplConvKernel* semap = csStructuringElementMap(sefg);

        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

        //P110 (5.8)
        csMorpHMT(src, temp, sefg, sebg, type);
        cvDilate(temp, dst, semap);

        cvReleaseImage(&temp);
        cvReleaseStructuringElement(&semap);

}

void CSMorphology::csMorpThin( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

        assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);


        csMorpHMT(src, dst, sefg, NULL, type);
        cvSub(src, dst, dst);

}

void CSMorphology::csMorpThinFit( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

        assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

        csMorpHMTOpen(src, dst, sefg, NULL, type);
        cvSub(src, dst, dst);
}

void CSMorphology::csMorpThick( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

        assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);


        csMorpHMT(src, dst, sefg, NULL, type);
        cvAdd(src, dst, dst);

}

void CSMorphology::csMorpThickMiss( IplImage* src, IplImage* dst, IplConvKernel* sefg, IplConvKernel* sebg, int type)
{

        assert(src != NULL && dst != NULL && src != dst && sefg!= NULL && sefg!=sebg);

        csMorpHMTOpen(src, dst, sefg, NULL, type);
        cvAdd(src, dst, dst);
}


int  CSMorphology::csImageCmp( IplImage* img1,  IplImage* img2)
{
        assert(img1->width == img2->width && img1->height == img2->height && img1->imageSize == img2->imageSize );
        return memcmp(img1->imageData, img2->imageData, img1->imageSize);
}

void CSMorphology::csMorpRDilate( IplImage* src,  IplImage* msk, IplImage* dst, IplConvKernel* se, int iterations)
{

        assert(src != NULL && msk != NULL && dst != NULL && src != dst );

        if(iterations < 0)
        {
                cvMin(src, msk, dst);
                cvDilate(dst, dst, se);
                cvMin(dst, msk, dst);

                IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
                //IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);

                do
                {
                        //record last result
                        cvCopy(dst, temp1);
                        cvDilate(dst, dst, se);
                        cvMin(dst, msk, dst);
                        //cvCmp(temp1, dst, temp2, CV_CMP_NE );

                }
                //while(cvSum(temp2).val[0] != 0);
                while(csImageCmp(temp1, dst)!= 0);

                cvReleaseImage(&temp1);
                //cvReleaseImage(&temp2);

                return;

        }
        else if (iterations == 0)
        {
                cvCopy(src, dst);
        }
        else
        {

                cvMin(src, msk, dst);
                cvDilate(dst, dst, se);
                cvMin(dst, msk, dst);

                for(int i=1; i<iterations; i++)
                {
                        cvDilate(dst, dst, se);
                        cvMin(dst, msk, dst);

                }

        }
}

void CSMorphology::csMorpRErode( IplImage* src,   IplImage* msk, IplImage* dst, IplConvKernel* se, int iterations)
{

        assert(src != NULL  && msk != NULL && dst != NULL && src != dst );

        if(iterations < 0)
        {
                //腐蚀重建
                cvMax(src, msk, dst);
                cvErode(dst, dst, se);
                cvMax(dst, msk, dst);

                IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
                //IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);

                do
                {
                        //record last result
                        cvCopy(dst, temp1);
                        cvErode(dst, dst, se);
                        cvMax(dst, msk, dst);
                        //cvCmp(temp1, dst, temp2, CV_CMP_NE);

                }
                //while(cvSum(temp2).val[0] != 0);
                while(csImageCmp(temp1, dst)!= 0);

                cvReleaseImage(&temp1);
                //cvReleaseImage(&temp2);

                return;

        }
        else if (iterations == 0)
        {
                cvCopy(src, dst);
        }
        else
        {
                //普通测地腐蚀 p137(6.2)
                cvMax(src, msk, dst);
                cvErode(dst, dst, se);
                cvMax(dst, msk, dst);

                for(int i=1; i<iterations; i++)
                {
                        cvErode(dst, dst, se);
                        cvMax(dst, msk, dst);
                }
        }
}

void CSMorphology::csMorpROpen( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL  && dst != NULL && src != dst );

        //p155(6.16)
        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);
        cvErode(src, temp, se, iterations);
        csMorpRDilate(temp, src, dst, se, -1);
        cvReleaseImage(&temp);

}

void CSMorphology::csMorpRClose( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL  && dst != NULL && src != dst );

        //p155(6.17)
        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);
        cvDilate(src, temp, se, iterations);
        csMorpRErode(temp, src, dst, se, -1);
        cvReleaseImage(&temp);

}

void CSMorphology::csMorpRWTH( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL  && dst != NULL && src != dst );
        //p156
        csMorpROpen(src, dst, se, iterations);
        cvSub(src, dst, dst);
}

void CSMorphology::csMorpRBTH( IplImage* src, IplImage* dst, IplConvKernel* se, int iterations)
{
        assert(src != NULL  && dst != NULL && src != dst );
        //p156
        csMorpRClose(src, dst, se, iterations);
        cvSub(dst, src, dst);
}



void CSMorphology::csMorpRSelfDual(IplImage *src, IplImage *msk, IplImage *dst, IplConvKernel *se, int iterations)
{
        assert(src != NULL  && msk != NULL && dst != NULL && src != dst );

        //p140(6.7) p142 (6.10)
        IplImage*  temp1 = cvCreateImage(cvGetSize(src), 8, 1);
        IplImage*  temp2 = cvCreateImage(cvGetSize(src), 8, 1);

        cvZero(temp2);

        csMorpRDilate(src, msk, temp1, se, iterations);

        csMorpRErode(src, msk, dst, se, iterations);

        cvCmp(src, msk, temp2, CV_CMP_LE);

        cvCopy(temp1, dst, temp2);

        cvReleaseImage(&temp1);
        cvReleaseImage(&temp2);
}



void CSMorphology::csMorpRMin( IplImage* src, IplImage* dst, IplConvKernel* se)
{
        assert(src != NULL  &&  dst != NULL && src != dst );

        //p149 (6.14)
        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

        cvAddS(src, cvScalar(1), temp);

        csMorpRErode(temp, src, dst, se);

        cvSub(dst, src, dst);

        cvReleaseImage(&temp);

}

void CSMorphology::csMorpRMax( IplImage* src, IplImage* dst, IplConvKernel* se)
{
        assert(src != NULL  &&  dst != NULL && src != dst );

        //p149 (6.13)
        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

        cvSubS(src, cvScalar(1), temp);

        csMorpRDilate(temp, src, dst, se);

        cvSub(src, dst, dst);

        cvReleaseImage(&temp);

}


void CSMorphology::csMorpHMax( IplImage* src, IplImage* dst,  char h, IplConvKernel* se)
{
        assert(src != NULL  &&  dst != NULL && src != dst );

        //p150
        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

        cvSubS(src, cvScalar(h), temp);

        csMorpRDilate(temp, src, dst, se);

        cvReleaseImage(&temp);

}


void CSMorphology::csMorpHMin( IplImage* src, IplImage* dst,  char h, IplConvKernel* se)
{
        assert(src != NULL  &&  dst != NULL && src != dst );

        //p150
        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

        cvAddS(src, cvScalar(h), temp);

        csMorpRErode(temp, src, dst, se);

        cvReleaseImage(&temp);

}

void CSMorphology::csMorpHConcave( IplImage* src, IplImage* dst,  char h, IplConvKernel* se)
{
        assert(src != NULL  &&  dst != NULL && src != dst );

        //p150

        csMorpHMin(src, dst, h, se);
        cvSub(dst, src, dst);

}

void CSMorphology::csMorpHConvex( IplImage* src, IplImage* dst,  char h, IplConvKernel* se)
{
        assert(src != NULL  &&  dst != NULL && src != dst );

        //p150

        csMorpHMax(src, dst, h, se);
        cvSub(src, dst, dst);

}

void CSMorphology::csMorpEMax( IplImage* src, IplImage* dst,  char h, IplConvKernel* se)
{
        assert(src != NULL  &&  dst != NULL && src != dst );

        //p150
        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

        csMorpHMax(src, temp, h, se);
        csMorpRMax(temp, dst, se);

        cvReleaseImage(&temp);

}

void CSMorphology::csMorpEMin( IplImage* src, IplImage* dst,  char h, IplConvKernel* se)
{
        assert(src != NULL  &&  dst != NULL && src != dst );

        //p150
        IplImage*  temp = cvCreateImage(cvGetSize(src), 8, 1);

        csMorpHMin(src, temp, h, se);
        csMorpRMin(temp, dst, se);

        cvReleaseImage(&temp);

}


void CSMorphology::csMorpRankFilterB(IplImage *src, IplImage *dst, IplConvKernel *se, int rank)
{
        assert(src != NULL  &&  dst != NULL && src != dst );

        bool defaultse = false;
        int card;
        if (se == NULL)
        {
                card = 3*3;
                assert(rank >= 0 && rank <= card);
                se = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT);
                defaultse = true;
        }
        else
        {
                card = csStructuringElementCard(se);
                assert(rank >= 0 && rank <= card);
        }

        //default rank is median
        if (rank == 0)
                rank = card/2+1;

        IplConvKernel* semap =	csStructuringElementMap(se);

        CvMat *semat = cvCreateMat(semap->nRows, semap->nCols, CV_32FC1);

        int i;
        for (i=0; i<semap->nRows*semap->nCols; i++)
        {
                semat->data.fl[i] = semap->values[i];
        }

        cvThreshold(src, dst, 0, 1, CV_THRESH_BINARY);
        IplImage *temp = cvCreateImage(cvGetSize(dst), 8, 1);

        cvFilter2D(dst, temp, semat, cvPoint(semap->anchorX, semap->anchorY));

        cvThreshold(temp, dst, card-rank, 255, CV_THRESH_BINARY);

        cvReleaseMat(&semat);
        cvReleaseStructuringElement(&semap);

        if (defaultse)
                cvReleaseStructuringElement(&se);

        cvReleaseImage(&temp);

}

void CSMorphology::csMorpRemoveBoderObj( IplImage* src, IplImage* dst)
{
        IplImage *temp = cvCloneImage(src);
        //double min, max;
        //cvMinMaxLoc(src, &min, &max);

        //
        cvRectangle(temp, cvPoint(3,3), cvPoint(temp->width-7, temp->height-7), CV_RGB(0,0,0), -1);

        //
        csMorpRDilate(temp, src, dst);

        cvReleaseImage(&temp);
        //cvSet((CvArr*)src, cvScalar(min), dst);
        //cvCopy(src, dst);
        cvSub(src, dst, dst);
}


void CSMorphology::csMorpFillHole( IplImage* src, IplImage* dst)
{
        IplImage *temp = cvCloneImage(src);
        double min, max;
        cvMinMaxLoc(src, &min, &max);
        //
        cvRectangle(temp, cvPoint(3,3), cvPoint(temp->width-7, temp->height-7), CV_RGB(max,max,max), -1);

        csMorpRErode(temp, src, dst);

        cvReleaseImage(&temp);
        //cvSub(src, dst, dst);
}

