
#include"csview.h"
#include<QGraphicsScene>
#include<QLinearGradient>
#include<QPixmap>
#include<QMessageBox>
CSView::CSView(QWidget *parent,QSettings *ccset,QString imgcountcurr,QLabel *reportlbl, QPixmap bg, int scaleMode): QGraphicsView(parent)
{
    ccsettings=ccset;
    report=reportlbl;
    background=bg;
    ccscene= new QGraphicsScene(this);
    ccscene->setItemIndexMethod(QGraphicsScene::NoIndex);
    ccscene->setSceneRect(0,0,this->background.width(),this->background.height());
    setScene(ccscene);
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    zoom_init(scaleMode);
    //init
    scaleNum=0;

    imgCountCurr=imgcountcurr;
    allPoint.clear();
    autoPointList.clear();
    manPoint.clear();
}
CSView::~CSView()
{
     allPoint.clear();
     autoPointList.clear();
     manPoint.clear();
}

void CSView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);
    painter->drawPixmap(0,0,this->background);

}
void CSView::mousePressEvent(QMouseEvent *event)
{
    QPointF cellp;
    cellp=this->mapToScene(event->pos());
    update();

    //settings cellcounter.ini
    int ManMode;
    ccsettings->beginGroup("CellCounter");
    ManMode=ccsettings->value("ManMode","0").toInt();
    ccsettings->endGroup();
    if(ManMode==1) {
            this->setCursor(Qt::ArrowCursor);
            if(event->button()==Qt::LeftButton)
            {
                //add cell
                if (!qgraphicsitem_cast<CSNode *>(ccscene->itemAt(cellp.rx(),cellp.ry(),QTransform())))
                {
                    CSNode *cell = new CSNode(this,ccsettings,1);
                    ccscene->addItem(cell);
                    cell->setPos(cellp.rx(),cellp.ry());
                    allPoint.append(cellp);
                    manPoint.append(cellp);
                }
            }else if(event->button()==Qt::RightButton)
            {
                //delete cell
                if (qgraphicsitem_cast<CSNode *>(ccscene->itemAt(cellp.rx(),cellp.ry(),QTransform())))
                {
                    QGraphicsItem *cell = ccscene->itemAt(cellp.rx(),cellp.ry(),QTransform());
                    ccscene->removeItem(cell);
                    int i=allPoint.indexOf(cell->pos());
                    if(i!=-1)
                        allPoint.takeAt(i);
                }
            }

            report->setText(QString::number(allPoint.count()));
    } else {
        this->setCursor(Qt::OpenHandCursor);
    }
}
void CSView::draw_point(QPointF cp,int autoOrman)
{
    if (!qgraphicsitem_cast<CSNode *>(ccscene->itemAt(cp.rx(),cp.ry(),QTransform())))
    {
        CSNode *cell = new CSNode(this,ccsettings,autoOrman);
        ccscene->addItem(cell);
        cell->setPos(cp.rx(),cp.ry());
    }
}
void CSView::zoom_in()
{
    if(!this->background.isNull())
    {
        this->scale(1.25,1.25);
        scaleNum+=1;
    }
}
void CSView::zoom_out()
{
    if(!this->background.isNull())
    {
        this->scale(1/1.25,1/1.25);
        scaleNum-=1;

    }
}
void CSView::zoom_normal()
{
    if(!this->background.isNull())
    {
        if(scaleNum<0)
            for(int i=0;i<abs(scaleNum);i++)
            {
                this->scale(1.25,1.25);
            }
        else
           for(int i=0;i<scaleNum;i++)
           {
               this->scale(1/1.25,1/1.25);
           }
        scaleNum=0;
    }
}
void CSView::zoom_init(int sm)
{
  if(!this->background.isNull())
   {
    if(sm<0)
        for(int i=0;i<abs(sm);i++)
        {
            this->scale(1/1.25,1/1.25);
        }
    else
        for(int i=0;i<sm;i++)
       {
            this->scale(1.25,1.25);
       }
  }
}
void CSView::init()
{
   // imgCountCurr=imgcountcurr;
    allPoint.clear();
    for(int i=0;i<this->autoPointList.count();i++)
    {
        //V2 added
        //if manPoint is not null ,and current point in manPointList then draw_point(xx,1)
        if (this->manPoint.count()>0 && this->manPoint.contains(this->autoPointList.at(i)) )
        {
            draw_point(this->autoPointList.at(i),1);//manual 1

        } else {
        draw_point(this->autoPointList.at(i),0);//automatic 0
        }
        allPoint.append(this->autoPointList.at(i));

    }
    report->setText(QString::number(this->allPoint.count()));
}
