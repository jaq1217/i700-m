#ifndef INITXML_H
#define INITXML_H

#include <QDialog>
#include <QString>
#include <QStandardItem>

namespace Ui {
class initxml;
}

class initxml : public QDialog
{
    Q_OBJECT

public:
    explicit initxml(QWidget *parent = 0);
    ~initxml();
    int read_xmll(QString filename);
    //void do_xml(const QString opt,QString filename);

    typedef struct {
       QString         Trans;
       QString         Comment;
       uint8_t       Timeout;
       uint8_t       Ccs;
       uint16_t      Index;
       uint8_t      SubIndex;
        union
        {
            uint32_t      DataInt;
            uint8_t       DataStr[100];
        };
        bool        IsChar;
       uint8_t       DataLenth;
    }XmlDataStruct;
    XmlDataStruct xmlData[300];//120
    int16_t XmlDataNum;
    QString  filename;
    QStandardItemModel* model;
    QStandardItem* itemProject;

private slots:
    void on_loadButton_clicked();

    void on_modifyButton_clicked();

    void on_downloadButton_clicked();

private:
    Ui::initxml *ui;
};

#endif // INITXML_H
