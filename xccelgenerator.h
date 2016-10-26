#ifndef XCCELGENERATOR_H
#define XCCELGENERATOR_H

#include <QTextStream>
#include <QSettings>
#include <QMap>
#include <QMultiMap>
#include <QPointF>
class XccelGenerator
{
public:
    XccelGenerator(QSettings *ccset,
                   QMultiMap<QString,QPointF> imgPointMap,
                   QMultiMap<QString,QPointF> imgPointMapMan,
                   QMap<QString, double> imgRwellMap,
                   QMap<QString, double> imgRcicrleMap);
    ~XccelGenerator();
    bool write(QIODevice *device);

private:
    static QString indent(int indentLevel);
    static QString escapedText(const QString &str);
    static QString escapedAttribute(const QString &str);
    void generateItem(QString imgPointKey,int depth);

    QSettings *xccelsettings;
    QMultiMap<QString,QPointF> xccelmaps;
    QMultiMap<QString,QPointF> xccelmapsman;
    QMap<QString,double> xccelrwells;
    QMap<QString,double> xccelrcicrles;

    QString imgSrcDir;

    QTextStream out;

    int mark_r;
    int mark_g;
    int mark_b;

    //V2 added
    int manmark_r;
    int manmark_g;
    int manmark_b;

    double step;
    double Rmax;
    double Rmin;
    double Rwell;
    double CWidth;
    double CRiadus;

};
#endif // XCCELGENERATOR_H
