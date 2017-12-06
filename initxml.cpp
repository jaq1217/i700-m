#include "initxml.h"
#include "ui_initxml.h"
#include <QStandardItem>
#include "qdebug.h"
#include "qfile.h"
#include <QDomDocument>
#include "qtextcodec.h"
initxml::initxml(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::initxml)
{
    ui->setupUi(this);
    int width = this->geometry().width();
    int height = this->geometry().height();
    this->setFixedSize(width,height);
    //QStandardItemModel* model = new QStandardItemModel(ui->treeView);
    //model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Tag")<<QStringLiteral("Index")<<QStringLiteral("SubIndex")<<QStringLiteral("Data")<<QStringLiteral("Comment")<<QStringLiteral("Lenth"));
    //ui->treeView->setModel(model);
    QFile *file;
    filename = "single-jq.xml";
}

initxml::~initxml()
{
    delete ui;
}

void initxml::on_loadButton_clicked()
{
    QString QStr = "";
    if(read_xmll(filename))
    {
        //QStandardItemModel* model = new QStandardItemModel(ui->treeView);
        model = new QStandardItemModel(ui->treeView);
        model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Tag")<<QStringLiteral("Index")<<QStringLiteral("SubIndex")<<QStringLiteral("Data")<<QStringLiteral("Comment")<<QStringLiteral("Lenth"));
        ui->treeView->setModel(model);


        //QStandardItem* itemProject = new QStandardItem(QStringLiteral("InitCmds"));
        itemProject = new QStandardItem(QStringLiteral("InitCmds"));
        model->appendRow(itemProject);
        for(int tempi = 0; tempi < XmlDataNum; tempi++)
        {
            QStandardItem* itemChild = new QStandardItem(QStringLiteral(" "));
            itemProject->appendRow(itemChild);

            itemProject->setChild(tempi,0,new QStandardItem(QString::number(tempi+1,10)));
            itemProject->setChild(tempi,1,new QStandardItem(QString::number(xmlData[tempi].Index,16)));
            itemProject->setChild(tempi,2,new QStandardItem(QString::number(xmlData[tempi].SubIndex,10)));

            if(xmlData[tempi].IsChar)
            {
                QStr = "";

                for(uint8_t intj = 0; intj < xmlData[tempi].DataLenth;intj++)
                {
                    QStr+=xmlData[tempi].DataStr[intj];
                }
                itemProject->setChild(tempi,3,new QStandardItem(QStr));
            }
            else
            {
                itemProject->setChild(tempi,3,new QStandardItem(QString("%1").arg(xmlData[tempi].DataInt,xmlData[tempi].DataLenth * 2,16,QChar('0'))));
            }
            itemProject->setChild(tempi,4,new QStandardItem(xmlData[tempi].Comment));
            itemProject->setChild(tempi,5,new QStandardItem(QString::number(xmlData[tempi].DataLenth,10)));
        }
    }
}
int initxml::read_xmll(QString filename)
{
   QString QStrTemp;
   XmlDataNum = 0;
   QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        return 0;
    }
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn)) {
        return 0;
        file.close();
    }
    file.close();
    //////////////////////////
    QDomElement root = doc.documentElement();
    if (root.tagName() != "EtherCATMailbox") {
       qDebug()<<"root.tagname != EtherCATMailbox..." ;
    }
    QDomNode CoEroot = root.firstChild();
    while (!CoEroot.isNull())
    {
        if(CoEroot.isElement())
        {
           QDomNode cmdroot = CoEroot.firstChild();
           while(!cmdroot.isNull())
           {
               if(cmdroot.isElement())
               {
                   QDomNode node = cmdroot.firstChild();
                   while(!node.isNull())
                   {
                       if(node.isElement())
                       {
                           QDomElement element = node.toElement();
                           QDomNodeList Llist = element.childNodes();

                           for(int tempi = 0;tempi < Llist.count(); tempi ++)
                           {
                               QDomNode nodechild = Llist.at(tempi);
                               if(nodechild.isElement())
                               {
                                   if(nodechild.toElement().tagName() == "Transition")
                                   {
                                       xmlData[XmlDataNum].Trans = nodechild.toElement().text();

                                   }
                                   if(nodechild.toElement().tagName() == "Comment")
                                   {
                                       xmlData[XmlDataNum].Comment = nodechild.toElement().text();
                                   }
                                   if(nodechild.toElement().tagName() == "Timeout")
                                   {
                                       QStrTemp = nodechild.toElement().text();
                                       xmlData[XmlDataNum].Timeout = QStrTemp.toInt(0,16);
                                   }
                                   if(nodechild.toElement().tagName() == "Ccs")
                                   {
                                       QStrTemp = nodechild.toElement().text();
                                       xmlData[XmlDataNum].Ccs = QStrTemp.toInt();
                                   }
                                   if(nodechild.toElement().tagName() == "Index")
                                   {
                                       QStrTemp = nodechild.toElement().text();
                                       xmlData[XmlDataNum].Index = QStrTemp.toInt(0,10);
                                   }
                                   if(nodechild.toElement().tagName() == "SubIndex")
                                   {
                                       QStrTemp = nodechild.toElement().text();
                                       xmlData[XmlDataNum].SubIndex = QStrTemp.toInt(0,10);
                                   }
                                   if(nodechild.toElement().tagName() == "Data")
                                   {
                                       int8_t DataStrLen = 0;
                                       QStrTemp = nodechild.toElement().text();
                                       if(xmlData[XmlDataNum].Index == 8193 | (xmlData[XmlDataNum].Index == 11265 & xmlData[XmlDataNum].SubIndex == 10) | xmlData[XmlDataNum].Index == 25604)
                                       {
                                           QStrTemp = nodechild.toElement().text();
                                           for(int tempi = 0;tempi < QStrTemp.length()/2;tempi ++)
                                           {
                                               xmlData[XmlDataNum].DataStr[tempi] = QStrTemp.mid(2*tempi,2).toInt(0,16);
                                               DataStrLen++;
                                           }
                                           xmlData[XmlDataNum].DataLenth = DataStrLen;
                                           xmlData[XmlDataNum].IsChar = true;
                                       }
                                       else
                                       {
                                           QStrTemp = nodechild.toElement().text();
                                           xmlData[XmlDataNum].DataInt = QStrTemp.toUInt(0,16);
                                           xmlData[XmlDataNum].IsChar = false;
                                           xmlData[XmlDataNum].DataLenth = QStrTemp.length() / 2;//bype of number
                                       }
                                   }

                               }
                           }

                           XmlDataNum++;
                       }
                       node = node.nextSibling();
                   }
               }
               cmdroot = cmdroot.nextSibling();
           }

        }
        CoEroot = CoEroot.nextSibling();
    }
    return 1;
}

void initxml::on_modifyButton_clicked()
{
    qDebug() << itemProject->rowCount();
    for(int tempi = 0; tempi < 10; tempi++)
    {
        //qDebug() << itemProject->child(tempi);
        qDebug() << itemProject;


    }

}

void initxml::on_downloadButton_clicked()
{

}
