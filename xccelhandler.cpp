#include<QMessageBox>
#include "xccelhandler.h"

XccelHandler::XccelHandler(QSettings *ccset)
{
    metXccelTag=false;

    xccelsettings=ccset;


    xccelmaps.clear();
    xccelmapsman.clear();
    xccelrwells.clear();
    xccelrcicrles.clear();

}
XccelHandler::~XccelHandler()
{

}

bool XccelHandler::startElement(const QString & /* namespaceURI */,
                               const QString & /* localName */,
                               const QString &qName,
                               const QXmlAttributes &attributes)
{
    if (!metXccelTag && qName != "xccel") {
        errorStr = QObject::tr("The file is not an XCCEL(XML) file.");
        return false;
    }
    if (qName == "xccel") {
        QString version = attributes.value("version");
        if (!version.isEmpty() && version != "0.1") {
            errorStr = QObject::tr("The file is not an XCCEL version 0.1 file.");
            return false;
        }
        metXccelTag = true;
    } else if (qName == "images"){

        xccelsettings->beginGroup("CellCounter");
        xccelsettings->setValue("Step",attributes.value("Step"));
        xccelsettings->setValue("Max",attributes.value("Max"));
        xccelsettings->setValue("Min",attributes.value("Min"));
        xccelsettings->setValue("Cwidth",attributes.value("Cwidth"));
        xccelsettings->setValue("MarkColorR",attributes.value("MarkColorR"));
        xccelsettings->setValue("MarkColorG",attributes.value("MarkColorG"));
        xccelsettings->setValue("MarkColorB",attributes.value("MarkColorB"));
        xccelsettings->setValue("ManMarkColorR",attributes.value("ManMarkColorR"));
        xccelsettings->setValue("ManMarkColorG",attributes.value("ManMarkColorG"));
        xccelsettings->setValue("ManMarkColorB",attributes.value("ManMarkColorB"));
        xccelsettings->setValue("IMGDIR",attributes.value("IMGDIR"));
        xccelsettings->endGroup();

    } else if (qName == "image") {
        xccelrwells.insert(attributes.value("path"),attributes.value("rwell").toDouble());
        xccelrcicrles.insert(attributes.value("path"),attributes.value("rcicrle").toDouble());
    } else if (qName == "cell") {
        xccelmaps.insert(attributes.value("file"),QPointF(attributes.value("x").toInt(),attributes.value("y").toInt()));
        if (attributes.value("mode")=="1") {
            xccelmapsman.insert(attributes.value("file"),QPointF(attributes.value("x").toInt(),attributes.value("y").toInt()));
        }
    }



    currentText.clear();
    return true;
}

bool XccelHandler::endElement(const QString & /* namespaceURI */,
                             const QString & /* localName */,
                             const QString &qName)
{
    return true;
}
bool XccelHandler::characters(const QString &str)
{
    currentText += str;
    return true;
}

bool XccelHandler::fatalError(const QXmlParseException &exception)
{
    QMessageBox::information(0, QObject::tr("CellCounter Error"),
                             QObject::tr("Parse error at line %1, column %2:\n"
                                         "%3")
                             .arg(exception.lineNumber())
                             .arg(exception.columnNumber())
                             .arg(exception.message()));

    return false;
}

QString XccelHandler::errorString() const
{
    return errorStr;
}
