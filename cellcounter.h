#ifndef CELLCOUNTER_H
#define CELLCOUNTER_H
#include"csopencv.h"
#include <QMainWindow>
#include <QString>
#include <QListWidget>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QStandardItemModel>
#include <QSettings>
#include <QProgressBar>
#include <QGraphicsItem>
#include <QPointF>
#include <QMouseEvent>
#include <QPainter>
#include<QVBoxLayout>
#include<csview.h>
#include<QMultiMap>
#include<QMap>
#include<xccelhandler.h>
#include<xccelgenerator.h>
namespace Ui {
    class CellCounter;
}
class CellCounter : public QMainWindow
{
    Q_OBJECT

public:
     CellCounter(QWidget *parent = 0);
    ~CellCounter();

    //image source directory
    QString imgSrcDir;
    //current image
    QString imgCurr;
    QPixmap currPX;

    //appliaction image queue
    QStringList imgQueue;


    //available Formats Byte Array List
    QList<QByteArray> imgFormats;
    QString openFormats;
    QString saveFormats;

    QStandardItemModel *resModel;


    QSettings *ccsettings;
    //cell count class base opencv
    CSOpencv *cscv;

    double cell_maxR;
    int magic_Threshold;

    //man mode

private slots:

    void on_imgQueueList_itemClicked(QListWidgetItem* item);

    void on_ldBtn_clicked();

    void on_inBtn_clicked();

    void on_outBtn_clicked();

    void on_bchBtn_clicked();

    void on_exptBtn_clicked();

    void on_actionOnline_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_actionOpen_directory_triggered();

    void on_actionOpen_file_triggered();

    void on_maxSpin_valueChanged(double arg1);

    void on_minSpin_valueChanged(double arg1);

    void on_stepSpin_valueChanged(double arg1);

    void on_riadusSpin_valueChanged(double arg1);

    void on_countBtn_clicked();

    void on_orgBtn_clicked();

    void on_savBtn_clicked();

    void on_clrBtn_clicked();

    void on_nodeSpin_valueChanged(double arg1);

    void on_ld2Btn_clicked();

    void on_csResView_clicked(const QModelIndex &index);

    void on_contactbtn_clicked();

    void on_exptporBtn_clicked();

    void on_ldproBtn_clicked();

    void on_handBtn_toggled(bool checked);

    void on_manclrBtn_clicked();

private:
    Ui::CellCounter *ui;

    CSParameter *csp;
    int ccmode; //scale ratio

    CSView *ccview;
    QVBoxLayout *cclayout;

    XccelHandler *cchandler;

    //aspect ratio
    int max_comm_divisor(int x,int y);
    //initialization
    //init configuration
    void init_setting();
    //init gui
    void init_windows();
    //set tooltip
    void set_tooltips();
    //load image form imgSrcDir
    void load_img_srcdir();
    //load images
    void load_imgs();

    //result table
    void create_restbl(int i=0);
    //check directory
    void check_dir(QString ckdir);
    //check count ready
    void check_count();
    //count image cell
    void count_curr(int i);

    //save as project
    void save_proj();
    bool open_proj();
    //record every image point of all
    QMultiMap<QString,QPointF> imgPointMap;
    //V2 added
    QMultiMap<QString,QPointF> imgPointMapMan;

    //record the image count or not /or need recount  or form project file
    QMap<QString,int> imgCountMap;
    //result value for every image
    QMap<QString,int> imgResultMap;
    //rwell value for every image
    QMap<QString,double> imgRwellMap;
    //rcicrle value for every image
    QMap<QString,double> imgRcicrleMap;

    QColor markColor;
    //V2 added
    QColor manmarkColor;
};

#endif // CellCounter_H
