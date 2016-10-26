#include "cellcounter.h"
#include "ui_cellcounter.h"

#include"csopencv.h"
#include<QFileDialog>
#include<QMessageBox>
#include<QPixmap>
#include<QFileInfo>
#include<QIcon>
#include<QImageWriter>
#include<QImage>
#include<QDesktopServices>
#include<QUrl>
#include<QStandardItemModel>
#include<QPrinter>
#include<QTextDocument>
#include<QFile>
#include<QColorDialog>
#include<QColor>
#include<QProgressBar>
#include<QBuffer>
#include<QByteArray>
#include<QDateTime>
#include<QPainter>
#include<QPalette>
#include<QBrush>
#include<QRectF>
#include<QXmlSimpleReader>
#include<QXmlInputSource>
#include "xccelhandler.h"
#include "xccelgenerator.h"

CellCounter::CellCounter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CellCounter)
{
    ui->setupUi(this);
    //initialization
    init_setting();
    init_windows();
    set_tooltips();
}

CellCounter::~CellCounter()
{
    delete ui;
}

void CellCounter::init_setting()
{

    //image type
    imgFormats = QImageWriter::supportedImageFormats();
    //.jpg,jpeg,png,tiff
    QStringList ccFormats;
    ccFormats.append("jpg");
    ccFormats.append("jpeg");
    ccFormats.append("png");
    ccFormats.append("tiff");
    ccFormats.append("gif");

    //settings cellcounter.ini
//#ifdef defined(Q_WS_MAC)
    ccsettings = new QSettings(QDir::homePath()+QDir::separator()+QDir(".config").dirName()+QDir::separator()+"cellcounter.ini",QSettings::IniFormat);
//#elif defined(Q_WS_X11)
//    ccsettings = new QSettings(QDir::homePath()+QDir::separator()+QDir(".config").dirName()+QDir::separator()+"cellcounter.ini",QSettings::IniFormat);
//#elif defined(Q_WS_WIN)
//    ccsettings = new QSettings(QDir::homePath()+QDir::separator()+"cellcounter.ini",QSettings::IniFormat);
//#endif
    ccsettings->beginGroup("CellCounter");
    ccsettings->setValue("version","0.1");
    ccsettings->setValue("ManMode","0");
    ccsettings->endGroup();

    //image formats

    foreach (QByteArray availableFormatByteArry, imgFormats)
    {
       // QMessageBox::warning(this,tr("3"),availableFormatByteArry.toLower(),QMessageBox::Ok,NULL);
        if(ccFormats.contains(availableFormatByteArry))
        {
            openFormats.append("*"+availableFormatByteArry.toLower()+" ");
            saveFormats.append("*."+availableFormatByteArry.toLower()+"\n");
        }
    }

    //0:open dir 1:open files
    //init parameters
    csp = 0;
    cscv=0;
    ccmode=0;
}

void CellCounter::init_windows()
{

    //report banner
    //current image count result
    this->ui->parabox->hide();
    this->ui->reportLbl->setStyleSheet("QLabel{color:red;font-size:14px;font-weight:bolder;}");
    this->ui->reportLbl->setText(tr("no image selected"));

    cclayout = new QVBoxLayout();
    cclayout->setMargin(0);
    this->ui->cctab->setLayout(cclayout);


    //image list
    this->ui->imgQueueList->setViewMode(QListView::IconMode);
    this->ui->imgQueueList->setFlow(QListView::LeftToRight);
    this->ui->imgQueueList->setWrapping(false);
    this->ui->imgQueueList->setIconSize(QSize(48,48));
    this->ui->imgQueueList->setUniformItemSizes(true);
    this->ui->imgQueueList->setFixedHeight(70);
    this->ui->imgQueueList->setDragEnabled(false);



    //image src dir input
    this->ui->imgSrcDir->setReadOnly(true);

    //shown interface
    //init settings
    double Rstep;
    double Rmax;
    double Rmin;
    double Rwell;
    double Rcicrle;
    int Rwidth;

    ccsettings->beginGroup("CellCounter");
    Rstep=ccsettings->value("Step","0.5").toDouble();
    Rmax=ccsettings->value("Max","6").toDouble();
    Rmin=ccsettings->value("Min","4").toDouble();
    Rwell=ccsettings->value("Rwell","4").toDouble();
    Rcicrle=ccsettings->value("Rcicrle","5").toDouble();
    Rwidth=ccsettings->value("Cwidth","2").toInt();
    ccsettings->endGroup();

    this->ui->minSpin->setValue(Rmin);
    this->ui->maxSpin->setValue(Rmax);
    this->ui->stepSpin->setValue(Rstep);
    this->ui->riadusSpin->setValue(Rwell);
    this->ui->nodeSpin->setValue(Rcicrle);
    //value correct
    this->ui->stepSpin->setMaximum(this->ui->minSpin->value());
    this->ui->minSpin->setMaximum(this->ui->maxSpin->value());
    this->ui->maxSpin->setMinimum(this->ui->minSpin->value());
    this->ui->riadusSpin->setMinimum(1);//
    this->ui->riadusSpin->setMaximum(5);


    this->ui->handBtn->setChecked(false);
    //mark color
    int mark_r;
    int mark_g;
    int mark_b;

    ccsettings->beginGroup("CellCounter");
    mark_r=ccsettings->value("MarkColorR","0").toInt();
    mark_g=ccsettings->value("MarkColorG","255").toInt();
    mark_b=ccsettings->value("MarkColorB","0").toInt();
    ccsettings->endGroup();

    markColor = QColor(mark_r,mark_g,mark_b);

    this->ui->clrBtn->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(markColor.red())
                                                                       .arg(markColor.green())
                                                                       .arg(markColor.blue()));

    //V2 added
    int manmark_r;
    int manmark_g;
    int manmark_b;

    ccsettings->beginGroup("CellCounter");
    manmark_r=ccsettings->value("ManMarkColorR","255").toInt();
    manmark_g=ccsettings->value("ManMarkColorG","0").toInt();
    manmark_b=ccsettings->value("ManMarkColorB","0").toInt();
    ccsettings->endGroup();

    manmarkColor = QColor(manmark_r,manmark_g,manmark_b);

    this->ui->manclrBtn->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(manmarkColor.red())
                                                                       .arg(manmarkColor.green())
                                                                       .arg(manmarkColor.blue()));


}
void CellCounter::set_tooltips()
{
    //user interface tooltips
    this->ui->imgSrcDir->setToolTip(tr("Images Directory"));
    this->ui->ldBtn->setToolTip(tr("Choose  images directory  to load"));
    this->ui->ld2Btn->setToolTip(tr("Choose  one or more images  to load"));
    this->ui->ldproBtn->setToolTip(tr("Load Project file"));
    this->ui->inBtn->setToolTip(tr("Zoom In"));
    this->ui->outBtn->setToolTip(tr("Zoom Out"));
    this->ui->orgBtn->setToolTip(tr("Original size"));
    this->ui->savBtn->setToolTip(tr("Save as ..."));
    this->ui->reportLbl->setToolTip(tr("Result Display"));
    this->ui->imgQueueList->setToolTip(tr("Images List"));
    this->ui->countBtn->setToolTip(tr("Count current image"));
    this->ui->minSpin->setToolTip(tr("Minimum contour radius"));
    this->ui->maxSpin->setToolTip(tr("Maximum contour radius"));
    this->ui->stepSpin->setToolTip(tr("Step value"));
    this->ui->csResView->setToolTip(tr("Result Table"));
    this->ui->bchBtn->setToolTip(tr("Batch Count"));
    this->ui->exptBtn->setToolTip(tr("Export Results"));
    this->ui->exptporBtn->setToolTip(tr("Export Project"));
    this->ui->clrBtn->setToolTip(tr("Automatic Color Setting.."));
    this->ui->manclrBtn->setToolTip(tr("Manual Color Setting..."));
    this->ui->handBtn->setToolTip(tr("Manual Adjustment"));
}

