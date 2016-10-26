#include <QGraphicsView>
#include"csnode.h"
#include<QPainter>
#include<QRectF>
#include<QMouseEvent>
#include<QPixmap>
#include<QPointF>
#include<QList>
#include<QLabel>
#include<QSettings>
class CSView : public QGraphicsView
{
public:
    CSView(QWidget *parent,QSettings *ccset,QString imgcountcurr,QLabel *reportlbl,QPixmap bg,int scaleMode);
    ~CSView();

    void init();
    void draw_point(QPointF cp,int autoOrman);
    int scaleNum;
    void zoom_init(int sm);
    void zoom_in();
    void zoom_out();
    void zoom_normal();

    //cscv auto count
    QList<QPointF> autoPointList;
    //count cscv+man
    QList<QPointF> allPoint;
    //count manual
    //V2 added
    QList<QPointF> manPoint;
    //correct result
    QLabel *report;

    //current image
    QString imgCountCurr;

    //get settings
    QSettings *ccsettings;
    QGraphicsScene *ccscene;
    QPixmap background;
protected:
    void drawBackground(QPainter *painter, const QRectF &rect);
    void mousePressEvent(QMouseEvent *event);
private:


};
