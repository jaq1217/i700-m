#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "osal/osal.h"
#include "soem/ethercatmain.h"

#include "soem/ethercatconfig.h"
#include "soem/ethercatdc.h"
#include "ifaddrs.h"
#include "soem/ethercatcoe.h"
#include "soem/ethercattype.h"
#include "soem/ethercatprint.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString str;
    ui->setupUi(this);
    int width = this->geometry().width();
    int height = this->geometry().height();
    this->setFixedSize(width,height);
    timer0 = new QTimer(this);
    enable_timer  = new QTimer(this);
    disable_timer  = new QTimer(this);

    //Count = 0;
    MotorEnable = false;
    MotorEnable2A = false;
    MotorEnable2B = false;
    SendPdo = false;
    MotorStaCount = 0;
    MotorStaCount2A = 0;
    MotorStaCount2B = 0;
    MotorStatus = MOTOR_NOT_READY;
    MotorStatus2A = MOTOR_NOT_READY;
    MotorStatus2B = MOTOR_NOT_READY;
    TargetPosi = 0x0000;

}

MainWindow::~MainWindow()
{

    if(timer0->isActive())
    {
        timer0->stop();
    }
    if(enable_timer->isActive())
    {
        enable_timer->stop();
    }
    if(disable_timer->isActive())
    {
        disable_timer->stop();
    }
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    int nSM,nFMMU;
    int Osize,Isize,PDOassign;
    int chk = 0;
    int slave;
    QString str;
    connect(timer0,SIGNAL(timeout()),this,SLOT(UpStatus()));
    if(ec_init("ens33"))
    {
        ui->infoBrowser->setText("ec_init succeeded");
        if(ec_config_init(FALSE) > 0)
        {

            str.sprintf("%d slaves was found",ec_slavecount);
            ui->infoBrowser->append(str);

            ec_readstate();
            str.sprintf("state:%d,%d,%d",ec_slave[0].state,ec_slave[1].state,ec_slave[2].state);
            ui->infoBrowser->append(str);

            for(slave = 1;slave <= ec_slavecount;slave++)
            {
                if(ec_slave[slave].eep_id == SINGLE_CODE)
                {
                    Single_SDOWrite(slave);
                }
                else if(ec_slave[slave].eep_id == DOUBLE_CODE)
                {
                   Double_SDOWrite(slave);
                }
            }


            ec_config_map(&IOmap);
            ec_configdc();

            ec_readstate();
            str.sprintf("state:%d,%d,%d",ec_slave[0].state,ec_slave[1].state,ec_slave[2].state);
            ui->infoBrowser->append(str);

            for(slave = 1;slave <= ec_slavecount; slave++)
            {
                str.sprintf("Name:%s",ec_slave[slave].name);
                ui->infoBrowser->append(str);
            }
            ui->infoBrowser->append("");

           //ready to OP state
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            ec_writestate(0);
            chk = 100;
            do
            {
                 ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
            }while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

            ec_readstate();
            str.sprintf("state:%d,%d,%d",ec_slave[0].state,ec_slave[1].state,ec_slave[2].state);
            ui->infoBrowser->append(str);

            if(ec_slave[0].state == EC_STATE_OPERATIONAL)
            {
                str.sprintf("////////////OP State///////////////");
                ui->infoBrowser->append(str);
                SendPdo = true;
            }
            else
            {
                str.sprintf("////////////OP Fail///////////////");
                ui->infoBrowser->append(str);
            }

        }
    }
    timer0->start(50);

}
void MainWindow::Single_SDOWrite(int id)
{
    int os;
    uint32 ob1;
    int16 ob2;
    uint8 ob3;
    int tempi;
    uint16 tempH,tempL;
    uint32 tempHH,tempHL,tempLH,tempLL;
    unsigned int SDODATA1[SINGLE_CONGIG_NUM * 4]={
    4337,2,0x14000000,4,
    9472,0,0x0000,2,
    9473,0,0x00000000,4,
    9536,1,0x00,1,
    9536,2,0x2C01,2,
    9536,5,0x1B03,2,
    10272,1,0x01,1,
    10272,2,0x6400,2,
    10272,3,0x6400,2,
    10272,5,0x00,1,
    10272,6,0x01,1,
    10276,0,0x01,1,
    10277,0,0x00,1,
    10278,0,0x04000000,4,
    10294,3,0x0000,2,
    10294,4,0xF401,2,
    10294,5,0xC4090000,4,
    10294,6,0x4C130000,4,
    10294,7,0xA1000000,4,
    10336,1,0xE803,2,
    10336,2,0x3200,2,
    10496,1,0x4D010000,4,
    10496,2,0xB000,2,
    10496,3,0x0000,2,
    10497,0,0x1027,2,
    10498,0,0x0000,2,
    10499,0,0x0000,2,
    10500,0,0x0600,2,
    10512,1,0xAC000000,4,
    10512,2,0x00000000,4,
    10512,3,0x00,1,
    10553,0,0x02,1,
    10561,0,0x00,1,
    10562,1,0x4C130000,4,
    10562,2,0xA1000000,4,
    10563,0,0x0000,2,
    10564,1,0xD007,2,
    10564,2,0xC800,2,
    10564,3,0x00,1,
    10564,4,0xA00F,2,
    10564,5,0x9001,2,
    10564,6,0x00,1,
    10565,0,0xA00F,2,
    10566,1,0x00000000,4,
    10566,2,0x00000000,4,
    10567,1,0x0000,2,
    10567,2,0x0000,2,
    10567,3,0x0000,2,
    10567,4,0x0000,2,
    10567,5,0x0000,2,
    10567,6,0x0000,2,
    10567,7,0x0000,2,
    10567,8,0x0000,2,
    10567,9,0x0000,2,
    10567,10,0x0000,2,
    10567,11,0x0000,2,
    10567,12,0x0000,2,
    10567,13,0x0000,2,
    10567,14,0x0000,2,
    10567,15,0x0000,2,
    10567,16,0x0000,2,
    10567,17,0x0000,2,
    10624,0,0x180B0000,4,
    10625,0,0x1027,2,
    10626,0,0xFFFFFF7F,4,
    10627,0,0x00000000,4,
    10628,0,0x00,1,
    10688,1,0xC8400000,4,
    10688,2,0x9700,2,
    10720,1,0x00000000,4,
    10720,2,0x204E0000,4,
    10721,0,0x1027,2,
    10722,0,0xFA00,2,
    10723,0,0xFA00,2,
    10724,0,0x05,1,
    11008,0,0x00,1,
    11009,1,0x5E01,2,
    11009,2,0x9600,2,
    11010,1,0xCEFF,2,
    11010,2,0xD8FF,2,
    11010,3,0xE2FF,2,
    11010,4,0xECFF,2,
    11010,5,0xF6FF,2,
    11010,6,0x0000,2,
    11010,7,0x0A00,2,
    11010,8,0x1400,2,
    11010,9,0x1E00,2,
    11010,10,0x2800,2,
    11010,11,0x3200,2,
    11011,1,0x409C0000,4,
    11011,2,0x007D0000,4,
    11011,3,0xC05D0000,4,
    11011,4,0x803E0000,4,
    11011,5,0x401F0000,4,
    11011,6,0x00000000,4,
    11011,7,0x401F0000,4,
    11011,8,0x803E0000,4,
    11011,9,0xC05D0000,4,
    11011,10,0x007D0000,4,
    11011,11,0x409C0000,4,
    11012,0,0x41000000,4,
    11013,1,0x4C130000,4,
    11013,2,0xA1000000,4,
    11014,0,0x0000,2,
    11015,1,0x00,1,
    11015,2,0xD0070000,4,
    11016,1,0x01000000,4,
    11016,2,0xE8030000,4,
    11017,1,0x0000,2,
    11017,2,0xD007,2,
    11018,1,0x1400,2,
    11018,2,0x0500,2,
    11018,3,0x0200,2,
    11018,4,0x00,1,
    11020,0,0x0000,2,
    11136,0,0x0000,2,
    11137,0,0x0000,2,
    11138,0,0x00,1,
    11168,0,0x00,1,
    11169,0,0x0F00,2,
    11170,0,0xC800,2,
    11171,0,0x5802,2,
    11172,0,0xF401,2,
    11173,0,0x0000,2,
    11264,0,0x01,1,
    11265,2,0xE09F0200,4,
    11265,3,0x5C8F0000,4,
    11265,4,0xD007,2,
    11265,5,0x3205,2,
    11265,6,0x4300,2,
    11265,7,0x2101,2,
    11265,8,0x6200,2,
    11265,9,0x04,1,
    11266,1,0x00000000,4,
    11266,2,0x00000000,4,
    11266,3,0x0000,2,
    11267,1,0xA2010000,4,
    11267,2,0x7CFC,2,
    11267,3,0x92FF,2,
    11267,4,0x0000,2,
    11268,1,0x6400,2,
    11268,2,0x6400,2,
    11268,3,0x6400,2,
    11268,4,0x6400,2,
    11268,5,0x6400,2,
    11268,6,0x6400,2,
    11268,7,0x6400,2,
    11268,8,0x6400,2,
    11268,9,0x6400,2,
    11268,10,0x6400,2,
    11268,11,0x6400,2,
    11268,12,0x6400,2,
    11268,13,0x6400,2,
    11268,14,0x6400,2,
    11268,15,0x6400,2,
    11268,16,0x6400,2,
    11268,17,0x6400,2,
    11268,18,0x0100,2,
    11269,0,0x3800,2,
    11270,1,0x0000,2,
    11270,2,0x6400,2,
    11270,3,0x6400,2,
    11270,4,0x6400,2,
    11270,5,0xC800,2,
    11270,6,0x6400,2,
    11270,7,0x9F01,2,
    11270,8,0x4800,2,
    11271,1,0x7600,2,
    11271,2,0x7600,2,
    11271,3,0x7600,2,
    11271,4,0x7500,2,
    11271,5,0x7400,2,
    11271,6,0x7200,2,
    11271,7,0x6F00,2,
    11271,8,0x6B00,2,
    11271,9,0x6400,2,
    11271,10,0x5D00,2,
    11271,11,0x5600,2,
    11271,12,0x4E00,2,
    11271,13,0x4700,2,
    11271,14,0x4000,2,
    11271,15,0x3900,2,
    11271,16,0x3200,2,
    11271,17,0x2A00,2,
    11272,0,0x03,1,
    11280,1,0xF401,2,
    11280,2,0x401F,2,
    11280,3,0xF401,2,
    11280,4,0x0000,2,
    11280,5,0x0000,2,
    11280,6,0x0000,2,
    11280,7,0x0500,2,
    11281,1,0x1400,2,
    11281,2,0xFA00,2,
    11281,3,0x4A01,2,
    11281,4,0x0000,2,
    11328,0,0x01,1,
    11329,2,0x00,1,
    11329,3,0x0100,2,
    11329,4,0x01,1,
    11330,1,0x00040000,4,
    11330,2,0x32,1,
    11331,0,0x01,1,
    11332,1,0x0000,2,
    11332,2,0x6400,2,
    11332,3,0x6400,2,
    11333,0,0x01,1,
    11335,0,0x64,1,
    11360,0,0x01,1,
    11361,1,0x6400,2,
    11361,2,0x2800,2,
    11361,3,0x00,1,
    11361,4,0x14,1,
    11362,1,0x1900,2,
    11362,2,0x0A00,2,
    11362,3,0x0000,2,
    11362,4,0x7102,2,
    11362,5,0x14,1,
    11363,1,0x00,1,
    11520,1,0x0000,2,
    11520,2,0x0000,2,
    11584,2,0x5F00,2,
    11584,5,0x5F00,2,
    11588,1,0x7C15,2,
    11588,2,0x01,1,
    11589,1,0x00,1,
    11589,2,0x32,1,
    11589,3,0x6400,2,
    11589,4,0x00,1,
    11590,1,0x3800,2,
    11590,2,0x01,1,
    11593,1,0x00,1,
    11593,2,0x01,1,
    11593,3,0xAA05,2,
    11593,4,0x0E06,2,
    11593,6,0xFA00,2,
    11593,7,0xDC05,2,
    11593,8,0xE803,2,
    11593,9,0xB108,2,
    11596,1,0x5A00,2,
    11596,2,0xB607,2,
    11596,3,0x1B,1,
    11596,4,0x0000,2,
    11597,1,0x0000,2,
    11597,2,0x6000,2,
    11597,3,0x0000,2,
    11597,4,0x6000,2,
    11597,5,0x6400,2,
    11597,6,0x6400,2,
    11597,7,0x6400,2,
    11597,8,0x6400,2,
    11598,0,0x6400,2,
    11600,1,0x00,1,
    11600,2,0x6900,2,
    11652,2,0x2003,2,
    11652,3,0x2602,2,
    11652,4,0xC201,2,
    11744,5,0x01,1,
    24648,1,0x00000000,4,
    24648,2,0x0A00,2,
    24649,1,0x00000000,4,
    24649,2,0x0A00,2,
    24666,0,0x0200,2,
    24670,0,0xFEFF,2,
    24672,0,0x00,1,
    24677,0,0xE8030000,4,
    24678,0,0x0000,2,
    24679,0,0xE8030000,4,
    24680,0,0x0000,2,
    24690,0,0xC409,2,
    24691,0,0xDC05,2,
    24693,0,0x9A060000,4,
    24694,0,0x800C0000,4,
    24702,0,0x00,1,
    24704,0,0x94110000,4,
    24709,0,0xFFFFFF7F,4,
    24719,1,0x00000100,4,
    24719,2,0x01000000,4,
    24720,1,0x00000002,4,
    24720,2,0x7D000000,4,
    24753,0,0x00000000,4,
    24754,0,0x0000,2,
    24760,0,0x0000,2,
    24768,0,0x0000,2,
    24770,1,0x02,1,
    24770,2,0xFD,1,
    24800,0,0x0000,2,
    24801,0,0x0000,2
    };

    //clear
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1c12,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1c13,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1a06,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//??data from twincat is different with wireshark
    //config
    os=sizeof(ob1); ob1 = 0x28330010;
    ec_SDOwrite(id,0x1a06,01,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob1); ob1 = 0x60fd0020;
    ec_SDOwrite(id,0x1a06,02,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0x02;
    ec_SDOwrite(id,0x1a06,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//??data from twincat is different with wireshark
    os=sizeof(ob2); ob2 = 0x1600;
    ec_SDOwrite(id,0x1c12,01,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1604;
    ec_SDOwrite(id,0x1c12,02,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1606;
    ec_SDOwrite(id,0x1c12,03,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1607;
    ec_SDOwrite(id,0x1c12,04,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0x04;
    ec_SDOwrite(id,0x1c12,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1a00;
    ec_SDOwrite(id,0x1c13,01,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1a04;
    ec_SDOwrite(id,0x1c13,02,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1a06;
    ec_SDOwrite(id,0x1c13,03,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0x03;
    ec_SDOwrite(id,0x1c13,00,FALSE,os,&ob2,EC_TIMEOUTRXM);

    for(tempi=0;tempi<SINGLE_CONGIG_NUM;tempi++)
    {
        if(SDODATA1[tempi*4+3] == 4)
        {
            tempHH = (SDODATA1[tempi*4+2] & 0x000000ff) << 24;
            tempHL = (SDODATA1[tempi*4+2] & 0x0000ff00) << 8;
            tempLH = (SDODATA1[tempi*4+2] & 0x00ff0000) >> 8 ;
            tempLL = (SDODATA1[tempi*4+2] & 0xff000000) >> 24;
            SDODATA1[tempi*4+2] = tempHH | tempHL | tempLH | tempLL;
            os=sizeof(ob1); ob1 = SDODATA1[tempi*4+2];
            ec_SDOwrite(id,SDODATA1[tempi*4],SDODATA1[tempi*4+1],FALSE,os,&ob1,4*EC_TIMEOUTRXM);
        }
        else if(SDODATA1[tempi*4+3] == 2)
        {
            tempH = (SDODATA1[tempi*4+2] & 0x00ff) << 8;
            tempL = (SDODATA1[tempi*4+2] & 0xff00) >> 8;
            SDODATA1[tempi*4+2] = tempH | tempL;
            os=sizeof(ob2); ob2 = SDODATA1[tempi*4+2];
            ec_SDOwrite(id,SDODATA1[tempi*4],SDODATA1[tempi*4+1],FALSE,os,&ob2,4*EC_TIMEOUTRXM);
        }
        else if(SDODATA1[tempi*4+3] == 1)
        {
            os=sizeof(ob3); ob3 = SDODATA1[tempi*4+2];
            ec_SDOwrite(id,SDODATA1[tempi*4],SDODATA1[tempi*4+1],FALSE,os,&ob3,4*EC_TIMEOUTRXM);
        }
        else
        {
            //nothing
        }
    }

}
void MainWindow::Double_SDOWrite(int id)
{
    int os;
    uint32 ob1;
    int16 ob2;
    uint8 ob3;
    int tempi;
    uint16 tempH,tempL;
    uint32 tempHH,tempHL,tempLH,tempLL;
    unsigned int SDODATA2[DOUBLE_CONGIG_NUM * 4]={
    4337,2,0x14000000,4,
    9472,0,0x0000,2,
    9473,0,0x00000000,4,
    9536,1,0x00,1,
    9536,5,0x1B03,2,
    9536,2,0x2C01,2,
    10272,5,0x00,1,
    10272,10,0x0000,2,
    10272,1,0x01,1,
    10272,6,0x01,1,
    10272,9,0x0100,2,
    10272,2,0x6400,2,
    10272,3,0x6400,2,
    10276,0,0x01,1,
    10277,0,0x00,1,
    10278,0,0x04000000,4,
    10294,3,0x0000,2,
    10294,6,0x4C130000,4,
    10294,7,0xA1000000,4,
    10294,5,0xC4090000,4,
    10294,4,0xF401,2,
    10336,2,0x3200,2,
    10336,1,0xE803,2,
    10496,3,0x0000,2,
    10496,1,0x4D010000,4,
    10496,2,0xB000,2,
    10497,0,0x1027,2,
    10498,0,0x0000,2,
    10499,0,0x0000,2,
    10500,0,0x0600,2,
    10512,3,0x00,1,
    10512,2,0x00000000,4,
    10512,1,0xAC000000,4,
    10553,0,0x02,1,
    10561,0,0x00,1,
    10562,1,0x4C130000,4,
    10562,2,0xA1000000,4,
    10563,0,0x0000,2,
    10564,3,0x00,1,
    10564,6,0x00,1,
    10564,5,0x9001,2,
    10564,4,0xA00F,2,
    10564,2,0xC800,2,
    10564,1,0xD007,2,
    10565,0,0xA00F,2,
    10566,1,0x00000000,4,
    10566,2,0x00000000,4,
    10567,1,0x0000,2,
    10567,2,0x0000,2,
    10567,3,0x0000,2,
    10567,4,0x0000,2,
    10567,5,0x0000,2,
    10567,6,0x0000,2,
    10567,7,0x0000,2,
    10567,8,0x0000,2,
    10567,9,0x0000,2,
    10567,10,0x0000,2,
    10567,11,0x0000,2,
    10567,12,0x0000,2,
    10567,13,0x0000,2,
    10567,14,0x0000,2,
    10567,15,0x0000,2,
    10567,16,0x0000,2,
    10567,17,0x0000,2,
    10624,0,0x180B0000,4,
    10625,0,0x1027,2,
    10626,0,0xFFFFFF7F,4,
    10627,0,0x00000000,4,
    10628,0,0x00,1,
    10688,2,0x9700,2,
    10688,1,0xC8400000,4,
    10720,1,0x00000000,4,
    10720,2,0x204E0000,4,
    10721,0,0x1027,2,
    10722,0,0xFA00,2,
    10723,0,0xFA00,2,
    10724,0,0x05,1,
    11008,0,0x00,1,
    11009,1,0x5E01,2,
    11009,2,0x9600,2,
    11010,6,0x0000,2,
    11010,7,0x0A00,2,
    11010,8,0x1400,2,
    11010,9,0x1E00,2,
    11010,10,0x2800,2,
    11010,11,0x3200,2,
    11010,1,0xCEFF,2,
    11010,2,0xD8FF,2,
    11010,3,0xE2FF,2,
    11010,4,0xECFF,2,
    11010,5,0xF6FF,2,
    11011,6,0x00000000,4,
    11011,2,0x007D0000,4,
    11011,10,0x007D0000,4,
    11011,5,0x401F0000,4,
    11011,7,0x401F0000,4,
    11011,1,0x409C0000,4,
    11011,11,0x409C0000,4,
    11011,4,0x803E0000,4,
    11011,8,0x803E0000,4,
    11011,3,0xC05D0000,4,
    11011,9,0xC05D0000,4,
    11012,0,0x41000000,4,
    11013,1,0x4C130000,4,
    11013,2,0xA1000000,4,
    11014,0,0x0000,2,
    11015,1,0x00,1,
    11015,2,0xD0070000,4,
    11016,1,0x01000000,4,
    11016,2,0xE8030000,4,
    11017,1,0x0000,2,
    11017,2,0xD007,2,
    11018,4,0x00,1,
    11018,3,0x0200,2,
    11018,2,0x0500,2,
    11018,1,0x1400,2,
    11020,0,0x0000,2,
    11136,0,0x0000,2,
    11137,0,0x0000,2,
    11138,0,0x00,1,
    11168,0,0x00,1,
    11169,0,0x0F00,2,
    11170,0,0xC800,2,
    11171,0,0x5802,2,
    11172,0,0xF401,2,
    11173,0,0x0000,2,
    11264,0,0x01,1,
    11265,9,0x04,1,
    11265,7,0x2101,2,
    11265,5,0x3205,2,
    11265,6,0x4300,2,
    11265,3,0x5C8F0000,4,
    11265,8,0x6200,2,
    11265,4,0xD007,2,
    11265,2,0xE09F0200,4,
    11266,3,0x0000,2,
    11266,1,0x00000000,4,
    11266,2,0x00000000,4,
    11267,4,0x0000,2,
    11267,2,0x7CFC,2,
    11267,3,0x92FF,2,
    11267,1,0xA2010000,4,
    11268,18,0x0100,2,
    11268,1,0x6400,2,
    11268,2,0x6400,2,
    11268,3,0x6400,2,
    11268,4,0x6400,2,
    11268,5,0x6400,2,
    11268,6,0x6400,2,
    11268,7,0x6400,2,
    11268,8,0x6400,2,
    11268,9,0x6400,2,
    11268,10,0x6400,2,
    11268,11,0x6400,2,
    11268,12,0x6400,2,
    11268,13,0x6400,2,
    11268,14,0x6400,2,
    11268,15,0x6400,2,
    11268,16,0x6400,2,
    11268,17,0x6400,2,
    11269,0,0x3800,2,
    11270,1,0x0000,2,
    11270,8,0x4800,2,
    11270,2,0x6400,2,
    11270,3,0x6400,2,
    11270,4,0x6400,2,
    11270,6,0x6400,2,
    11270,7,0x9F01,2,
    11270,5,0xC800,2,
    11271,17,0x2A00,2,
    11271,16,0x3200,2,
    11271,15,0x3900,2,
    11271,14,0x4000,2,
    11271,13,0x4700,2,
    11271,12,0x4E00,2,
    11271,11,0x5600,2,
    11271,10,0x5D00,2,
    11271,9,0x6400,2,
    11271,8,0x6B00,2,
    11271,7,0x6F00,2,
    11271,6,0x7200,2,
    11271,5,0x7400,2,
    11271,4,0x7500,2,
    11271,1,0x7600,2,
    11271,2,0x7600,2,
    11271,3,0x7600,2,
    11272,0,0x03,1,
    11280,4,0x0000,2,
    11280,5,0x0000,2,
    11280,6,0x0000,2,
    11280,7,0x0500,2,
    11280,2,0x401F,2,
    11280,1,0xF401,2,
    11280,3,0xF401,2,
    11281,4,0x0000,2,
    11281,1,0x1400,2,
    11281,3,0x4A01,2,
    11281,2,0xFA00,2,
    11282,2,0x1400,2,
    11282,1,0x6400,2,
    11283,0,0x01,1,
    11328,0,0x01,1,
    11329,2,0x00,1,
    11329,4,0x01,1,
    11329,3,0x0100,2,
    11330,1,0x00040000,4,
    11330,2,0x32,1,
    11331,0,0x01,1,
    11332,1,0x0000,2,
    11332,2,0x6400,2,
    11332,3,0x6400,2,
    11333,0,0x01,1,
    11335,0,0x64,1,
    11360,0,0x01,1,
    11361,3,0x00,1,
    11361,4,0x14,1,
    11361,2,0x2800,2,
    11361,1,0x6400,2,
    11362,3,0x0000,2,
    11362,2,0x0A00,2,
    11362,5,0x14,1,
    11362,1,0x1900,2,
    11362,4,0x7102,2,
    11363,1,0x00,1,
    11520,1,0x0000,2,
    11520,2,0x0000,2,
    11584,2,0x5F00,2,
    11584,5,0x5F00,2,
    11588,2,0x01,1,
    11588,1,0x7C15,2,
    11589,1,0x00,1,
    11589,4,0x00,1,
    11589,2,0x32,1,
    11589,3,0x6400,2,
    11590,2,0x01,1,
    11590,1,0x3800,2,
    11593,1,0x00,1,
    11593,2,0x01,1,
    11593,4,0x0E06,2,
    11593,3,0xAA05,2,
    11593,9,0xB108,2,
    11593,7,0xDC05,2,
    11593,8,0xE803,2,
    11593,6,0xFA00,2,
    11596,4,0x0000,2,
    11596,3,0x1B,1,
    11596,1,0x5A00,2,
    11596,2,0xB607,2,
    11597,1,0x0000,2,
    11597,3,0x0000,2,
    11597,2,0x6000,2,
    11597,4,0x6000,2,
    11597,5,0x6400,2,
    11597,6,0x6400,2,
    11597,7,0x6400,2,
    11597,8,0x6400,2,
    11598,0,0x6400,2,
    11600,1,0x00,1,
    11600,2,0x6900,2,
    11601,3,0x00,1,
    11601,6,0x00,1,
    11601,2,0x0000,2,
    11601,5,0x0000,2,
    11601,1,0x32000000,4,
    11601,4,0x68010000,4,
    11652,2,0x2003,2,
    11652,3,0x2602,2,
    11652,4,0xC201,2,
    11744,5,0x01,1,
    12320,5,0x00,1,
    12320,10,0x0000,2,
    12320,1,0x01,1,
    12320,6,0x01,1,
    12320,9,0x0100,2,
    12320,2,0x6400,2,
    12320,3,0x6400,2,
    12324,0,0x01,1,
    12325,0,0x00,1,
    12326,0,0x04000000,4,
    12342,3,0x0000,2,
    12342,5,0xC4090000,4,
    12342,6,0xD0070000,4,
    12342,7,0xD0070000,4,
    12342,4,0xF401,2,
    12384,2,0x3200,2,
    12384,1,0xE803,2,
    12544,3,0x0000,2,
    12544,1,0x21000000,4,
    12544,2,0xB000,2,
    12545,0,0x1027,2,
    12546,0,0x0000,2,
    12547,0,0x0000,2,
    12548,0,0x0600,2,
    12560,3,0x00,1,
    12560,2,0x00000000,4,
    12560,1,0x0E000000,4,
    12601,0,0x02,1,
    12609,0,0x00,1,
    12610,2,0x79010000,4,
    12610,1,0xE5390000,4,
    12611,0,0x0000,2,
    12612,3,0x00,1,
    12612,6,0x00,1,
    12612,5,0x9001,2,
    12612,4,0xA00F,2,
    12612,2,0xC800,2,
    12612,1,0xD007,2,
    12613,0,0xA00F,2,
    12614,1,0x00000000,4,
    12614,2,0x00000000,4,
    12615,1,0x0000,2,
    12615,2,0x0000,2,
    12615,3,0x0000,2,
    12615,4,0x0000,2,
    12615,5,0x0000,2,
    12615,6,0x0000,2,
    12615,7,0x0000,2,
    12615,8,0x0000,2,
    12615,9,0x0000,2,
    12615,10,0x0000,2,
    12615,11,0x0000,2,
    12615,12,0x0000,2,
    12615,13,0x0000,2,
    12615,14,0x0000,2,
    12615,15,0x0000,2,
    12615,16,0x0000,2,
    12615,17,0x0000,2,
    12672,0,0x180B0000,4,
    12673,0,0x1027,2,
    12674,0,0xFFFFFF7F,4,
    12675,0,0x00000000,4,
    12676,0,0x00,1,
    12736,2,0x9700,2,
    12736,1,0xC8400000,4,
    12768,1,0x00000000,4,
    12768,2,0x204E0000,4,
    12769,0,0x1027,2,
    12770,0,0xFA00,2,
    12771,0,0xFA00,2,
    12772,0,0x05,1,
    13056,0,0x00,1,
    13057,2,0x0E01,2,
    13057,1,0xE100,2,
    13058,6,0x0000,2,
    13058,7,0x0A00,2,
    13058,8,0x1400,2,
    13058,9,0x1E00,2,
    13058,10,0x2800,2,
    13058,11,0x3200,2,
    13058,1,0xCEFF,2,
    13058,2,0xD8FF,2,
    13058,3,0xE2FF,2,
    13058,4,0xECFF,2,
    13058,5,0xF6FF,2,
    13059,6,0x00000000,4,
    13059,2,0x007D0000,4,
    13059,10,0x007D0000,4,
    13059,5,0x401F0000,4,
    13059,7,0x401F0000,4,
    13059,1,0x409C0000,4,
    13059,11,0x409C0000,4,
    13059,4,0x803E0000,4,
    13059,8,0x803E0000,4,
    13059,3,0xC05D0000,4,
    13059,9,0xC05D0000,4,
    13060,0,0x00000000,4,
    13061,2,0x79010000,4,
    13061,1,0xE5390000,4,
    13062,0,0x0000,2,
    13063,1,0x00,1,
    13063,2,0xD0070000,4,
    13064,1,0x01000000,4,
    13064,2,0xE8030000,4,
    13065,1,0x0000,2,
    13065,2,0xD007,2,
    13066,4,0x00,1,
    13066,3,0x0200,2,
    13066,2,0x0500,2,
    13066,1,0x1400,2,
    13068,0,0x0000,2,
    13184,0,0x0000,2,
    13185,0,0x0000,2,
    13186,0,0x00,1,
    13216,0,0x00,1,
    13217,0,0x0F00,2,
    13218,0,0xC800,2,
    13219,0,0x5802,2,
    13220,0,0xF401,2,
    13221,0,0x0000,2,
    13312,0,0x01,1,
    13313,9,0x04,1,
    13313,6,0x1900,2,
    13313,3,0x38C70000,4,
    13313,8,0x5000,2,
    13313,2,0x580F0200,4,
    13313,5,0x8C0A,2,
    13313,4,0xD20F,2,
    13313,7,0xE100,2,
    13314,3,0x0000,2,
    13314,1,0x00000000,4,
    13314,2,0x00000000,4,
    13315,4,0x0000,2,
    13315,2,0x7CFC,2,
    13315,3,0x92FF,2,
    13315,1,0xA2010000,4,
    13316,18,0x0100,2,
    13316,14,0x2900,2,
    13316,15,0x2900,2,
    13316,16,0x2900,2,
    13316,17,0x2900,2,
    13316,13,0x2A00,2,
    13316,12,0x2B00,2,
    13316,11,0x2E00,2,
    13316,10,0x3200,2,
    13316,9,0x3700,2,
    13316,8,0x3E00,2,
    13316,7,0x4700,2,
    13316,6,0x5200,2,
    13316,5,0x6100,2,
    13316,4,0x7500,2,
    13316,3,0x9200,2,
    13316,1,0xA500,2,
    13316,2,0xC800,2,
    13317,0,0x3600,2,
    13318,1,0x0000,2,
    13318,8,0x4800,2,
    13318,2,0x6400,2,
    13318,3,0x6400,2,
    13318,4,0x6400,2,
    13318,6,0x6400,2,
    13318,7,0x9F01,2,
    13318,5,0xC800,2,
    13319,17,0x2A00,2,
    13319,16,0x3200,2,
    13319,15,0x3900,2,
    13319,14,0x4000,2,
    13319,13,0x4700,2,
    13319,12,0x4E00,2,
    13319,11,0x5600,2,
    13319,10,0x5D00,2,
    13319,9,0x6400,2,
    13319,8,0x6B00,2,
    13319,7,0x6F00,2,
    13319,6,0x7200,2,
    13319,5,0x7400,2,
    13319,4,0x7500,2,
    13319,1,0x7600,2,
    13319,2,0x7600,2,
    13319,3,0x7600,2,
    13320,0,0x01,1,
    13328,4,0x0000,2,
    13328,5,0x0000,2,
    13328,6,0x0000,2,
    13328,7,0x0500,2,
    13328,2,0x401F,2,
    13328,1,0xF401,2,
    13328,3,0xF401,2,
    13329,4,0x0000,2,
    13329,1,0x1400,2,
    13329,3,0x4A01,2,
    13329,2,0xFA00,2,
    13330,2,0x1400,2,
    13330,1,0x6400,2,
    13331,0,0x01,1,
    13376,0,0x01,1,
    13377,2,0x00,1,
    13377,4,0x01,1,
    13377,3,0x0100,2,
    13378,1,0x00040000,4,
    13378,2,0x32,1,
    13379,0,0x01,1,
    13380,1,0x0000,2,
    13380,2,0x6400,2,
    13380,3,0x6400,2,
    13381,0,0x01,1,
    13383,0,0x64,1,
    13408,0,0x01,1,
    13409,3,0x00,1,
    13409,4,0x14,1,
    13409,2,0x2800,2,
    13409,1,0x6400,2,
    13410,3,0x0000,2,
    13410,2,0x0A00,2,
    13410,5,0x14,1,
    13410,1,0x1900,2,
    13410,4,0x7102,2,
    13411,1,0x00,1,
    13568,1,0x0000,2,
    13568,2,0x0000,2,
    13632,2,0x5F00,2,
    13632,5,0x5F00,2,
    13636,2,0x01,1,
    13636,1,0x401F,2,
    13637,1,0x00,1,
    13637,4,0x00,1,
    13637,2,0x32,1,
    13637,3,0x6400,2,
    13638,2,0x01,1,
    13638,1,0x3600,2,
    13641,1,0x00,1,
    13641,2,0x01,1,
    13641,4,0x0E06,2,
    13641,3,0xAA05,2,
    13641,9,0xB108,2,
    13641,7,0xDC05,2,
    13641,8,0xE803,2,
    13641,6,0xFA00,2,
    13644,4,0x0000,2,
    13644,3,0x1B,1,
    13644,1,0x3C00,2,
    13644,2,0x5403,2,
    13645,1,0x0000,2,
    13645,3,0x0000,2,
    13645,2,0x6400,2,
    13645,4,0x6400,2,
    13645,5,0x6400,2,
    13645,6,0x6400,2,
    13645,7,0x6400,2,
    13645,8,0x6400,2,
    13646,0,0x6400,2,
    13648,1,0x00,1,
    13648,2,0x6900,2,
    13649,3,0x00,1,
    13649,6,0x00,1,
    13649,2,0x0000,2,
    13649,5,0x0000,2,
    13649,1,0x32000000,4,
    13649,4,0x68010000,4,
    13700,2,0x2003,2,
    13700,3,0x2602,2,
    13700,4,0xC201,2,
    13792,5,0x01,1,
    24648,1,0x00000000,4,
    24648,2,0x0A00,2,
    24649,1,0x00000000,4,
    24649,2,0x0A00,2,
    24666,0,0x0200,2,
    24670,0,0xFEFF,2,
    24672,0,0x00,1,
    24677,0,0xE8030000,4,
    24678,0,0x0000,2,
    24679,0,0xE8030000,4,
    24680,0,0x0000,2,
    24690,0,0xC409,2,
    24691,0,0xDC05,2,
    24693,0,0x9A060000,4,
    24694,0,0x800C0000,4,
    24702,0,0x00,1,
    24704,0,0x94110000,4,
    24709,0,0xFFFFFF7F,4,
    24719,1,0x00000100,4,
    24719,2,0x01000000,4,
    24720,1,0x00000002,4,
    24720,2,0x7D000000,4,
    24753,0,0x00000000,4,
    24754,0,0x0000,2,
    24760,0,0x0000,2,
    24768,0,0x0000,2,
    24770,1,0x01,1,
    24770,2,0xFD,1,
    24800,0,0x0000,2,
    24801,0,0x0000,2,
    26696,1,0x00000000,4,
    26696,2,0x0A00,2,
    26697,1,0x00000000,4,
    26697,2,0x0A00,2,
    26714,0,0x0200,2,
    26718,0,0xFEFF,2,
    26720,0,0x00,1,
    26725,0,0xE8030000,4,
    26726,0,0x0000,2,
    26727,0,0xE8030000,4,
    26728,0,0x0000,2,
    26738,0,0xC409,2,
    26739,0,0xDC05,2,
    26741,0,0x14050000,4,
    26742,0,0x58020000,4,
    26750,0,0x00,1,
    26752,0,0xBB170000,4,
    26757,0,0xFFFFFF7F,4,
    26767,1,0x00000100,4,
    26767,2,0x01000000,4,
    26768,1,0x00000002,4,
    26768,2,0x7D000000,4,
    26801,0,0x00000000,4,
    26802,0,0x0000,2,
    26808,0,0x0000,2,
    26816,0,0x0000,2,
    26818,1,0x01,1,
    26818,2,0xFD,1,
    26848,0,0x0000,2,
    26849,0,0x0000,2
    };
    //clear sm
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1c12,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1c13,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
    //clear and config axis A
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1a06,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//??clear pdo 0x1a06
    os=sizeof(ob1); ob1 = 0x28330010;
    ec_SDOwrite(id,0x1a06,01,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob1); ob1 = 0x60fd0020;
    ec_SDOwrite(id,0x1a06,02,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0x02;
    ec_SDOwrite(id,0x1a06,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//?? download pdo 0x1a06 count
    //clear and config axis B
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1a16,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//??clear pdo 0x1a16
    os=sizeof(ob1); ob1 = 0x30330010;
    ec_SDOwrite(id,0x1a16,01,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob1); ob1 = 0x68fd0020;
    ec_SDOwrite(id,0x1a16,02,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0x02;
    ec_SDOwrite(id,0x1a16,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//??download pdo 0x1a16 count
    //download pdo 0x1c12
    os=sizeof(ob2); ob2 = 0x1600;
    ec_SDOwrite(id,0x1c12,01,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1604;
    ec_SDOwrite(id,0x1c12,02,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1606;
    ec_SDOwrite(id,0x1c12,03,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1607;
    ec_SDOwrite(id,0x1c12,04,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1610;
    ec_SDOwrite(id,0x1c12,05,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1614;
    ec_SDOwrite(id,0x1c12,06,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1616;
    ec_SDOwrite(id,0x1c12,07,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1617;
    ec_SDOwrite(id,0x1c12,8,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0x08;
    ec_SDOwrite(id,0x1c12,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
    //download pdo 0x1c13
    os=sizeof(ob2); ob2 = 0x1a00;
    ec_SDOwrite(id,0x1c13,01,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1a04;
    ec_SDOwrite(id,0x1c13,02,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1a06;
    ec_SDOwrite(id,0x1c13,03,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1a10;
    ec_SDOwrite(id,0x1c13,04,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1a14;
    ec_SDOwrite(id,0x1c13,05,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob2); ob2 = 0x1a16;
    ec_SDOwrite(id,0x1c13,06,FALSE,os,&ob2,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0x06;
    ec_SDOwrite(id,0x1c13,00,FALSE,os,&ob3,EC_TIMEOUTRXM);

    for(tempi=0;tempi<DOUBLE_CONGIG_NUM;tempi++)////////////////jaq???
    {
        if(SDODATA2[tempi*4+3] == 4)
        {
            tempHH = (SDODATA2[tempi*4+2] & 0x000000ff) << 24;
            tempHL = (SDODATA2[tempi*4+2] & 0x0000ff00) << 8;
            tempLH = (SDODATA2[tempi*4+2] & 0x00ff0000) >> 8 ;
            tempLL = (SDODATA2[tempi*4+2] & 0xff000000) >> 24;
            SDODATA2[tempi*4+2] = tempHH | tempHL | tempLH | tempLL;
            os=sizeof(ob1); ob1 = SDODATA2[tempi*4+2];
            ec_SDOwrite(id,SDODATA2[tempi*4],SDODATA2[tempi*4+1],FALSE,os,&ob1,4*EC_TIMEOUTRXM);
        }
        else if(SDODATA2[tempi*4+3] == 2)
        {
            tempH = (SDODATA2[tempi*4+2] & 0x00ff) << 8;
            tempL = (SDODATA2[tempi*4+2] & 0xff00) >> 8;
            SDODATA2[tempi*4+2] = tempH | tempL;
            os=sizeof(ob2); ob2 = SDODATA2[tempi*4+2];
            ec_SDOwrite(id,SDODATA2[tempi*4],SDODATA2[tempi*4+1],FALSE,os,&ob2,4*EC_TIMEOUTRXM);
        }
        else if(SDODATA2[tempi*4+3] == 1)
        {
            os=sizeof(ob3); ob3 = SDODATA2[tempi*4+2];
            ec_SDOwrite(id,SDODATA2[tempi*4],SDODATA2[tempi*4+1],FALSE,os,&ob3,4*EC_TIMEOUTRXM);
        }
        else
        {
            //nothing
        }
    }


}

void MainWindow:: UpStatus()
{
    QString str,sta;
    int p,psize,oloop,iloop,expectedWKC,Tempj,wkc;
    int TempData = 0;
    int slave;
    ec_errort Ec;
    //Count++;
    //ec_readstate();
    if(ec_slave[0].state == EC_STATE_INIT)
    {
        sta.sprintf("Init");

    }
    else if(ec_slave[0].state == EC_STATE_PRE_OP)
    {
        sta.sprintf("Pre-Op");
    }
    else if(ec_slave[0].state == EC_STATE_BOOT)
    {
        sta.sprintf("BOOT");
    }
    else if(ec_slave[0].state == EC_STATE_OPERATIONAL)
    {
        sta.sprintf("Operational");
    }
    else if(ec_slave[0].state == EC_STATE_SAFE_OP)
    {
        sta.sprintf("Safe-Op");
    }
    else if(ec_slave[1].state == EC_STATE_ACK)
    {
        sta.sprintf("ACK");
    }
    else if(ec_slave[0].state == EC_STATE_ERROR)
    {
        sta.sprintf("Error");
    }
    else
    {
        sta.sprintf("%d",ec_slave[0].state);
    }

    ui->currentlabel->setText(sta);

    if(EcatError)
    {
        str.sprintf("%s:",ec_elist2string());
        ui->infoBrowser->append(str);
    }
    else
    {
        //nothing
    }
    if(SendPdo && ec_slave[0].state == EC_STATE_OPERATIONAL)
    {
        //i700-single ,ECATDIO-I16-O16
        //i700
        OutputData.TargetPosi = TargetPosi;
        *(ec_slave[1].outputs) = OutputData.ControlWord & 0x00ff;
        *(ec_slave[1].outputs + 1 ) = (OutputData.ControlWord & 0xff00) >> 8;
        *(ec_slave[1].outputs + 4 ) = OutputData.ModeCmd;
        *(ec_slave[1].outputs + 7 ) = OutputData.TargetPosi & 0x000000ff;
        *(ec_slave[1].outputs + 8 ) = (OutputData.TargetPosi & 0x0000ff00) >> 8;
        *(ec_slave[1].outputs + 9 ) = (OutputData.TargetPosi & 0x00ff0000) >> 16;
        *(ec_slave[1].outputs + 10 ) = (OutputData.TargetPosi & 0xff000000) >> 24;
        *(ec_slave[1].outputs + 19) = OutputData.PosTorLim & 0x00ff;
        *(ec_slave[1].outputs + 20 ) = (OutputData.PosTorLim & 0xff00) >> 8;
        *(ec_slave[1].outputs + 21) = OutputData.NegTorLim & 0x00ff;
        *(ec_slave[1].outputs + 22 ) = (OutputData.NegTorLim & 0xff00) >> 8;

        //ECATDIO-I16-O16
        *(ec_slave[2].outputs) = 0xaa;
        *(ec_slave[2].outputs +1) = 0xbb;

        ui->infoBrowser->append(str);
        ec_send_processdata();
        wkc = ec_receive_processdata(EC_TIMEOUTRET);
        expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;


        //i700-single
        InputData.StatusWord = 0x0000;
        InputData.LenzeStatus = 0x0000;
        InputData.ModeStatus = 0x00;
        InputData.ErrorCode = 0x0000;
        InputData.VelValue = 0x00000000;
        InputData.TorValue = 0x0000;
        InputData.PosiValue = 0x00000000;
        InputData.StatusWord = *(ec_slave[1].inputs+1);
        InputData.StatusWord <<= 8;
        InputData.StatusWord |= *(ec_slave[1].inputs);
        InputData.LenzeStatus = *(ec_slave[1].inputs+3);
        InputData.LenzeStatus <<= 8;
        InputData.LenzeStatus |= *(ec_slave[1].inputs+2);
        InputData.ModeStatus = *(ec_slave[1].inputs + 4);
        InputData.ErrorCode = *(ec_slave[1].inputs + 6);
        InputData.ErrorCode <<= 8;
        InputData.ErrorCode |= *(ec_slave[1].inputs + 5);
        InputData.VelValue |= *(ec_slave[1].inputs+10);
        InputData.VelValue <<= 8;
        InputData.VelValue |= *(ec_slave[1].inputs+9);
        InputData.VelValue <<= 8;
        InputData.VelValue |= *(ec_slave[1].inputs+8);
        InputData.VelValue <<= 8;
        InputData.VelValue |= *(ec_slave[1].inputs+7);
        InputData.TorValue = 0;
        InputData.TorValue |= *(ec_slave[1].inputs+12);
        InputData.TorValue <<= 8;
        InputData.TorValue |= *(ec_slave[1].inputs+11);
        InputData.PosiValue |= *(ec_slave[1].inputs+16);
        InputData.PosiValue <<= 8;
        InputData.PosiValue |= *(ec_slave[1].inputs+15);
        InputData.PosiValue <<= 8;
        InputData.PosiValue |= *(ec_slave[1].inputs+14);
        InputData.PosiValue <<= 8;
        InputData.PosiValue |= *(ec_slave[1].inputs+13);
        str.sprintf("0x%04x",InputData.StatusWord);
        ui->Stawordlabel->setText(str);//status word
        str.sprintf("0x%04x",InputData.LenzeStatus);
        ui->LenzeStalabel->setText(str);//lenze status
        str.sprintf("0x%02x",InputData.ModeStatus);
        ui->MotStalabel->setText(str);//mode of operation display
        str.sprintf("0x%04x",InputData.ErrorCode);
        ui->errcodelabel->setText(str);//error code
        str.sprintf("%d",InputData.VelValue);
        ui->VelVallabel->setText(str);//velocity actual value
        str.sprintf("%d",InputData.TorValue);
        ui->TorVallabel->setText(str);//Torque actual value
        str.sprintf("%d",InputData.PosiValue);
        ui->Posilabel->setText(str);//position actual value
        //ECATDIO-I16-O16
        str.sprintf("ECATDIO-I16-O16 input L:0x%x:",*(ec_slave[2].inputs));
        ui->infoBrowser->append(str);
        str.sprintf("ECATDIO-I16-O16 input H:0x%x:",*(ec_slave[2].inputs + 1));
        ui->infoBrowser->append(str);

    }

}

void MainWindow::on_stopButton_clicked()
{
    ec_close();
}

void MainWindow::on_clearButton_clicked()
{
    ui->infoBrowser->clear();
}

void MainWindow::on_initButton_clicked()
{
    QString str;
    str.sprintf("Init");
    ui->requestedlabel->setText(str);
    ec_slave[0].state = EC_STATE_INIT;
    ec_writestate(0);
}

void MainWindow::on_preopButton_clicked()
{
    QString str;
    str.sprintf("Pre-Op");
    ui->requestedlabel->setText(str);
    if(ec_slave[0].state == EC_STATE_BOOT)
    {
        ec_slave[0].state = EC_STATE_INIT;
        ec_writestate(0);
        ec_slave[0].state = EC_STATE_PRE_OP;
        ec_writestate(0);
    }
    else
    {
        ec_slave[0].state = EC_STATE_PRE_OP;
        ec_writestate(0);
    }
}

void MainWindow::on_safeopButton_clicked()
{
     QString str;
     str.sprintf("Safe-Op");
     ui->requestedlabel->setText(str);
    if(ec_slave[0].state == EC_STATE_INIT)
    {
        ec_slave[0].state = EC_STATE_PRE_OP;
        ec_writestate(0);
        ec_slave[0].state = EC_STATE_SAFE_OP;
        ec_writestate(0);
    }
    else if(ec_slave[0].state == EC_STATE_BOOT)
    {
        ec_slave[0].state = EC_STATE_INIT;
        ec_writestate(0);
        ec_slave[0].state = EC_STATE_PRE_OP;
        ec_writestate(0);
        ec_slave[0].state = EC_STATE_SAFE_OP;
        ec_writestate(0);
    }
    else
    {
        ec_slave[0].state = EC_STATE_SAFE_OP;
        ec_writestate(0);
    }

}

void MainWindow::on_opButton_clicked()
{
    QString str;
    str.sprintf("Operational");
    ui->requestedlabel->setText(str);
   if(ec_slave[0].state == EC_STATE_INIT)
   {
       ec_slave[0].state = EC_STATE_PRE_OP;
       ec_writestate(0);

       ec_slave[0].state = EC_STATE_SAFE_OP;
       ec_writestate(0);
       ec_slave[0].state = EC_STATE_OPERATIONAL;
       ec_writestate(0);
   }
   else if(ec_slave[0].state == EC_STATE_BOOT)
   {
       ec_slave[0].state = EC_STATE_INIT;
       ec_writestate(0);
       ec_slave[0].state = EC_STATE_PRE_OP;
       ec_writestate(0);
       ec_slave[0].state = EC_STATE_SAFE_OP;
       ec_writestate(0);
       ec_slave[0].state = EC_STATE_OPERATIONAL;
       ec_writestate(0);
   }
   else if(ec_slave[0].state == EC_STATE_PRE_OP)
   {
       ec_slave[0].state = EC_STATE_SAFE_OP;
       ec_writestate(0);
       ec_slave[0].state = EC_STATE_OPERATIONAL;
       ec_writestate(0);
   }
   else
   {
       ec_slave[0].state = EC_STATE_OPERATIONAL;
       ec_writestate(0);
   }
}

void MainWindow::on_enableButton_clicked()
{
    if(!MotorEnable)//enable operator
    {

        MotorEnable = true;
        ui->enableButton->setText("Disable");

        connect(enable_timer,SIGNAL(timeout()),this,SLOT(enable_motion_thread()));
        enable_timer->start(50);
    }
    else//disable operator
    {

        MotorEnable = false;
        ui->enableButton->setText("Enable");

        connect(disable_timer,SIGNAL(timeout()),this,SLOT(disable_motion_thread()));
        disable_timer->start(50);
    }
}
void MainWindow::on_enableButton_2A_clicked()
{
    if(!MotorEnable2A)//enable operator
    {

        MotorEnable2A = true;
        ui->enableButton_2A->setText("Disable");

        connect(enable_timer,SIGNAL(timeout()),this,SLOT(enable_motion_thread2A()));
        enable_timer->start(50);
    }
    else//disable operator
    {

        MotorEnable2A = false;
        ui->enableButton_2A->setText("Enable");

        connect(disable_timer,SIGNAL(timeout()),this,SLOT(disable_motion_thread2A()));
        disable_timer->start(50);
    }
}
void MainWindow::on_enableButton_2B_clicked()
{
    if(!MotorEnable2B)//enable operator
    {

        MotorEnable2B = true;
        ui->enableButton_2B->setText("Disable");

        connect(enable_timer,SIGNAL(timeout()),this,SLOT(enable_motion_thread2B()));
        enable_timer->start(50);
    }
    else//disable operator
    {

        MotorEnable2B = false;
        ui->enableButton_2B->setText("Enable");

        connect(disable_timer,SIGNAL(timeout()),this,SLOT(disable_motion_thread2B()));
        disable_timer->start(50);
    }
}

void MainWindow::enable_motion_thread()
{
    QString str;
    if(InputData.ErrorCode == 0x00)
    {

        if(MotorStaCount == 10)
        {
            if((InputData.StatusWord & 0x000f) == 0x00)
            {
                MotorStatus = MOTOR_NOT_READY;
                MotorStaCount = 0;
            }
            if((InputData.StatusWord & 0x000f) == 0x01)
            {
                MotorStatus = MOTOR_READY_SWITCH_ON;
                MotorStaCount = 0;
            }
            else if((InputData.StatusWord & 0x000f) == 0x03)
            {
                MotorStatus = MOTOR_SWITCH_ON;
                MotorStaCount = 0;
            }
            else if((InputData.StatusWord & 0x000f) == 0x07)
            {
                MotorStatus = MOTOR_ENABLE;
                MotorStaCount = 0;
                enable_timer->stop();
            }
            else
            {
                //nothing
            }
        }
        else
        {
            //nothing
        }
        if(MotorStatus == MOTOR_NOT_READY)//ready to switch on
        {
            OutputData.ControlWord |= 0x0006;
            OutputData.NegTorLim = 100;
            OutputData.PosTorLim = 100;
            OutputData.ModeCmd = 8;
            TargetPosi = InputData.PosiValue;
            //TargetPosi = 0x0000;
            OutputData.TargetPosi = TargetPosi;
            MotorStaCount++;


        }
        else if(MotorStatus == MOTOR_READY_SWITCH_ON)
        {
            OutputData.ControlWord |= 0x0007;
            MotorStaCount++;
        }
        else if(MotorStatus == MOTOR_SWITCH_ON)
        {
            OutputData.ControlWord |= 0x000f;
            MotorStaCount++;
        }
        else
        {
           //nothing
        }

    }
    else
    {
        //nothing
    }

}
void MainWindow::enable_motion_thread2A()
{
    QString str;
    if(InputData2A.ErrorCode == 0x00)
    {

        if(MotorStaCount2A == 10)
        {
            if((InputData2A.StatusWord & 0x000f) == 0x00)
            {
                MotorStatus2A = MOTOR_NOT_READY;
                MotorStaCount2A = 0;
            }
            if((InputData2A.StatusWord & 0x000f) == 0x01)
            {
                MotorStatus2A = MOTOR_READY_SWITCH_ON;
                MotorStaCount2A = 0;
            }
            else if((InputData2A.StatusWord & 0x000f) == 0x03)
            {
                MotorStatus2A = MOTOR_SWITCH_ON;
                MotorStaCount2A = 0;
            }
            else if((InputData2A.StatusWord & 0x000f) == 0x07)
            {
                MotorStatus2A = MOTOR_ENABLE;
                MotorStaCount2A = 0;
                enable_timer->stop();
            }
            else
            {
                //nothing
            }
        }
        else
        {
            //nothing
        }
        if(MotorStatus2A == MOTOR_NOT_READY)//ready to switch on
        {
            OutputData2A.ControlWord |= 0x0006;
            OutputData2A.NegTorLim = 100;
            OutputData2A.PosTorLim = 100;
            OutputData2A.ModeCmd = 8;
            TargetPosi = InputData2A.PosiValue;
            //TargetPosi = 0x0000;
            OutputData2A.TargetPosi = TargetPosi;
            MotorStaCount2A ++;


        }
        else if(MotorStatus2A == MOTOR_READY_SWITCH_ON)
        {
            OutputData2A.ControlWord |= 0x0007;
            MotorStaCount2A ++;
        }
        else if(MotorStatus2A == MOTOR_SWITCH_ON)
        {
            OutputData2A.ControlWord |= 0x000f;
            MotorStaCount2A ++;
        }
        else
        {
           //nothing
        }

    }
    else
    {
        //nothing
    }

}
void MainWindow::enable_motion_thread2B()
{
    QString str;
    if(InputData2B.ErrorCode == 0x00)
    {

        if(MotorStaCount2B == 10)
        {
            if((InputData2B.StatusWord & 0x000f) == 0x00)
            {
                MotorStatus2B = MOTOR_NOT_READY;
                MotorStaCount2B = 0;
            }
            if((InputData2B.StatusWord & 0x000f) == 0x01)
            {
                MotorStatus2B = MOTOR_READY_SWITCH_ON;
                MotorStaCount2B = 0;
            }
            else if((InputData2B.StatusWord & 0x000f) == 0x03)
            {
                MotorStatus2B = MOTOR_SWITCH_ON;
                MotorStaCount2B = 0;
            }
            else if((InputData2B.StatusWord & 0x000f) == 0x07)
            {
                MotorStatus2B = MOTOR_ENABLE;
                MotorStaCount2B = 0;
                enable_timer->stop();
            }
            else
            {
                //nothing
            }
        }
        else
        {
            //nothing
        }
        if(MotorStatus2B == MOTOR_NOT_READY)//ready to switch on
        {
            OutputData2B.ControlWord |= 0x0006;
            OutputData2B.NegTorLim = 100;
            OutputData2B.PosTorLim = 100;
            OutputData2B.ModeCmd = 8;
            TargetPosi = InputData2B.PosiValue;
            //TargetPosi = 0x0000;
            OutputData2B.TargetPosi = TargetPosi;
            MotorStaCount2B++;


        }
        else if(MotorStatus2B == MOTOR_READY_SWITCH_ON)
        {
            OutputData2B.ControlWord |= 0x0007;
            MotorStaCount2B++;
        }
        else if(MotorStatus2B == MOTOR_SWITCH_ON)
        {
            OutputData2B.ControlWord |= 0x000f;
            MotorStaCount2B++;
        }
        else
        {
           //nothing
        }

    }
    else
    {
        //nothing
    }

}


void MainWindow::disable_motion_thread()
{

    if(InputData.ErrorCode == 0x00)
    {
        OutputData.ControlWord &= 0xfff0;
        MotorStatus = MOTOR_NOT_READY;
        MotorStaCount++;
        if(MotorStaCount == 10)
        {
            disable_timer->stop();
            MotorStaCount = 0;
        }

    }
    else
    {
        //nothing
    }
}
void MainWindow::disable_motion_thread2A()
{

    if(InputData2A.ErrorCode == 0x00)
    {
        OutputData2A.ControlWord &= 0xfff0;
        MotorStatus2A = MOTOR_NOT_READY;
        MotorStaCount2A++;
        if(MotorStaCount2A == 10)
        {
            disable_timer->stop();
            MotorStaCount2A = 0;
        }

    }
    else
    {
        //nothing
    }
}
void MainWindow::disable_motion_thread2B()
{

    if(InputData2B.ErrorCode == 0x00)
    {
        OutputData2B.ControlWord &= 0xfff0;
        MotorStatus2B = MOTOR_NOT_READY;
        MotorStaCount2B++;
        if(MotorStaCount2B == 10)
        {
            disable_timer->stop();
            MotorStaCount2B = 0;
        }

    }
    else
    {
        //nothing
    }
}

void MainWindow::on_forwardButton_clicked()
{
    TargetPosi += 10000;


}
void MainWindow::on_forwardButton_2A_clicked()
{
    TargetPosi2A += 10000;
}
void MainWindow::on_forwardButton_2B_clicked()
{
    TargetPosi2B += 10000;
}
void MainWindow::on_resetButton_pressed()
{
    OutputData.ControlWord |= 0x0080;
}

void MainWindow::on_resetButton_2A_clicked()
{
    OutputData2A.ControlWord |= 0x0080;
    OutputData2B.ControlWord |= 0x0080;
}
void MainWindow::on_resetButton_2B_clicked()
{
    OutputData2A.ControlWord |= 0x0080;
    OutputData2B.ControlWord |= 0x0080;
}
void MainWindow::on_resetButton_released()
{
    OutputData.ControlWord &= 0xff07;
}

void MainWindow::on_reverseButton_clicked()
{
    TargetPosi -= 10000;
}
void MainWindow::on_reverseButton_2A_clicked()
{
    TargetPosi2A -= 10000;
}
void MainWindow::on_reverseButton_2B_clicked()
{
    TargetPosi2B -= 10000;
}

void MainWindow::on_cfgButton_clicked()
{
    initxmll = new initxml(this);
    initxmll->show();
}








