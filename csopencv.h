#ifndef CSOPENCV_H
#define CSOPENCV_H

#include<opencv/cv.h>
#include<opencv/cxcore.h>
#include<opencv/highgui.h>
#include<QString>
#include<QImage>
#include<QLabel>
#include<QSettings>
#include<QList>
#include<QPointF>
#include"csmorphology.h"
#include"csthreshold.h"
#include"csparameter.h"
class CSOpencv
{
public:
    CSOpencv(QString csfile,QSettings *ccset);
    ~CSOpencv();

    QImage ipl2qimg(IplImage *iplimg);
    //get settings
    QSettings *ccsettings;
    //sum count;end result
    int COUNT_RESULT;

    //cell point list ;all
    QList<QPointF> cellPointList;
    //result times 1,2...n
    QList<int> cellCountList;
    //get log;all
    QList<QString> cellLogList;


    IplImage *src;

    IplImage *cellMark;

    void process_gray(IplImage *src,IplImage *dst);
    void process_equal(IplImage *src,IplImage *dst2,int type);
    void process_enhance(IplImage* src, IplImage* dst, IplConvKernel* se, int iterations);
    void process_threshold(IplImage *src,IplImage *dst,int laThreshold,int methods);

    void process_erode(const CvArr *src, CvArr *dst, IplConvKernel *element, int iterations);
    void process_dilate(const CvArr *src, CvArr *dst, IplConvKernel *element, int iterations);

    void process_close(IplImage *src, IplImage *dst, IplConvKernel *se, int iterations);
    void process_open(IplImage *src, IplImage *dst, IplConvKernel *se, int iterations);

    void process_smooth(IplImage *src,IplImage *dst,int method=1);
    void process_revert(IplImage *src,IplImage *dst);
    void process_contour(IplImage *src, IplImage *dst, IplImage *Tmp,
                         double contMinArea,double contMinR,double cellMinR,
                         int mode,int colorValue=256,int times=0,int startNum=0);
    //
    void process_process(IplImage *src,IplImage *dst,IplImage *Tmp,
                         IplConvKernel *cselememt,
                         double contMinArea,double contMinR,double cellMinR,
                         int mode,int colorValue=256);
    void process_sub(IplImage *src,IplImage *dst,int type);
    //roi
    void process_wellr();
    void process_roi(IplImage *src,IplImage *dst);
    void process_seg(IplImage *roisrc,IplImage *cellroi,IplImage *segroi);
    void process_simplecount2(IplImage *roisrc,int colorValue,int times);
    void process_simplecount1(IplImage *roisrc, int colorValue, int times);
    void process_cellcount();

private:
    int COUNT_CELL;
    int CellCOUNT;
    int threshold;

    // IplImage *dbg;
    CvFont ft;
    int mark_r;
    int mark_g;
    int mark_b;

    int start_num; //cvputtext number

    double step;
    double Rmax;
    double Rmin;
    double Rwell;

    CSMorphology *cspoly;
    CSThreshold *cshold;
     CSParameter *csparam;
    IplConvKernel *cselement;
};

#endif // CSOPENCV_H
