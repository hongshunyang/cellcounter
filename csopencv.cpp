#include "csopencv.h"
#include<QString>
#include<QByteArray>
#include<opencv/cxcore.h>
#include<opencv/cvaux.h>
#include<QMessageBox>
#include<vector>
CSOpencv::CSOpencv(QString csfile,QSettings *ccset)
{
    QByteArray csflstr = csfile.toLocal8Bit();
    const char *csflname = csflstr.data();
    //load image
    src=cvLoadImage(csflname,1);

    //init
    cellMark=cvCloneImage(src);
    cspoly= new CSMorphology();
    cshold= new CSThreshold();

    cselement=cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_ELLIPSE);

    CellCOUNT=0;
    COUNT_CELL=0;
    COUNT_RESULT=0;


    //settings cellcounter.ini
    ccsettings=ccset;
    ccsettings->beginGroup("CellCounter");
    mark_r=ccsettings->value("MarkColorR","0").toInt();
    mark_g=ccsettings->value("MarkColorG","255").toInt();
    mark_b=ccsettings->value("MarkColorB","0").toInt();
    step=ccsettings->value("Step","0.5").toDouble();
    Rmax=ccsettings->value("Max","6").toDouble();
    Rmin=ccsettings->value("Min","4").toDouble();
    Rwell=ccsettings->value("Rwell","4").toDouble();
    ccsettings->endGroup();

    //mark font
    ft=cvFont(1,1);
    cvInitFont(&ft,CV_FONT_HERSHEY_SIMPLEX,0.4,0.4,0,1,0);
    //start mark number
    start_num=0;
}
CSOpencv::~CSOpencv()
{
    cvReleaseImage(&src);
    cvReleaseImage(&cellMark);
}
void CSOpencv::process_gray(IplImage *src,IplImage *dst)
{
    cvCvtColor(src,dst,CV_RGB2GRAY);
}
void CSOpencv::process_enhance(IplImage *src, IplImage *dst, IplConvKernel *se, int iterations)
{
    cspoly->csMorpEnhance(src, dst,se,iterations);
}
void CSOpencv::process_threshold(IplImage *src,IplImage *dst,int laThreshold,int methods)
{
    int thresholdT;
    switch(methods)
    {
        case 0:
            //well+cell better than ostu
            thresholdT=cshold->csMaxEntropy(src);
            break;
        case 1:
            thresholdT=cshold->csOstu(src);
            break;
        default:
            thresholdT=cshold->csMaxEntropy(src);
            break;
    }

    this->threshold=thresholdT+laThreshold;
    cvThreshold(src,dst,this->threshold,255,CV_THRESH_BINARY);//threshold+?(magic!!)

}
void CSOpencv::process_smooth(IplImage *src, IplImage *dst,int method)
{
    IplImage *tmp;
    tmp=cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
    int smoSize;
    if(int(csparam->WELLR)%2==0)
        smoSize=csparam->WELLR-1;
     else
        smoSize=csparam->WELLR;

    switch(method)
    {
        case 1:
            cvSmooth(src,tmp,CV_MEDIAN);
            cvSmooth(tmp,dst,CV_MEDIAN);
            break;
        case 2:
            cvSmooth(src,tmp,CV_MEDIAN,smoSize);
            cvSmooth(tmp,dst,CV_MEDIAN,smoSize);
            break;
        case 3:
            cvSmooth(src,dst,CV_MEDIAN);
            break;
        case 4:
            cvSmooth(src,dst,CV_MEDIAN,smoSize);
            break;
}
    cvReleaseImage(&tmp);
}
QImage CSOpencv::ipl2qimg(IplImage *iplimg)
{  
    IplImage *RGBimage = cvCreateImage(cvSize(iplimg->width,iplimg->height), IPL_DEPTH_8U, 3);
    cvConvertImage(iplimg, RGBimage, CV_CVTIMG_SWAP_RB);
    QImage qimg((uchar *)RGBimage->imageData, RGBimage->width, RGBimage->height, RGBimage->widthStep,QImage::Format_RGB888);
    return qimg;
}
void CSOpencv::process_revert(IplImage *src,IplImage *dst)
{
    cvNot(src,dst);
}
void CSOpencv::process_dilate(const CvArr *src, CvArr *dst, IplConvKernel *element, int iterations)
{
    cvDilate(src,dst,element,iterations);
}
void CSOpencv::process_erode(const CvArr *src, CvArr *dst, IplConvKernel *element, int iterations)
{
    cvErode(src,dst,element,iterations);
}
void CSOpencv::process_close(IplImage *src, IplImage *dst, IplConvKernel *se, int iterations)
{
    cspoly->csMorpClose(src,dst,se,iterations);

}
void CSOpencv::process_open(IplImage *src, IplImage *dst, IplConvKernel *se, int iterations)
{
    cspoly->csMorpOpen(src,dst,se,iterations);
}
void CSOpencv::process_wellr()
{
    IplImage *gray;
    IplImage *gray_enhance;
    IplImage *threshimg;
    IplImage *revert;
    IplImage *smoth;
    IplImage *roi;

    CvMemStorage *stor;
    CvSeq *cont;

    CvBox2D *box;
    CvPoint *PointArray;
    CvPoint2D32f *PointArray2D32f;

    gray=cvCreateImage(cvGetSize(src),8,1);
    gray_enhance=cvCloneImage(gray);
    threshimg=cvCloneImage(gray);
    revert=cvCloneImage(gray);
    smoth=cvCloneImage(gray);

    roi=cvCloneImage(src);

     process_gray(src,gray);
     process_enhance(gray,gray_enhance,this->cselement,10);

     process_threshold(gray_enhance,threshimg,0,1);
     process_dilate(threshimg,threshimg,this->cselement,1);
     process_close(threshimg,threshimg,this->cselement,1);

    // process_smooth(threshimg,smoth);
     process_revert(threshimg,revert);

     int num;

     std::vector<double> diff((Rmax-Rmin)/step);

     int j=0;
     double wlr=Rwell;
     for(double r=Rmax;r>Rmin;r=r-step)
     {
         num=0;
         j+=1;
         stor = cvCreateMemStorage(0);
         cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),stor);
         CvContourScanner scanner = NULL;
         scanner = cvStartFindContours(revert,stor,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));

        while (cont=cvFindNextContour(scanner))
        {
             if(!cvCheckContourConvexity(cont))                                    //Filter::convexity
             {
                 int i;

                 int count= cont->total;

                 CvPoint center;
                 CvSize size;

                 PointArray = (CvPoint *)malloc(count*sizeof(CvPoint));
                 PointArray2D32f = (CvPoint2D32f*)malloc(count*sizeof(CvPoint2D32f));
                 box = (CvBox2D32f *)malloc(sizeof(CvBox2D32f));

                 cvCvtSeqToArray(cont,PointArray,CV_WHOLE_SEQ);
                 for (i=0;i<count;i++)
                 {
                     PointArray2D32f[i].x=(float)PointArray[i].x;
                     PointArray2D32f[i].y=(float)PointArray[i].y;
                 }

                 if(count>=6)
                 {
                     cvFitEllipse(PointArray2D32f, count,box);
                 }
                 center.x = cvRound(box->center.x);
                 center.y = cvRound(box->center.y);
                 size.width= cvRound(box->size.width/2);
                 size.height = cvRound(box->size.height/2);


                 if(size.width<=r && size.width >(r-1))
                     if(size.height<=1.5*r)
                     {
                        cvDrawContours(roi,cont,CV_RGB(0,100*int(r),0),CV_RGB(0,100*(r),0),0,1,8,cvPoint(0,0));
                        num+=1;
                     }
                free(PointArray);
                free(PointArray2D32f);
                free(box);

             }
        }
        diff[j-1]=num;
     }

    int p=0;
    for(int k=0;k<(Rmax-Rmin)/step;k++)
    {
        if(diff[k+1]-diff[k]>p)
           {
             p=diff[k+1]-diff[k];
            // wlr+=Rmin+step*k;//TODO
            // wlr=wlr/2;
           }
    }

     csparam = new CSParameter(wlr,0);

     cvReleaseImage(&gray);
     cvReleaseImage(&threshimg);
     cvReleaseImage(&smoth);
     cvReleaseImage(&revert);
     cvReleaseImage(&gray_enhance);
     cvReleaseImage(&roi);
     cvReleaseMemStorage(&stor);
}
void CSOpencv::process_contour(IplImage *src, IplImage *dst, IplImage *Tmp,
                              double contMinArea,double contMinR,double cellMinR,int mode,int colorValue,int times,int startNum)
{

   CvMemStorage *stor;
   CvSeq *cont;

   CvBox2D *box;
   CvPoint *PointArray;
   CvPoint2D32f *PointArray2D32f;

   CellCOUNT=0;
   startNum=this->COUNT_CELL;
   double contCurrArea=0;  //current area

   stor = cvCreateMemStorage(0);
   cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),stor);

   CvContourScanner scanner = NULL;

   scanner = cvStartFindContours(src,stor,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));

   while (cont=cvFindNextContour(scanner))
   {
        if(!cvCheckContourConvexity(cont))                                    //Filter::convexity
        {
            contCurrArea = fabs(cvContourArea( cont, CV_WHOLE_SEQ ));
            if (contCurrArea >=contMinArea)                                      //Filter::area
            {
                int i;

                int count= cont->total;
                CvPoint center;
                CvSize size;

                PointArray = (CvPoint *)malloc(count*sizeof(CvPoint));
                PointArray2D32f = (CvPoint2D32f*)malloc(count*sizeof(CvPoint2D32f));
                box = (CvBox2D32f *)malloc(sizeof(CvBox2D32f));

                cvCvtSeqToArray(cont,PointArray,CV_WHOLE_SEQ);
                for (i=0;i<count;i++)
                {
                    PointArray2D32f[i].x=(float)PointArray[i].x;
                    PointArray2D32f[i].y=(float)PointArray[i].y;
                }

                if(count>=6)
                {
                    cvFitEllipse(PointArray2D32f, count,box);
                }
                center.x = cvRound(box->center.x);
                center.y = cvRound(box->center.y);
                size.width= cvRound(box->size.width/2);
                size.height = cvRound(box->size.height/2);

                 CvBox2D minRect = cvMinAreaRect2(cont);

                    if(minRect.size.width>=contMinR && minRect.size.height>=contMinR)        //Filter::well Min R
                    {
                        if(size.width>=cellMinR && size.height>=cellMinR)                                             //Filter::cell Min R
                        {
                             if (mode==0)                                                    //Mode::well
                              {
                                   cvDrawContours(Tmp,cont,cvScalarAll(0),cvScalarAll(0),0,-1,8);
                              }
                             else if (mode==1)                                               //Mode::cell count
                             {
                                 CellCOUNT+=1;
                               /* if(CellCOUNT>1)
                                 {
                                    switch(times)
                                    {
                                    case 1:
                                     cvDrawContours(dst,cont,CV_RGB(0,colorValue,0),CV_RGB(0,colorValue,0),0,1,8,cvPoint(0,0));
                                     break;
                                    case 2:
                                     cvDrawContours(dst,cont,CV_RGB(0,0,colorValue),CV_RGB(0,0,colorValue),0,1,8,cvPoint(0,0));
                                     break;
                                    default:
                                     cvDrawContours(dst,cont,CV_RGB(colorValue,0,0),CV_RGB(colorValue,0,0),0,1,8,cvPoint(0,0));
                                    }
                                }*/
                                 //get cell point list
                                 if(times>=1)
                                 {
                                     cellPointList.append(QPointF(center.x,center.y));
                                 }
                              //   cvPutText(dst,QString::number(CellCOUNT+startNum).toLocal8Bit(),center,&ft,CV_RGB(mark_r,mark_g,mark_b));

                                if(CellCOUNT>1)
                                 {
                                    cvDrawContours(Tmp,cont,cvScalarAll(0),cvScalarAll(0),0,-1,8);
                                 }

                            }
                             else if (mode==2) {
                                    cvDrawContours(Tmp,cont,cvScalarAll(255),cvScalarAll(255),0,-1,8);
                             }
                        }
                    }

              free(PointArray);
              free(PointArray2D32f);
              free(box);
            }
       }
   }
   cvReleaseMemStorage(&stor);
}
void CSOpencv::process_sub(IplImage *src, IplImage *dst, int type)
 {
    IplImage *whiteImg;
    IplImage *blackImg;

    whiteImg = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
    blackImg = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);

    cvFloodFill(whiteImg,cvPoint(0,0),cvScalarAll(255));
    cvFloodFill(blackImg,cvPoint(0,0),cvScalarAll(0));
    switch(type)
    {
        case 0:
            cvSub(src,blackImg,dst);
            break;
        case 1:
            cvSub(whiteImg,src,dst);
            break;
        case 2:
            cvSub(src,whiteImg,dst);
            break;
        case 3:
            cvSub(blackImg,src,dst);
            break;
    }
    cvReleaseImage(&blackImg);
    cvReleaseImage(&whiteImg);
}
void CSOpencv::process_roi(IplImage *src,IplImage *dst)
{

    IplImage *gray;
    IplImage *gray_enhance;
    IplImage *threshimg;
    IplImage *revert;
    IplImage *smoth;
    IplImage *roi;

    IplImage *contourDst;
    IplImage *contourBg;
    IplImage *contourDst_not;


    gray=cvCreateImage(cvGetSize(src),8,1);
    gray_enhance=cvCloneImage(gray);
    threshimg=cvCloneImage(gray);
    revert=cvCloneImage(gray);
    smoth=cvCloneImage(gray);

    roi=cvCloneImage(src);

    contourBg=cvCloneImage(src);
    contourDst=cvCreateImage(cvSize(src->width,src->height),src->depth,src->nChannels);
    contourDst_not=cvCloneImage(contourDst);

     process_gray(src,gray);

     process_enhance(gray,gray_enhance,this->cselement,csparam->WELLR/2);

     process_threshold(gray_enhance,threshimg,csparam->THRESHOLD_ROI_WELL_CELL_LATHRESHOLD,
                       0);

     process_dilate(threshimg,threshimg,this->cselement,1);
     process_close(threshimg,threshimg,this->cselement,1);

     process_smooth(threshimg,smoth);
     process_revert(smoth,revert);

     process_contour(revert,roi,contourBg,
                     0,
                     0,
                     0,0);

        //get contour=well+cell,well
     cvSub(src,contourBg,contourDst);

     process_sub(contourDst,contourDst_not,1);


     cvNot(contourDst_not,dst);


     cvReleaseImage(&contourDst_not);
     cvReleaseImage(&contourDst);
     cvReleaseImage(&contourBg);
     cvReleaseImage(&gray);
     cvReleaseImage(&threshimg);
     cvReleaseImage(&smoth);
     cvReleaseImage(&revert);
     cvReleaseImage(&gray_enhance);
     cvReleaseImage(&roi);
}
void CSOpencv::process_seg(IplImage *roisrc,IplImage *cellroi,IplImage *segroi)
 {
    IplImage *gray;
    IplImage *grayEnhance;
    IplImage *threshimg;
    IplImage *revert;
    IplImage *smoth;

    IplImage *cellroi_tmp;
    IplImage *cellroi_dst;

    IplImage *contourBg;
    IplImage *contourSeg;
    IplImage *contourDst;
    IplImage *contourDst_not;


    IplImage *segroi_threshimg;
    IplImage *segroi_threshimg2;
    IplImage *segroi_dst;
    IplImage *segroi_revert;


    gray=cvCreateImage(cvGetSize(this->src),8,1);
    grayEnhance=cvCloneImage(gray);
    threshimg=cvCloneImage(gray);
    revert=cvCloneImage(threshimg);
    smoth=cvCloneImage(gray);

    cellroi_tmp=cvCloneImage(this->src);
    cellroi_dst=cvCloneImage(roisrc);

    contourBg=cvCloneImage(this->src);
    contourDst=cvCreateImage(cvSize(this->src->width,this->src->height),this->src->depth,this->src->nChannels);
    contourDst_not=cvCreateImage(cvGetSize(grayEnhance),grayEnhance->depth,grayEnhance->nChannels);

    process_process(roisrc,cellroi_tmp,contourBg,
                    this->cselement,
                    0,
                    0,
                    0,
                    1);
    cvSub(this->src,contourBg,contourDst);
    process_gray(contourDst,gray);
    process_revert(gray,revert);
    process_enhance(revert,grayEnhance,this->cselement,csparam->WELLR/2);

    contourSeg=cvCloneImage(grayEnhance);

    cvThreshold(grayEnhance,threshimg,
                this->threshold+csparam->THRESHOLD_ROI_WELL_CELL_LATHRESHOLD,
                255,CV_THRESH_BINARY);

    //get segroi ready
    segroi_threshimg=cvCloneImage(threshimg);
    segroi_threshimg2=cvCreateImage(cvGetSize(segroi_threshimg),8,1);
    segroi_dst=cvCloneImage(roisrc);
    segroi_revert=cvCreateImage(cvGetSize(segroi_threshimg),8,1);

    //cellroi
    process_revert(threshimg,revert);
    process_contour(revert,contourSeg,contourSeg,0,0,0,2,0); //

    process_revert(contourSeg,revert);
    process_contour(revert,this->src,cellroi_tmp,0,0,0,0);   ///wellr must!!!
    cvSub(this->src,cellroi_tmp,cellroi);

    //segroi
    segroi_threshimg2=cvCloneImage(segroi_threshimg);
    process_smooth(segroi_threshimg2,segroi_threshimg2,3);
    process_dilate(segroi_threshimg2,segroi_threshimg2,this->cselement,1);
    process_open(segroi_threshimg2,segroi_threshimg2,this->cselement,1);
    process_revert(segroi_threshimg2,segroi_revert);
    process_contour(segroi_revert,this->src,segroi_dst,0,0,0,0);  //csparam->CELLR_AVG
    cvSub(cellroi_dst,segroi_dst,segroi);

    cvReleaseImage(&gray);
    cvReleaseImage(&grayEnhance);
    cvReleaseImage(&threshimg);
    cvReleaseImage(&smoth);
    cvReleaseImage(&revert);
    cvReleaseImage(&contourBg);
    cvReleaseImage(&contourSeg);
    cvReleaseImage(&contourDst);
    cvReleaseImage(&contourDst_not);
    cvReleaseImage(&segroi_threshimg);
    cvReleaseImage(&segroi_threshimg2);
    cvReleaseImage(&segroi_dst);
    cvReleaseImage(&segroi_revert);
    cvReleaseImage(&cellroi_dst);
    cvReleaseImage(&cellroi_tmp);
}
void CSOpencv::process_simplecount1(IplImage *roisrc, int colorValue, int times)
{
    IplImage *gray;
    IplImage *grayEnhance;
    IplImage *threshimg;

    gray=cvCreateImage(cvGetSize(roisrc),8,1);
    threshimg=cvCreateImage(cvGetSize(roisrc),8,1);
    grayEnhance=cvCreateImage(cvGetSize(roisrc),8,1);

    process_gray(roisrc,gray);
    process_erode(gray,gray,this->cselement,1);   //delete circle line
    process_dilate(gray,gray,this->cselement,1);
    process_enhance(gray,grayEnhance,this->cselement,csparam->WELLR/2);

    cvThreshold(grayEnhance,threshimg,0,255,CV_THRESH_BINARY);

    process_contour(threshimg,this->cellMark,roisrc,
                    csparam->WELLR_AREA,csparam->WELLR,csparam->CELLR,
                    1,
                    colorValue,times); //count
    this->COUNT_CELL=this->CellCOUNT;

    cvReleaseImage(&gray);
    cvReleaseImage(&grayEnhance);
    cvReleaseImage(&threshimg);

}

