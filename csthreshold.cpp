#include "csthreshold.h"

CSThreshold::CSThreshold()
{
}

int CSThreshold::csOstu(IplImage *thresholdsrc)
{
        int height=thresholdsrc->height;
        int width=thresholdsrc->width;

        //histogram
        float histogram[256]={0};
        for(int i=0;i<height;i++) {
                unsigned char* p=(unsigned char*)thresholdsrc->imageData+thresholdsrc->widthStep*i;
                for(int j=0;j<width;j++) {
                        histogram[*p++]++;
                }
        }
        //normalize histogram
        int size=height*width;
        for(int i=0;i<256;i++) {
                histogram[i]=histogram[i]/size;
        }

        //average pixel value
        float avgValue=0;
        for(int i=0;i<256;i++) {
                avgValue+=i*histogram[i];
        }

        int threshold=0;
        float maxVariance=0;
        float w=0,u=0;
        for(int i=0;i<256;i++) {
                w+=histogram[i];
                u+=i*histogram[i];

               float t=avgValue*w-u;

               float variance=t*t/(w*(1-w));
                if(variance>maxVariance) {
                        maxVariance=variance;
                        threshold=i;
                }
        }

        return threshold;
}
/*============================================================================
 代码内容：最大熵阈值分割
===============================================================================*/
// 计算当前位置的能量熵
double CSThreshold::caculateCurrentEntropy(CvHistogram * Histogram1, int cur_threshold, entropy_state state)
{
    int start,end;
    if(state == back)
    {
      start = 0;
      end = cur_threshold;
    }
    else
    {
      start = cur_threshold;
      end = 256;
    }
    int  total = 0;
    for(int i = start; i < end; i++)
    {
      total += (int)cvQueryHistValue_1D(Histogram1, i);
    }
    double cur_entropy = 0.0;
    for(int i = start; i < end; i++)
    {
      if((int)cvQueryHistValue_1D(Histogram1,i) == 0)
       continue;
      double percentage = cvQueryHistValue_1D(Histogram1, i) / total;
      cur_entropy += - percentage * logf(percentage); // 能量的定义公式
    }
    return cur_entropy;
}
// 寻找最大熵阈值并分割
int CSThreshold::csMaxEntropy(IplImage *src)
{

    assert(src != NULL);
    assert(src->depth == 8);
    assert(src->nChannels == 1);

    int HistogramBins = 256;
    float HistogramRange1[2] = {0,255};
    float *HistogramRange[1] = {&HistogramRange1[0]};

    CvHistogram * hist  = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY, HistogramRange);
        cvCalcHist(&src, hist);
    double maxentropy = -1.0;
    int max_index = -1;
    // 循环测试每个分割点，寻找到最大的阈值分割点
    for(int i = 0; i < HistogramBins; i++)
    {
      double cur_entropy = caculateCurrentEntropy(hist, i, object) + caculateCurrentEntropy(hist, i, back);
      if(cur_entropy > maxentropy)
      {
       maxentropy = cur_entropy;
       max_index = i;
      }
    }
    //cvThreshold(src, dst, (double)max_index, 255, CV_THRESH_BINARY);
    cvReleaseHist(&hist);
    return max_index;
}