void CellCounter::check_dir(QString ckdir)
{
    //Check Dir exists
    if(!QDir(ckdir).exists(ckdir))
    {
        QMessageBox::warning(this,tr("Warning"),tr("Directory is not exists."),QMessageBox::Ok,NULL);
        return;
    }
}

void CellCounter::check_count()
{
    //Current image must be not empty
    if(imgCurr.isEmpty())
    {
        QMessageBox::warning(this,tr("Warning"),tr("Current image is not exists."),QMessageBox::Ok,NULL);
        return;
    }
}

void CellCounter::load_img_srcdir()
{
    //check imgsrcdir
    QString tmpDir;
    tmpDir=this->ui->imgSrcDir->text();
    check_dir(tmpDir);

    //get files list
    QFileInfo fileInfoDir(tmpDir);
    QDir dir;
    dir.setPath(fileInfoDir.filePath());
    dir.setFilter(QDir::Files |  QDir::NoSymLinks);

    //init
    const QFileInfoList fileList = dir.entryInfoList();
    imgQueue.clear();
    imgCountMap.clear();
    imgPointMap.clear();
    imgPointMapMan.clear();
    imgResultMap.clear();

    imgRwellMap.clear();
    imgRcicrleMap.clear();
    ccmode=0;
    if (cclayout->count()>=1) delete ccview;
    //add images of srcdir to image queue
    for (int i = 0; i < fileList.count(); i++) {
        QFileInfo fileInfo = fileList.at(i);
        if (imgFormats.contains(fileInfo.suffix().toLocal8Bit().toLower()))
         {

            QImage tmpimg(fileInfo.absoluteFilePath());
            QString tmpmsg;
            if(tmpimg.width()<1280 || tmpimg.height() < 960) {
                tmpmsg = tr("The Current image (%1) has a resolution lower than 1280px x 960px. ").arg(fileInfo.absoluteFilePath());
                QMessageBox::warning(this,tr("Warning"),tmpmsg);
            } else if(tmpimg.width()*3 != tmpimg.height()*4) {
                int mcd=max_comm_divisor(tmpimg.width(),tmpimg.height());
                tmpmsg =tr("The image aspect ratio has to be 4:3. The current image (%1) has aspect ration of %2:%3. ").arg(fileInfo.absoluteFilePath()).arg(tmpimg.width()/mcd).arg(tmpimg.height()/mcd);
                QMessageBox::warning(this,tr("Warning"),tmpmsg);

            } else {
                imgQueue.append(fileInfo.absoluteFilePath());
                //init count or not //first
                imgCountMap.insert(fileInfo.absoluteFilePath(),0);
                //init rwell value //first
                imgRwellMap.insert(fileInfo.absoluteFilePath(),this->ui->riadusSpin->value());
                //init rcicrle value //first
                imgRcicrleMap.insert(fileInfo.absoluteFilePath(),this->ui->nodeSpin->value());
            }



         }
     }
    //image queue added to list
     if(imgQueue.count()>0)
     {
        this->ui->imgQueueList->clear();
        for(int i=0;i<imgQueue.count();i++)
        {
            QFileInfo fileInfo = imgQueue.at(i);

            QListWidgetItem *item = new QListWidgetItem;
            item->setIcon(QIcon(imgQueue.at(i)));
            item->setText(fileInfo.fileName());
            item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom | Qt::AlignAbsolute);
            item->setToolTip(QString::number(i+1));

            this->ui->imgQueueList->addItem(item);
         }
        if(cclayout->count()>=1) delete ccview;
        //generate result table
        create_restbl(0);

     }

}