void CSOpencv::process_simplecount2(IplImage *roisrc,int colorValue,int times)
{

    IplImage *threshimg;

    IplImage *gray;
    IplImage *grayEnhance;
    IplImage *smthimg;
    IplImage *revert;

    gray=cvCreateImage(cvGetSize(roisrc),8,1);

    threshimg=cvCreateImage(cvGetSize(roisrc),8,1);
    grayEnhance=cvCreateImage(cvGetSize(roisrc),8,1);
    smthimg=cvCreateImage(cvGetSize(roisrc),8,1);
    revert=cvCloneImage(smthimg);

    process_gray(roisrc,gray);
    process_enhance(gray,grayEnhance,this->cselement,csparam->WELLR/2);

    cvThreshold(grayEnhance,smthimg,0,255,CV_THRESH_BINARY);


    process_smooth(smthimg,threshimg,3);
    process_dilate(threshimg,threshimg,this->cselement,1);
    process_open(threshimg,threshimg,this->cselement,1);

    process_revert(smthimg,revert);

    process_contour(revert,this->cellMark,roisrc,
                    0,0,0,1,
                    colorValue,times); //count

    this->COUNT_CELL=this->CellCOUNT;

    cvReleaseImage(&gray);
    cvReleaseImage(&grayEnhance);
    cvReleaseImage(&smthimg);
    cvReleaseImage(&threshimg);
}


