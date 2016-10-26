#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QColor>
#include "csnode.h"
CSNode::CSNode(QGraphicsView *gview, QSettings *ccset,int autoORman)
    : gimgview(gview)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);

    ccsettings=ccset;
    ccsettings->beginGroup("CellCounter");
    mark_r=ccsettings->value("MarkColorR","0").toInt();
    mark_g=ccsettings->value("MarkColorG","255").toInt();
    mark_b=ccsettings->value("MarkColorB","0").toInt();
    circleRiadus=ccsettings->value("Rcicrle","5").toDouble();
    circleWidth=ccsettings->value("Cwidth",2).toInt();
    //V2 added
    manmark_r=ccsettings->value("ManMarkColorR","255").toInt();
    manmark_g=ccsettings->value("ManMarkColorG","0").toInt();
    manmark_b=ccsettings->value("ManMarkColorB","0").toInt();
    ccsettings->endGroup();

     AutoOrMan = autoORman;
}
CSNode::~CSNode()
{
}

QRectF CSNode::boundingRect() const
{

    qreal adjust = 2;
    return QRectF( -circleRiadus - adjust, -circleRiadus - adjust,
                  2*circleRiadus+3 + adjust, 2*circleRiadus+3 + adjust);

}


void CSNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
   // painter->setBrush(gradient);

    painter->setRenderHint(QPainter::Antialiasing, true);
    //AutoOrMan =1 manmode =1
    //0 automatic,manmode = 0
    if(this->AutoOrMan==0) {
        painter->setPen(QPen(QColor(mark_r,mark_g,mark_b),circleWidth));
    } else if (this->AutoOrMan==1) {
        painter->setPen(QPen(QColor(manmark_r,manmark_g,manmark_b),circleWidth));
    }

    painter->drawEllipse(-circleRiadus, -circleRiadus, circleRiadus*2, circleRiadus*2);
  //  painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
}

void CSNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void CSNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