void CellCounter::on_imgQueueList_itemClicked(QListWidgetItem* item)
{
    if(!item->text().isEmpty())
    {
     //QMessageBox::warning(this,tr("3"),QString::number(this->ui->imgQueueList->currentRow()),QMessageBox::Ok,NULL);
     count_curr(this->ui->imgQueueList->currentRow());
     this->ui->csResView->selectRow(this->ui->imgQueueList->currentRow());
    }

}
void CellCounter::on_ldBtn_clicked()
{

        //save current image dir
        QString imgsd;
        ccsettings->beginGroup("CellCounter");
        imgsd=ccsettings->value("IMGDIR",QDir::homePath()).toString();
        ccsettings->endGroup();

        if(!QDir(imgsd).isReadable())
        {
          imgsd=QDir::homePath();
        }

        imgsd = QFileDialog::getExistingDirectory(this, tr("Choose Image Source Directory"),
                                                  imgsd,
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
        if(imgsd.isEmpty())
            return;
        else
        {

          imgSrcDir=imgsd;
          ccsettings->beginGroup("CellCounter");
          ccsettings->setValue("IMGDIR",imgsd);
          ccsettings->endGroup();

          this->ui->imgSrcDir->setText(imgSrcDir);
          this->ui->imgSrcDir->setToolTip(imgSrcDir);

          //load image list
          load_img_srcdir();

        }
}

void CellCounter::on_inBtn_clicked()
{
    //zoom in
    if(!imgCurr.isNull())
    {
        ccview->zoom_in();
        ccmode+=1;
    }
}

void CellCounter::on_outBtn_clicked()
{
    //zoom out
    if(!imgCurr.isNull())
    {
        ccview->zoom_out();
        ccmode-=1;
    }
}

void CellCounter::on_actionOnline_triggered()
{
    //online help
    QDesktopServices::openUrl(QUrl("https://bitbucket.org/linora/cellcounter"));
}

void CellCounter::on_actionExit_triggered()
{
    //quit
     exit(-1);
}

void CellCounter::on_actionAbout_triggered()
{
    //about us
    QString aboutus;
    QVBoxLayout *aboutly;
    QHBoxLayout *aboutly2;
    QDialog *aboutdlg;
    QPushButton *closeBtn;
    QLabel *aboutlb;
    aboutus=tr("Xiaoni Li ( lxn0601@mail.ustc.edu.cn ) \
               <br />Hongshun Yang ( yanghongshun@gmail.com ) \
               <br />Hailiang Huang ( hlhuang@jhmi.edu ) \
              <br />Tao Zhu ( zhut@ustc.edu.cn ) ");


    QString about_tab=QString::fromLocal8Bit("<table cellpadding=\"0\" cellspacing=\"1\" border=\"0\" >");
    QString cc_tr=QString::fromLocal8Bit("<tr><td  align=\"center\" colspan=\"2\" >%1</td></tr>").arg(tr("CellCounter"));
    QString v_tr=QString::fromLocal8Bit("<tr><td  align=\"right\" >Version:</td><td   align=\"left\" style=\"padding-left:10px;\" >%1</td></tr>").arg(tr("0.1"));
    QString site_tr=QString::fromLocal8Bit("<tr><td  align=\"right\" >Website:</td><td   align=\"left\" style=\"padding-left:10px;\" ><a href=\"https://bitbucket.org/linora/cellcounter\">%1</a></td></tr>").arg(tr("https://bitbucket.org/linora/cellcounter"));
    QString lic_tr=QString::fromLocal8Bit("<tr><td  align=\"right\" >License:</td><td  align=\"left\" style=\"padding-left:10px;\" ><a href=\"%1\" target=\"_blank\">THE MIT LICENSE</a></td></tr>").arg(tr("http://opensource.org/licenses/MIT"));

    QString dev_tr=QString::fromLocal8Bit("<tr><td align=\"center\" >Developers:</td><td align=\"left\" style=\"padding-left:10px;\" >%1</td></tr>").arg(aboutus);

    QString about_ble=QString::fromLocal8Bit("</table>");


    about_tab.append(cc_tr).append(v_tr).append(site_tr).append(lic_tr).append(dev_tr).append(about_ble);
   // about_text->setHtml(about_tab);

    aboutdlg = new QDialog();

    aboutdlg->setFixedSize(450,250);
    aboutlb = new QLabel(about_tab);
    aboutlb->setOpenExternalLinks(true);
    closeBtn = new QPushButton(tr("Close"));
    connect(closeBtn,SIGNAL(clicked()),aboutdlg,SLOT(close()));

    aboutly = new QVBoxLayout();

    aboutly2  = new QHBoxLayout();
    aboutly2->addWidget(new QLabel());
    aboutly2->addWidget(closeBtn);
    aboutly2->addWidget(new QLabel());

    aboutly->addWidget(aboutlb);
    aboutly->addLayout(aboutly2);

    aboutdlg->setLayout(aboutly);
    //QMessageBox::about(this,tr("CellCounter"),about_text);
    aboutdlg->exec();
}

void CellCounter::on_actionOpen_directory_triggered()
{
    //menu button
    on_ldBtn_clicked();
}

void CellCounter::on_actionOpen_file_triggered()
{
    load_imgs();
}


void CellCounter::on_bchBtn_clicked()
{

    if(imgQueue.count()>0)
    {
        cscv=0;
        this->ui->statusBar->showMessage(QString("Counting ..."));

        QApplication::setOverrideCursor(Qt::WaitCursor);
        for(int i=0;i<imgQueue.count();i++)
        {
            //set rwell rcicrle value for current image
            ccsettings->beginGroup("CellCounter");
            ccsettings->setValue("Rcicrle",QString::number(imgRcicrleMap.value(imgQueue.at(i))));
            ccsettings->setValue("Rwell",QString::number(imgRwellMap.value(imgQueue.at(i))));
            ccsettings->endGroup();
            //sync ui value
            this->ui->riadusSpin->setValue(imgRwellMap.value(imgQueue.at(i)));
            this->ui->nodeSpin->setValue(imgRcicrleMap.value(imgQueue.at(i)));

            //new cscv count
            cscv = new CSOpencv(imgQueue.at(i),ccsettings);
            cscv->process_cellcount();
            //count_curr(i);
            //result
            imgResultMap.insert(imgQueue.at(i),cscv->COUNT_RESULT);
            imgCountMap.insert(imgQueue.at(i),0);

            //result table
            QStandardItem *resitem = new QStandardItem(QString::number(cscv->COUNT_RESULT));
            resitem->setTextAlignment(Qt::AlignCenter);
            resModel->setItem(i, 2, resitem);
            this->ui->csResView->setModel(resModel);
            this->ui->csResView->selectRow(i);
            this->ui->statusBar->showMessage(QString("Counting %1 ...").arg(QFileInfo(imgQueue.at(i)).fileName()));

        }
        if(cclayout->count()>=1)delete ccview;
        this->ui->exptBtn->setEnabled(true);
        this->ui->statusBar->showMessage(QString("Completed."));
    }
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}
void CellCounter::create_restbl(int i)
{

    if(imgQueue.count()>0){
    //result table ready
    int resRow=imgQueue.count();
    int resCol=3;//id,file name , result
    resModel = new QStandardItemModel(resRow,resCol,this);

    //set horizontal header

    resModel->setHorizontalHeaderItem(0,new QStandardItem(QString("ID")));
    resModel->setHorizontalHeaderItem(1,new QStandardItem(QString("File Name")));
    resModel->setHorizontalHeaderItem(2,new QStandardItem(QString("Result")));

    //hide vertical header
    this->ui->csResView->verticalHeader()->hide();

    //fill  value
    //id
     for( int r=0; r<resRow; r++ )
     {

         QStandardItem *iditem = new QStandardItem(QString::number(r+1));
         iditem->setTextAlignment(Qt::AlignCenter);
         resModel->setItem(r, 0, iditem);

         QFileInfo finfo = imgQueue.at(r);
         QStandardItem *fnitem = new QStandardItem(finfo.fileName());
         fnitem->setToolTip(finfo.fileName());
         fnitem->setTextAlignment(Qt::AlignCenter);
         resModel->setItem(r,1,fnitem);
         if(i==1){
             //result
             QStandardItem *resitem = new QStandardItem(QString::number(imgPointMap.keys().count(imgQueue.at(r))));
             resitem->setTextAlignment(Qt::AlignCenter);
             resModel->setItem(r,2,resitem);
         }
     }
    this->ui->csResView->setModel(resModel);
    }
}

void CellCounter::on_exptBtn_clicked()
{
    int wdth=0;
    int hght=0;
    for(int i=0;i<3;i++) //3 columns
        wdth +=this->ui->csResView->columnWidth(i);
    wdth+=1;    //correct width

    for(int i=0;i<imgQueue.count();i++) //header
        hght +=this->ui->csResView->rowHeight(i);
    hght+=25;   //correct hight

    //dir
    QString imgsd;
    ccsettings->beginGroup("CellCounter");
    imgsd=ccsettings->value("IMGDIR",QDir::homePath()).toString();
    ccsettings->endGroup();


    //save .pdf
    QString svpdf = QFileDialog::getSaveFileName(this,tr("Export PDF"),imgsd,tr("PDF File (*.pdf)"));
    if(!svpdf.isEmpty())
    {
        if(QFileInfo(svpdf).suffix().isEmpty())
        {
           svpdf.append(".pdf");
         }
         QPrinter ccprinter(QPrinter::ScreenResolution);
         ccprinter.setOutputFormat(QPrinter::PdfFormat);
         ccprinter.setPageSize(QPrinter::A4);
         ccprinter.setOutputFileName(svpdf);

         QFile pdffile(svpdf);
         if (!pdffile.open(QFile::WriteOnly))
         {
             QMessageBox::warning(this,tr("Warning"),tr("Error to open file"),QMessageBox::Ok,NULL);
             return;
         }

         QDateTime *count_time=new QDateTime(QDateTime::currentDateTime());


         QTextDocument *pdf_text = new QTextDocument(this);
         QString poweredby=tr("Generated by CellCounter   ")+count_time->toString();

         QString resTable=QString::fromLocal8Bit("<table cellpadding=\"0\" cellspacing=\"1\" border=\"0\"  bgcolor=\"#000000\" width=\"90%\" ><tr><td bgcolor=\"#FFFFFF\" align=\"center\" >ID</td><td bgcolor=\"#FFFFFF\" align=\"center\">File Name</td><td bgcolor=\"#FFFFFF\" align=\"center\">Result</td></tr>");
         QString res_tr=QString::fromLocal8Bit("<tr><td bgcolor=\"#FFFFFF\" align=\"center\" >%1</td><td  bgcolor=\"#FFFFFF\" align=\"center\" >%2</td><td bgcolor=\"#FFFFFF\" align=\"center\" >%3</td></tr>");
         QString res_end=QString::fromLocal8Bit("</table>");

         for(int i=0;i<imgQueue.count();i++)
         {
            QString res_row=res_tr.arg(i+1).arg(QFileInfo(imgQueue.at(i)).fileName()).arg(imgResultMap.value(imgQueue.at(i)));
            resTable.append(res_row);
         }
         resTable.append(res_end);
         QString result=resTable+poweredby;
         pdf_text->setHtml(result);
         pdf_text->print(&ccprinter);
         pdffile.close();
   }
}


void CellCounter::on_maxSpin_valueChanged(double arg1)
{
    double Rmax;
    ccsettings->beginGroup("CellCounter");
    Rmax=ccsettings->value("Max","6").toDouble();
    ccsettings->endGroup();
    if(Rmax!=arg1)
    {
    ccsettings->beginGroup("CellCounter");
    ccsettings->setValue("Max",QString::number(arg1));
    ccsettings->endGroup();

    this->ui->minSpin->setMaximum(arg1);
    this->ui->riadusSpin->setMaximum(arg1);

    if(!imgCurr.isNull()) imgCountMap.insert(imgCurr,-1);
    }
}

void CellCounter::on_minSpin_valueChanged(double arg1)
{
    double Rmin;
    ccsettings->beginGroup("CellCounter");
    Rmin=ccsettings->value("Min","4").toDouble();
    ccsettings->endGroup();
    if(Rmin!=arg1)
    {

    ccsettings->beginGroup("CellCounter");
    ccsettings->setValue("Min",QString::number(arg1));
    ccsettings->endGroup();

    this->ui->stepSpin->setMaximum(arg1);
    this->ui->maxSpin->setMinimum(arg1);

    if(!imgCurr.isNull()) imgCountMap.insert(imgCurr,-1);
    }
 }

void CellCounter::on_stepSpin_valueChanged(double arg1)
{
    double Rstep;
    ccsettings->beginGroup("CellCounter");
    Rstep=ccsettings->value("Step","0.5").toDouble();
    ccsettings->endGroup();
    if(Rstep!=arg1)
    {
    ccsettings->beginGroup("CellCounter");
    ccsettings->setValue("Step",QString::number(arg1));
    ccsettings->endGroup();

   if(!imgCurr.isNull())  imgCountMap.insert(imgCurr,-1);
    }
}

void CellCounter::on_countBtn_clicked()
{
    if(QFileInfo(imgCurr).exists())
    {
        //result table
        QStandardItem *resitem = new QStandardItem(this->ui->reportLbl->text());
        resitem->setTextAlignment(Qt::AlignCenter);
        resModel->setItem(this->ui->imgQueueList->currentRow(), 2, resitem);
        this->ui->csResView->setModel(resModel);

        imgResultMap.insert(imgCurr,ccview->allPoint.count());


        this->ui->csResView->selectRow(this->ui->imgQueueList->currentRow());
    }
}
void CellCounter::count_curr(int i)
{

       QApplication::setOverrideCursor(Qt::WaitCursor);
       this->ui->reportLbl->setText(tr("..."));
       this->ui->statusBar->showMessage(tr("Counting..."));
       if(!imgQueue.isEmpty())
            imgCurr=imgQueue.at(i);
       else
            QMessageBox::warning(this,tr("Warning"),
                                     tr("Sorry,unknow error occurred,please restart application."),QMessageBox::Ok,NULL);
        check_count();



        //2 means from project
        if(imgCountMap.value(imgCurr)==2)
        {


            if(cclayout->count()>=1) delete ccview;
            currPX = QPixmap::fromImage(QImage(imgCurr));
            ccview= new CSView(this,ccsettings,imgCurr,this->ui->reportLbl,currPX,ccmode);

            cclayout->addWidget(ccview);
            ccview->autoPointList=imgPointMap.values(imgCurr);
            ccview->manPoint = imgPointMapMan.values(imgCurr);
            ccview->init();
            //status
            this->ui->statusBar->showMessage(imgCurr);
            QApplication::setOverrideCursor(Qt::ArrowCursor);

            imgCountMap.insert(imgCurr,1);
            return;
        }

        //set rwell rcicrle value for current image
        ccsettings->beginGroup("CellCounter");
        ccsettings->setValue("Rcicrle",QString::number(imgRcicrleMap.value(imgCurr)));
        ccsettings->setValue("Rwell",QString::number(imgRwellMap.value(imgCurr)));
        ccsettings->endGroup();
        //sync ui value
        this->ui->riadusSpin->setValue(imgRwellMap.value(imgCurr));
        this->ui->nodeSpin->setValue(imgRcicrleMap.value(imgCurr));

        if(imgCountMap.value(imgCurr)==1)
           {
             currPX=QPixmap::fromImage(QImage(imgCurr));
            }
         else
         {
            cscv = new CSOpencv(imgCurr,ccsettings);
            cscv->process_cellcount();
            currPX = QPixmap::fromImage(cscv->ipl2qimg(cscv->cellMark));
         }

        //QMessageBox::warning(this,tr("3"),QString::number(imgPointMapMan.count()),QMessageBox::Ok,NULL);


        if(cclayout->count()>=1)
        {
            if(imgCountMap.value(ccview->imgCountCurr)==-1)
            {

                //V2 added
                imgPointMap.remove(ccview->imgCountCurr);
                imgPointMapMan.remove(ccview->imgCountCurr);

                for(int i=0;i<ccview->allPoint.count();i++)
                {
                        imgPointMap.insert(ccview->imgCountCurr,ccview->allPoint.at(i));
                }
                for(int i=0;i<ccview->manPoint.count();i++)
                {
                        imgPointMapMan.insert(ccview->imgCountCurr,ccview->manPoint.at(i));
                }
                //V2 END
                imgCountMap.insert(ccview->imgCountCurr,0);
                delete ccview;

            }
            else
            {
              //sync current ccview.allpoint
                imgPointMap.remove(ccview->imgCountCurr);
                imgPointMapMan.remove(ccview->imgCountCurr);

                for(int i=0;i<ccview->allPoint.count();i++)
                {
                        imgPointMap.insert(ccview->imgCountCurr,ccview->allPoint.at(i));
                }
                for(int i=0;i<ccview->manPoint.count();i++)
                {
                        imgPointMapMan.insert(ccview->imgCountCurr,ccview->manPoint.at(i));
                }
                //1 count;0 no count
                imgCountMap.insert(ccview->imgCountCurr,1);
              //delete current ccview
                delete ccview;
            }
        }

        ccview= new CSView(this,ccsettings,imgCurr,this->ui->reportLbl,currPX,ccmode);

        if(imgCountMap.value(ccview->imgCountCurr)==0)
        {
            ccview->autoPointList=cscv->cellPointList;
            //new load
            if(cclayout->count()==0) {
                //sync current ccview.allpoint
                  imgPointMap.remove(ccview->imgCountCurr);
                  imgPointMapMan.remove(ccview->imgCountCurr);
                  for(int i=0;i<ccview->autoPointList.count();i++)
                  {
                          imgPointMap.insert(ccview->imgCountCurr,ccview->autoPointList.at(i));
                  }
                  for(int i=0;i<ccview->manPoint.count();i++)
                  {
                          imgPointMapMan.insert(ccview->imgCountCurr,ccview->manPoint.at(i));
                  }
            }
            ccview->manPoint=imgPointMapMan.values(ccview->imgCountCurr);

        }
        else
        {
            ccview->autoPointList=imgPointMap.values(ccview->imgCountCurr);
            ccview->manPoint=imgPointMapMan.values(ccview->imgCountCurr);
        }
        cclayout->addWidget(ccview);
        ccview->init();

        //status
        this->ui->statusBar->showMessage(imgCurr);
        QApplication::setOverrideCursor(Qt::ArrowCursor);

}

void CellCounter::on_orgBtn_clicked()
{
    //original image
    if(!imgCurr.isNull())
    {
        if(ccmode!=0)
            ccview->scaleNum=ccmode;
        ccview->zoom_normal();
        ccmode=0;
    }
}

void CellCounter::on_savBtn_clicked()
{
    //save as image
    if(!imgCurr.isNull())
    {
        QString svfl = QFileDialog::getSaveFileName(this,tr("Save as ..."),imgSrcDir,saveFormats);
        if(!svfl.isEmpty())
        {
            if(QFileInfo(svfl).suffix().isEmpty())
            {
                svfl.append(tr(".")+QFileInfo(imgCurr).suffix());
            }

            QImage img(ccview->scene()->sceneRect().size().toSize(),QImage::Format_ARGB32_Premultiplied);
            QPainter imgPainter(&img);
            imgPainter.setRenderHint(QPainter::Antialiasing,true);
            QBrush bsh(currPX);
            ccview->ccscene->setBackgroundBrush(bsh);
            ccview->ccscene->render(&imgPainter);
            if((img.width()-280>0)&&img.height()>10)
            {

                QFont fnt = this->ui->reportLbl->font();
                fnt.setBold(true);
                fnt.setPointSize(24);
                imgPainter.setFont(fnt);
                imgPainter.setPen(Qt::red);
                QRectF textRect(img.width()*4/5,10,img.width()/5,100);
                QString mesg=tr("Result: %1 ").arg(this->ui->reportLbl->text());
                imgPainter.drawText(textRect,mesg);
            }
            imgPainter.end();
            img.save(svfl);

        }
    }
}

void CellCounter::on_riadusSpin_valueChanged(double arg1)
{
    double Rwell;
    ccsettings->beginGroup("CellCounter");
    Rwell=ccsettings->value("Rwell","4").toDouble();
    ccsettings->endGroup();
 //   QMessageBox::warning(this,tr("1"),QString::number(Rwell),QMessageBox::Ok,NULL);
    if(Rwell!=arg1)
    {
    //sync settings
    ccsettings->beginGroup("CellCounter");
    ccsettings->setValue("Rwell",QString::number(arg1));
    ccsettings->endGroup();

 //   QMessageBox::warning(this,tr("2"),QString::number(arg1),QMessageBox::Ok,NULL);

   if(!imgCurr.isNull())
   {
       imgCountMap.insert(imgCurr,-1);
       //change rwell rcicrle value/current image
        imgRwellMap.insert(imgCurr,arg1);
    //   count_curr(this->ui->imgQueueList->currentRow());
    }
   }
}


void CellCounter::on_clrBtn_clicked()
{
    //mark color
    int mark_r;
    int mark_g;
    int mark_b;

    ccsettings->beginGroup("CellCounter");
    mark_r=ccsettings->value("MarkColorR","0").toInt();
    mark_g=ccsettings->value("MarkColorG","255").toInt();
    mark_b=ccsettings->value("MarkColorB","0").toInt();
    ccsettings->endGroup();

    markColor =QColor(mark_r,mark_g,mark_b);
    markColor=QColorDialog::getColor(markColor,this,tr("Mark Color"));
    if(markColor.isValid())
    {
        ccsettings->beginGroup("CellCounter");
        ccsettings->setValue("MarkColorR",QString::number(markColor.red()));
        ccsettings->setValue("MarkColorG",QString::number(markColor.green()));
        ccsettings->setValue("MarkColorB",QString::number(markColor.blue()));
        ccsettings->endGroup();


        this->ui->clrBtn->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(markColor.red())
                                                                           .arg(markColor.green())
                                                                           .arg(markColor.blue()));

        if(!imgCurr.isNull())
            count_curr(this->ui->imgQueueList->currentRow());
    }


}

