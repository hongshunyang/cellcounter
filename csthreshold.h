#ifndef CSTHRESHOLD_H
#define CSTHRESHOLD_H
#include<opencv/cv.h>
#include<opencv/cxcore.h>
#include<opencv/highgui.h>
class CSThreshold
{
public:
    CSThreshold();
    //OTSU global thresholding routine
    int csOstu(IplImage *threshlodsrc);
    //Maximum entropy thresholding routine
    int  csMaxEntropy(IplImage *src);


private:
    typedef enum {back,object} entropy_state;
    double caculateCurrentEntropy(CvHistogram * Histogram1, int cur_threshold, entropy_state state);

};

#endif // CSTHRESHOLD_H