void CSOpencv::process_cellcount()
{
   IplImage *roiDst;
   IplImage *segroiDst;
   IplImage *cellroiDst;


   roiDst=cvCreateImage(cvSize(this->src->width,this->src->height),this->src->depth,this->src->nChannels);
   segroiDst=cvCloneImage(roiDst);
   cellroiDst=cvCloneImage(roiDst);

   process_wellr();

   process_roi(this->src,roiDst); //roi(well+cell)+roi(well)->roi(cell)

  // this->dbg=cvCloneImage(roiDst);

   process_seg(roiDst,cellroiDst,segroiDst);

   //first
   process_simplecount1(cellroiDst,256,1);
   this->COUNT_RESULT=this->COUNT_CELL;
   cellCountList.append(this->COUNT_CELL);

   //second
   process_simplecount2(segroiDst,256,2);
   this->COUNT_RESULT+=this->COUNT_CELL;
   cellCountList.append(this->COUNT_CELL);

   cvReleaseImage(&roiDst);
   cvReleaseImage(&segroiDst);
   cvReleaseImage(&cellroiDst);
}
void CSOpencv::process_process(IplImage *src,IplImage *dst,IplImage *Tmp,
                               IplConvKernel *cselememt,
                               double contMinArea,double contMinR,double cellMinR,
                               int mode,int colorValue)
{
    IplImage *csgray;
    IplImage *csgrayEnhance;
    IplImage *csthreshimg;
    IplImage *cssmoth;
    IplImage *csrevert;


    csgray=cvCreateImage(cvGetSize(src),8,1);
    csgrayEnhance=cvCloneImage(csgray);
    csthreshimg=cvCloneImage(csgray);
    cssmoth=cvCloneImage(csthreshimg);
    csrevert=cvCloneImage(csthreshimg);


    process_gray(src,csgray);
    process_enhance(csgray,csgrayEnhance,this->cselement,1);
    process_threshold(csgrayEnhance,csthreshimg,0,1);
    process_smooth(csthreshimg,cssmoth);
    process_erode(cssmoth,csthreshimg,cselememt,1);
    process_open(csthreshimg,csthreshimg,cselememt,1);
    process_revert(csthreshimg,csrevert);

    process_contour(csrevert,dst,Tmp,contMinArea,contMinR,cellMinR,mode,colorValue);

    cvReleaseImage(&csgray);
    cvReleaseImage(&csgrayEnhance);
    cvReleaseImage(&csthreshimg);
    cvReleaseImage(&cssmoth);
    cvReleaseImage(&csrevert);

}