void CellCounter::on_nodeSpin_valueChanged(double arg1)
{
    double Rcicrle;
    ccsettings->beginGroup("CellCounter");
    Rcicrle=ccsettings->value("Rcicrle","5").toDouble();
    ccsettings->endGroup();
 //   QMessageBox::warning(this,tr("1"),QString::number(Rwell),QMessageBox::Ok,NULL);
    if(Rcicrle!=arg1)
    {
    //sync settings

    ccsettings->beginGroup("CellCounter");
    ccsettings->setValue("Rcicrle",QString::number(arg1));
    ccsettings->endGroup();

    if(!imgCurr.isNull())
    {
       // imgCountMap.insert(imgCurr,-1);
        //change rwell rcicrle value/current image
         imgRcicrleMap.insert(imgCurr,arg1);
       // count_curr(this->ui->imgQueueList->currentRow());
    }

    }
}

void CellCounter::on_ld2Btn_clicked()
{
    load_imgs();
}
void CellCounter::load_imgs()
{
    //save current image dir
    QString imgsd;
    ccsettings->beginGroup("CellCounter");
    imgsd=ccsettings->value("IMGDIR",QDir::homePath()).toString();
    ccsettings->endGroup();

    if(!QDir(imgsd).isReadable())
    {
      imgsd=QDir::homePath();
    }

    QStringList flnames = QFileDialog::getOpenFileNames(this,tr("choose one or more images to count"),
                                                       imgsd,
                                                       openFormats);

    if (flnames.isEmpty())
    {
        return;
    }
    else
    {
        QFileInfo fileDir=flnames.at(0);
        imgSrcDir=fileDir.dir().absolutePath();
        ccsettings->beginGroup("CellCounter");
        ccsettings->setValue("IMGDIR",imgSrcDir);
        ccsettings->endGroup();
        this->ui->imgSrcDir->setText(imgSrcDir);
        this->ui->imgSrcDir->setToolTip(imgSrcDir);

        imgQueue.clear();
        imgPointMap.clear();
        imgPointMapMan.clear();
        imgCountMap.clear();
        imgRwellMap.clear();
        imgRcicrleMap.clear();
        for(int i=0;i<flnames.count();i++)
        {
            QFileInfo fileInfo = flnames.at(i);

            QImage tmpimg(fileInfo.absoluteFilePath());
            QString tmpmsg;
            if(tmpimg.width()<1280 || tmpimg.height() < 960) {
                tmpmsg = tr("The Current image (%1)  has a resolution lower than 1280px x 960px. ").arg(fileInfo.absoluteFilePath());
                QMessageBox::warning(this,tr("Warning"),tmpmsg);
            } else if(tmpimg.width()*3 != tmpimg.height()*4) {
                int mcd=max_comm_divisor(tmpimg.width(),tmpimg.height());
                tmpmsg =tr("The image aspect ratio has to be 4:3. The current image (%1) has aspect ration of %2:%3. ").arg(fileInfo.absoluteFilePath()).arg(tmpimg.width()/mcd).arg(tmpimg.height()/mcd);
                QMessageBox::warning(this,tr("Warning"),tmpmsg);
            } else {
                imgQueue.append(fileInfo.absoluteFilePath());
            }
        }



        if(imgQueue.count()>0)
        {
           this->ui->imgQueueList->clear();
           for(int i=0;i<imgQueue.count();i++)
           {
               QFileInfo fileInfo = imgQueue.at(i);
               //init count or not for every image
               imgCountMap.insert(fileInfo.absoluteFilePath(),0);
               //init rwell value //first
               imgRwellMap.insert(fileInfo.absoluteFilePath(),this->ui->riadusSpin->value());
               //init rcicrle value //first
               imgRcicrleMap.insert(fileInfo.absoluteFilePath(),this->ui->nodeSpin->value());

               QListWidgetItem *item = new QListWidgetItem;
               item->setIcon(QIcon(fileInfo.absoluteFilePath()));
               item->setText(fileInfo.fileName());
               item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom | Qt::AlignAbsolute);
               item->setToolTip(QString::number(i+1));

               this->ui->imgQueueList->addItem(item);

            }
           if(cclayout->count()>=1) delete ccview;

        }
       create_restbl(0);
    }
}

