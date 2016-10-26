#ifndef XCCELHANDLER_H
#define XCCELHANDLER_H
#include <QXmlDefaultHandler>
#include <QMap>
#include <QMultiMap>
#include <QSettings>
#include <QPointF>
#include <QString>
#include<QList>
class XccelHandler : public QXmlDefaultHandler
{
public:
    XccelHandler(QSettings *ccset);
    ~XccelHandler();
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);
    QString errorString() const;

    QMultiMap<QString,QPointF> xccelmaps;
    QMultiMap<QString,QPointF> xccelmapsman;
    QMap<QString,double> xccelrwells;
    QMap<QString,double> xccelrcicrles;

private:


    QString currentText;
    QString errorStr;
    bool metXccelTag;


    QSettings *xccelsettings;


};

#endif // XCCELHANDLER_H
