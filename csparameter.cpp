#include "csparameter.h"

CSParameter::CSParameter(double wr,double cr)
{

     PI=3.1415926;
     WELLR=wr;
     CELLR=cr;

     WELLR_AREA=PI*WELLR*WELLR;
     // threshold roi well cell lathreshold
     THRESHOLD_ROI_WELL_CELL_LATHRESHOLD=(WELLR/2+WELLR)*WELLR;

}