void CellCounter::on_csResView_clicked(const QModelIndex &index)
{
    //ID click
    if(index.column()==0)
    {
        count_curr(index.row());
        this->ui->imgQueueList->item(index.row())->setSelected(true);
    }

}


void CellCounter::on_contactbtn_clicked()
{
    //online help
    QDesktopServices::openUrl(QUrl("https://bitbucket.org/linora/cellcounter"));
}
void CellCounter::save_proj()
{
    //save current image dir
    QString imgsd;
    ccsettings->beginGroup("CellCounter");
    imgsd=ccsettings->value("IMGDIR",QDir::homePath()).toString();
    ccsettings->endGroup();

    if(!QDir(imgsd).isReadable())
    {
      imgsd=QDir::homePath();
    }

    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Save Project File"),
                                         imgsd,
                                         tr("XCCEL Files (*.xccel *.xml)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("CellCounter Project File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    //imgPointMapManthis->ccview->manPoint;
    XccelGenerator ccgenerator(ccsettings,imgPointMap,imgPointMapMan,imgRwellMap,imgRcicrleMap);
    if (ccgenerator.write(&file))
        this->ui->statusBar->showMessage(tr("File saved"));

}
bool CellCounter::open_proj()
{
    //save current image dir
    QString imgsd;
    ccsettings->beginGroup("CellCounter");
    imgsd=ccsettings->value("IMGDIR",QDir::homePath()).toString();
    ccsettings->endGroup();

    if(!QDir(imgsd).isReadable())
    {
      imgsd=QDir::homePath();
    }


    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open CellCounter Project File"),
                                         imgsd,
                                         tr("XCCEL Files (*.xccel *.xml)"));
    if (fileName.isEmpty())
        return false;
    cchandler= new XccelHandler(this->ccsettings);
    QXmlSimpleReader reader;
    reader.setContentHandler(cchandler);
    reader.setErrorHandler(cchandler);

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("CellCounter"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QXmlInputSource xmlInputSource(&file);
    if (reader.parse(xmlInputSource))
    {
        statusBar()->showMessage(tr("File loaded"), 2000);
        return true;
    }
    return false;
}

void CellCounter::on_exptporBtn_clicked()
{
    for(int i=0;i<imgQueue.count();i++){
        count_curr(i);
    }

    if(imgQueue.count()>0)
        save_proj();
}

void CellCounter::on_ldproBtn_clicked()
{

    if(open_proj())
    {
        //init color
        //mark color
        int mark_r;
        int mark_g;
        int mark_b;
        int manmark_r;
        int manmark_g;
        int manmark_b;
        double rwell;
        double rcicrle;

        ccsettings->beginGroup("CellCounter");
        mark_r=ccsettings->value("MarkColorR","0").toInt();
        mark_g=ccsettings->value("MarkColorG","255").toInt();
        mark_b=ccsettings->value("MarkColorB","0").toInt();
        manmark_r=ccsettings->value("ManMarkColorR","255").toInt();
        manmark_g=ccsettings->value("ManMarkColorG","0").toInt();
        manmark_b=ccsettings->value("ManMarkColorB","0").toInt();
        imgSrcDir=ccsettings->value("IMGDIR",QDir::homePath()).toString();
        rwell=ccsettings->value("Rwell","4").toDouble();
        rcicrle=ccsettings->value("Rcicrle","5").toDouble();
        ccsettings->endGroup();

        markColor =QColor(mark_r,mark_g,mark_b);
        manmarkColor =QColor(manmark_r,manmark_g,manmark_b);

        this->ui->clrBtn->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(markColor.red())
                                                                           .arg(markColor.green())
                                                                           .arg(markColor.blue()));

        this->ui->manclrBtn->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(manmarkColor.red())
                                                                           .arg(manmarkColor.green())
                                                                           .arg(manmarkColor.blue()));

        //init imgsrcdir
        this->ui->imgSrcDir->setText(imgSrcDir);
        this->ui->imgSrcDir->setToolTip(imgSrcDir);
        this->ui->riadusSpin->setValue(rwell);
        this->ui->nodeSpin->setValue(rcicrle);

        imgCountMap.clear();
        //load imgs

          imgQueue.clear();
          imgQueue=cchandler->xccelrwells.keys();
          imgPointMap.clear();
          imgPointMap=cchandler->xccelmaps;
          imgPointMapMan.clear();
          imgPointMapMan=cchandler->xccelmapsman;
          imgRwellMap.clear();
          imgRwellMap=cchandler->xccelrwells;
          imgRcicrleMap.clear();
          imgRcicrleMap=cchandler->xccelrcicrles;

        if(imgQueue.count()>0)
        {
           this->ui->imgQueueList->clear();
           for(int i=0;i<imgQueue.count();i++)
           {
               QFileInfo fileInfo = imgQueue.at(i);

               QListWidgetItem *item = new QListWidgetItem;
               item->setIcon(QIcon(imgQueue.at(i)));
               item->setText(fileInfo.fileName());
               item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom | Qt::AlignAbsolute);
               item->setToolTip(QString::number(i+1));

               this->ui->imgQueueList->addItem(item);

                //counted 2 means project
               imgCountMap.insert(imgQueue.at(i),2);
            }

           //generate result table
           //set result;
           create_restbl(1);

        }


    }
}
void CellCounter::on_handBtn_toggled(bool checked)
{
    int ManMode;
    if(checked){
        ManMode=1;
    } else {
        ManMode=0;
    }
    ccsettings->beginGroup("CellCounter");
    ccsettings->setValue("ManMode",QString::number(ManMode));
    ccsettings->endGroup();
}
int CellCounter::max_comm_divisor(int x, int y)
{
    int min, max;
    max=x>y?x:y;
    min=x<y?x:y;
    if(max%min==0)
        return min;
    else
        return max_comm_divisor(min,max%min);
}