void CSOpencv::process_equal(IplImage *src, IplImage *dst2, int type)
{   //type=1 gray ;type=2 color
        int i;
        IplImage* imgChannel[4] = { 0, 0, 0, 0 };
        IplImage* dst = cvCreateImage( cvGetSize( src ), IPL_DEPTH_8U, 3 );

        if( src )
        {
            for( i = 0; i < src -> nChannels; i++ )
            {
                imgChannel[i] = cvCreateImage( cvGetSize( src ), IPL_DEPTH_8U, 1 );
                //1 channel
            }
            //split channel
            cvSplit( src, imgChannel[0], imgChannel[1], imgChannel[2], imgChannel[3] );
            for( i = 0; i < dst -> nChannels; i++ )
            {
                //1 channel srt dst
                cvEqualizeHist( imgChannel[i], imgChannel[i] );
            }

            //merge 4 channel
            cvMerge( imgChannel[0], imgChannel[1], imgChannel[2], imgChannel[3], dst );
           // cvNamedWindow( "src", 1 );
           // cvShowImage( "src", src );
           // cvNamedWindow( "Equalize", 1 );
           // cvShowImage( "Equalize", dst );

            cvWaitKey(0);
            //release
            for( i = 0; i < src -> nChannels; i++ )
            {
                if( imgChannel[i] )
                {
                    cvReleaseImage( &imgChannel[i] );
                    //imgChannel[i] = 0;
                }
            }
            cvReleaseImage( &dst );
        }
}

