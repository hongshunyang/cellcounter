#include "xccelgenerator.h"
#include <QtGui>
#include <QList>
#include <QMessageBox>
#include <QString>
XccelGenerator::XccelGenerator(QSettings *ccset,
                               QMultiMap<QString, QPointF> imgPointMap,
                               QMultiMap<QString, QPointF> imgPointMapMan,
                               QMap<QString,double> imgRwellMap,
                               QMap<QString,double> imgRcicrleMap)
{
    //ccset
    xccelsettings=ccset;
    xccelmaps=imgPointMap;
    xccelmapsman=imgPointMapMan;
    xccelrwells=imgRwellMap;
    xccelrcicrles=imgRcicrleMap;


    xccelsettings->beginGroup("CellCounter");
    imgSrcDir=xccelsettings->value("IMGDIR",QDir::homePath()).toString();
    mark_r=xccelsettings->value("MarkColorR","0").toInt();
    mark_g=xccelsettings->value("MarkColorG","255").toInt();
    mark_b=xccelsettings->value("MarkColorB","0").toInt();
    manmark_r=xccelsettings->value("ManMarkColorR","255").toInt();
    manmark_g=xccelsettings->value("ManMarkColorG","0").toInt();
    manmark_b=xccelsettings->value("ManMarkColorB","0").toInt();
    step=xccelsettings->value("Step","0.5").toDouble();
    Rmax=xccelsettings->value("Max","6").toDouble();
    Rmin=xccelsettings->value("Min","4").toDouble();
    CWidth=xccelsettings->value("Cwidth",2).toInt();
    xccelsettings->endGroup();

}
XccelGenerator::~XccelGenerator()
{

}

bool XccelGenerator::write(QIODevice *device)
{

    out.setDevice(device);
    out.setCodec("UTF-8");
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<!DOCTYPE xccel>\n"
        << "<xccel version=\"0.1\">\n"
        << indent(1) << "<images"
                     << " Step=" << escapedAttribute(QString::number(step))
                     << " Max=" << escapedAttribute(QString::number(Rmax))
                     << " Min=" << escapedAttribute(QString::number(Rmin))
                     << " Cwidth=" << escapedAttribute(QString::number(CWidth))
                     << " MarkColorR=" << escapedAttribute(QString::number(mark_r))
                     << " MarkColorG=" << escapedAttribute(QString::number(mark_g))
                     << " MarkColorB=" << escapedAttribute(QString::number(mark_b))
                     << " ManMarkColorR=" << escapedAttribute(QString::number(manmark_r))
                     << " ManMarkColorG=" << escapedAttribute(QString::number(manmark_g))
                     << " ManMarkColorB=" << escapedAttribute(QString::number(manmark_b))
                     << " IMGDIR=" << escapedAttribute(imgSrcDir)
                     << ">\n";


        QList<QString> xccelkeys = xccelrwells.keys();
     //   QMessageBox::warning(0," ",QString::number(xccelkeys.count()),QMessageBox::Ok,NULL);
        for (int i = 0; i < xccelkeys.count(); i++)
            generateItem(xccelkeys.at(i),1);


    out << indent(1) << "</images>\n"
        << "</xccel>\n";
    return true;
}

QString XccelGenerator::indent(int depth)
{
    const int IndentSize = 4;
    return QString(IndentSize * depth, ' ');
}

QString XccelGenerator::escapedText(const QString &str)
{
    QString result = str;
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    return result;
}

QString XccelGenerator::escapedAttribute(const QString &str)
{
    QString result = escapedText(str);
    result.replace("\"", "&quot;");
    result.prepend("\"");
    result.append("\"");
    return result;
}

void XccelGenerator::generateItem(QString imgPointKey, int depth)
{
    QList<QPointF> imgPointValue = xccelmaps.values(imgPointKey);
    QList<QPointF> imgPointManValue = xccelmapsman.values(imgPointKey);
    out << indent(depth) << "<image";
    out << " rwell=" << escapedAttribute(QString::number(xccelrwells.value(imgPointKey)));
    out << " rcicrle=" << escapedAttribute(QString::number(xccelrcicrles.value(imgPointKey)));
    out << " path=" << escapedAttribute(imgPointKey);
    out << ">\n";


    for(int i=0;i<imgPointValue.count();i++)
    {
        int manmode=0;
        if(imgPointManValue.contains(imgPointValue.at(i))) manmode =1;

        out << indent(depth+1) << "<cell";
        out << " x=" << escapedAttribute(QString::number(imgPointValue.at(i).x()));
        out << " y=" << escapedAttribute(QString::number(imgPointValue.at(i).y()));
        out << " mode=" << escapedAttribute(QString::number(manmode));
        out << " file=" << escapedAttribute(imgPointKey);
        out << ">\n"
            << indent(depth+1) << "</cell>\n";
    }

    out << indent(depth) << "</image>\n";






}
