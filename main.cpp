#include <QtWidgets/QApplication>
#include "cellcounter.h"
#include<QMessageBox>
#include<QIcon>
class CC: public QApplication
{
public:
   CC(int argc, char **argv): QApplication(argc, argv)
   {

   }
   bool notify(QObject *rec, QEvent *ev)
   {
       try{
            return QApplication::notify(rec, ev);
           }
       catch (...)
       {

           QMessageBox::warning(NULL,
                                tr("Error"),
                                tr("Error occurred\n")+
                                tr("Please restart application."),
                                QMessageBox::Ok,NULL);
            return false;
       }

    }
};
int main(int argc, char *argv[])
{

    CC a(argc, argv);
    //settings
    CC::addLibraryPath(QCoreApplication::applicationDirPath());
    CC::setApplicationName("CellCounter");
    CC::setOrganizationDomain("cellcounter.org");
    CC::setOrganizationName("Laboratory for Tumor Pathology,School of Life Sciences, University of Science and Technology of China");
    CellCounter w;
    w.showMaximized();
    return a.exec();
}
