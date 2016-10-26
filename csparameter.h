#ifndef CSPARAMETER_H
#define CSPARAMETER_H

#include<math.h>
class CSParameter
{
public:
    CSParameter(double wr,double cr);

    //math static
    double PI;
    double WELLR;
    double CELLR;
    double WELLR_AREA;
    //******************computting parameter(used)******************//
    //roi(well+cell)

   // threshold roi well cell lathreshold
    double THRESHOLD_ROI_WELL_CELL_LATHRESHOLD;

};

#endif // CSPARAMETER_H
