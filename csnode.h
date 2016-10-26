#include <QGraphicsView>
#include <QGraphicsItem>
#include <QList>
#include <QGraphicsSceneMouseEvent>
#include <QSettings>
class CSNode : public QGraphicsItem
{
public:
    CSNode(QGraphicsView *gview,QSettings *ccset,int autoORman);//v2 int autoORman
    ~CSNode();

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    //get settings
    QSettings *ccsettings;


    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QGraphicsView *gimgview;

    double circleRiadus;
    int mark_r;
    int mark_g;
    int mark_b;
    int circleWidth;

    //V2 added
    int manmark_r;
    int manmark_g;
    int manmark_b;

    int AutoOrMan;
};