void CellCounter::on_manclrBtn_clicked()
{
    //mark color
    int manmark_r;
    int manmark_g;
    int manmark_b;

    ccsettings->beginGroup("CellCounter");
    manmark_r=ccsettings->value("ManMarkColorR","255").toInt();
    manmark_g=ccsettings->value("ManMarkColorG","0").toInt();
    manmark_b=ccsettings->value("ManMarkColorB","0").toInt();
    ccsettings->endGroup();

    manmarkColor =QColor(manmark_r,manmark_g,manmark_b);
    manmarkColor=QColorDialog::getColor(manmarkColor,this,tr("Man Mark Color"));
    if(manmarkColor.isValid())
    {
        ccsettings->beginGroup("CellCounter");
        ccsettings->setValue("ManMarkColorR",QString::number(manmarkColor.red()));
        ccsettings->setValue("ManMarkColorG",QString::number(manmarkColor.green()));
        ccsettings->setValue("ManMarkColorB",QString::number(manmarkColor.blue()));
        ccsettings->endGroup();


        this->ui->manclrBtn->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(manmarkColor.red())
                                                                           .arg(manmarkColor.green())
                                                                           .arg(manmarkColor.blue()));
        //V2 added
        if(!imgCurr.isNull())
            count_curr(this->ui->imgQueueList->currentRow());
    }

}
