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

    Count = 0;
    MotorEnable = false;
    SendPdo = false;
    MotorStaCount = 0;
    MotorStatus = MOTOR_NOT_READY;
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

    unsigned int SDODATA[320]={
        10272,1,0x00,1,
        11593,1,0x01,1,
        11272,0,0x03,1,
        24672,0,0x08,1,
        24704,0,0x08070000,4,
        11270,7,0x0B01,2,
        11266,1,0x100E0000,4,
        24694,0,0x18730100,4,
        11016,1,0x1D010000,4,
        10688,1,0x1E2F0200,4,
        11266,3,0x2803,3,
        11012,0,0x29040000,4,
        11171,0,0x290C,2,
        11268,17,0x3600,2,
        11009,1,0x3601,2,
        11268,16,0x3800,2,
        11265,2,0x38180000,4,
        11268,15,0x3900,2,
        11016,2,0x3A000000,4,
        11265,7,0x3A01,2,
        11268,14,0x3B00,2,
        11268,13,0x3E00,2,
        11271,17,0x3F00,2,
        11268,12,0x4000,2,
        11271,16,0x4200,2,
        11268,11,0x4300,2,
        11271,15,0x4600,2,
        11268,10,0x4700,2,
        11268,9,0x4A00,2,
        11271,14,0x4A00,2,
        11271,13,0x4E00,2,
        11268,8,0x4F00,2,
        11009,2,0x5000,2,
        11270,8,0x5200,2,
        11271,12,0x5200,2,
        11268,7,0x5400,2,
        11271,11,0x5700,2,
        11269,0,0x5802,2,
        11590,1,0x5802,2,
        11268,6,0x5A00,2,
        11265,8,0x5C00,2,
        11268,5,0x6200,2,
        24800,0,0x6400,2,
        24801,0,0x6400,2,
        10512,1,0x68420000,4,
        11597,2,0x6900,2,
        11597,4,0x6900,2,
        11268,4,0x6C00,2,
        11271,8,0x6C00,2,
        11268,1,0x6F00,2,
        11271,7,0x7100,2,
        11271,6,0x7500,2,
        11268,3,0x7600,2,
        11271,5,0x7700,2,
        11270,5,0x7800,2,
        11271,4,0x7800,2,
        11271,3,0x7900,2,
        11268,2,0x7A00,2,
        11271,1,0x7A00,2,
        11271,2,0x7A00,2,
        11265,3,0x8C230000,4,
        11265,5,0x9E02,2,
        10720,2,0x9ED90000,4,
        11267,1,0xA0090000,4,
        11588,1,0xA00F,2,
        10688,2,0xAE07,2,
        11266,2,0xAF020000,4,
        11596,2,0xB40C,2,
        10496,1,0xBF940000,4,
        11596,1,0xD401,2,
        11265,6,0xDE03,2,
        10294,7,0xE30D0000,4,
        10562,2,0xE30D0000,4,
        11013,2,0xE30D0000,4,
        10294,6,0xE60A0000,4,
        10562,1,0xE60A0000,4,
        11013,1,0xE60A0000,4,
        11265,4,0xE803,2,
        24693,0,0xF0550000,4,
        10626,0,0xFFFFFF7F,4
    };

    int nSM,nFMMU;
    int Osize,Isize,PDOassign;
    int chk = 0;
    int os;
    int mos;
    int tempi;
    int16 mod;
    uint32 ob1;
    int16 ob2;
    uint8 ob3;
    uint16 tempH,tempL;
    uint32 tempHH,tempHL,tempLH,tempLL;
    QString str,str2;
    connect(timer0,SIGNAL(timeout()),this,SLOT(UpStatus()));
    if(ec_init("ens33"))
    {
        ui->infoBrowser->setText("ec_init succeeded");
        if(ec_config_init(FALSE) > 0)
        {

            str.sprintf("%d slaves was found",ec_slavecount);
            ui->infoBrowser->append(str);
            /////////////
            printf("SDO send:\n\r");
            //clear
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1c12,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1c13,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1a00,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x60410010;
            ec_SDOwrite(1,0x1a00,01,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x60610008;
            ec_SDOwrite(1,0x1a00,02,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x603f0010;
            ec_SDOwrite(1,0x1a00,03,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x606c0020;
            ec_SDOwrite(1,0x1a00,04,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x60770010;
            ec_SDOwrite(1,0x1a00,05,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x60640020;
            ec_SDOwrite(1,0x1a00,06,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x60f40020;
            ec_SDOwrite(1,0x1a00,07,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x07;
            ec_SDOwrite(1,0x1a00,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1a01,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1a02,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1a03,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1a04,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1a05,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1a06,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1a07,00,FALSE,os,&ob3,EC_TIMEOUTRXM);

            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1600,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x60400010;
            ec_SDOwrite(1,0x1600,01,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x60600008;
            ec_SDOwrite(1,0x1600,02,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob1); ob1 = 0x607a0020;
            ec_SDOwrite(1,0x1600,03,FALSE,os,&ob1,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x03;
            ec_SDOwrite(1,0x1600,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1601,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1602,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1603,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1604,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1605,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1606,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x00;
            ec_SDOwrite(1,0x1607,00,FALSE,os,&ob3,EC_TIMEOUTRXM);

            os=sizeof(ob2); ob2 = 0x1600;
            ec_SDOwrite(1,0x1c12,01,FALSE,os,&ob2,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x01;
            ec_SDOwrite(1,0x1c12,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            os=sizeof(ob2); ob2 = 0x1a00;
            ec_SDOwrite(1,0x1c13,01,FALSE,os,&ob2,EC_TIMEOUTRXM);
            os=sizeof(ob3); ob3 = 0x01;
            ec_SDOwrite(1,0x1c13,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
            ec_config_map(&IOmap);
            ec_configdc();
            //sdo resqust
            str.sprintf("Name:%s",ec_slave[1].name);
            ui->infoBrowser->append(str);
            str.sprintf("Vender ID:%8.8x",ec_slave[1].eep_man);
            ui->infoBrowser->append(str);
            str.sprintf("Product Code:%8.8x",ec_slave[1].eep_id);
            ui->infoBrowser->append(str);
            ui->infoBrowser->append("");
            for(nSM = 0 ; nSM < EC_MAXSM ; nSM++)
            {
                if(ec_slave[1].SM[nSM].StartAddr > 0)
                {
                    if(ec_slave[1].SM[nSM].StartAddr > 0)
                       str.sprintf(" SM%1d StAddr:%4.4x Len:%4d Flag:%8.8x Type:",nSM, ec_slave[1].SM[nSM].StartAddr, ec_slave[1].SM[nSM].SMlength,
                             (int)ec_slave[1].SM[nSM].SMflags);
                     if(ec_slave[1].SMtype[nSM] == 1)
                             str += "MbxWr";
                     if(ec_slave[1].SMtype[nSM] == 2)
                             str += "MbxRd";
                     if(ec_slave[1].SMtype[nSM] == 3)
                             str += "Outputs";
                     if(ec_slave[1].SMtype[nSM] == 4)
                             str += "Inputs";
                      ui->infoBrowser->append(str);
                }

            }
            ui->infoBrowser->append("");
            nFMMU = ec_slave[1].FMMUunused;
            for(nFMMU = 0 ; nFMMU < ec_slave[1].FMMUunused ; nFMMU++)
            {
                str.sprintf(" FMMU%1d Ls:%8.8x Ll:%4d Lsb:%d Leb:%d Ps:%4.4x Psb:%d Ty:%2.2x Act:%2.2x", nFMMU,
                       (int)ec_slave[1].FMMU[nFMMU].LogStart, ec_slave[1].FMMU[nFMMU].LogLength, ec_slave[1].FMMU[nFMMU].LogStartbit,
                       ec_slave[1].FMMU[nFMMU].LogEndbit, ec_slave[1].FMMU[nFMMU].PhysStart, ec_slave[1].FMMU[nFMMU].PhysStartBit,
                       ec_slave[1].FMMU[nFMMU].FMMUtype, ec_slave[1].FMMU[nFMMU].FMMUactive);
                ui->infoBrowser->append(str);
            }
            str.sprintf(" FMMUfunc 0:%d 1:%d 2:%d 3:%d",
                        ec_slave[1].FMMU0func, ec_slave[1].FMMU1func, ec_slave[1].FMMU2func, ec_slave[1].FMMU3func);
            ui->infoBrowser->append(str);
            str.sprintf(" MBX length wr: %d rd: %d MBX protocols : %2.2x", ec_slave[1].mbx_l, ec_slave[1].mbx_rl, ec_slave[1].mbx_proto);
            ui->infoBrowser->append(str);

           ///////
           ui->infoBrowser->append("");
           str.sprintf(" ///////////////////////////////////////////////");
           ui->infoBrowser->append(str);
           ec_readPDOmapCA(1,&Osize,&Isize);
           ec_readPDOassign(1, 0x1c12);//prarm:(slave,index)
           //str.sprintf(" Osize:%d,Isize:%d,PDOassign:%d",Osize,Isize,PDOassign);
           //ui->infoBrowser->append(str);
           ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);
           str.sprintf("Obytes:%d, Ivytes:%d, Obits:%d, Ibits:%d",ec_slave[0].Obytes,ec_slave[0].Ibytes,ec_slave[0].Obits,ec_slave[0].Ibits);
           ui->infoBrowser->append(str);

           //ready to OP state
            ec_slave[1].state = EC_STATE_OPERATIONAL;
            ec_writestate(1);
            chk = 100;
            do
            {
                 ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
            }while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
            if(ec_slave[1].state == EC_STATE_OPERATIONAL)
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
    timer0->start(5);

}
void MainWindow:: UpStatus()
{
    QString str,sta;
    int p,psize,oloop,iloop,expectedWKC,Tempj,wkc;
    int TempData = 0;
    ec_errort Ec;
    Count++;
    //ec_readstate();
    if(ec_slave[1].state == EC_STATE_INIT)
    {
        sta.sprintf("Init");

    }
    else if(ec_slave[1].state == EC_STATE_PRE_OP)
    {
        sta.sprintf("Pre-Op");
    }
    else if(ec_slave[1].state == EC_STATE_BOOT)
    {
        sta.sprintf("BOOT");
    }
    else if(ec_slave[1].state == EC_STATE_OPERATIONAL)
    {
        sta.sprintf("Operational");
    }
    else if(ec_slave[1].state == EC_STATE_SAFE_OP)
    {
        sta.sprintf("Safe-Op");
    }
    else if(ec_slave[1].state == EC_STATE_ACK)
    {
        sta.sprintf("ACK");
    }
    else if(ec_slave[1].state == EC_STATE_ERROR)
    {
        sta.sprintf("Error");
    }
    else
    {
        sta.sprintf("%d",ec_slave[1].state);
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
    if(SendPdo && ec_slave[1].state == EC_STATE_OPERATIONAL)
    {
         OutputData.TargetPosi = TargetPosi;
         OutputData.ModeCmd = 8;
        *(ec_slave[1].outputs) = OutputData.ControlWord & 0x00ff;
        *(ec_slave[1].outputs + 1 ) = (OutputData.ControlWord & 0xff00) >> 8;
        *(ec_slave[1].outputs + 2 ) = OutputData.ModeCmd;
        *(ec_slave[1].outputs + 3 ) = OutputData.TargetPosi & 0x000000ff;
        *(ec_slave[1].outputs + 4 ) = (OutputData.TargetPosi & 0x0000ff00) >> 8;
        *(ec_slave[1].outputs + 5 ) = (OutputData.TargetPosi & 0x00ff0000) >> 16;
        *(ec_slave[1].outputs + 6 ) = (OutputData.TargetPosi & 0xff000000) >> 24;
        ec_send_processdata();
        wkc = ec_receive_processdata(EC_TIMEOUTRET);
        expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;

        //str.sprintf("IN PosiValue:%x,TargetPosi:%x,OUT TargetPosi:%x",InputData.PosiValue,TargetPosi,OutputData.TargetPosi);
        //ui->infoBrowser->append(str);

        InputData.StatusWord = 0x0000;
        InputData.ModeStatus = 0x00;
        InputData.ErrorCode = 0x0000;
        InputData.VelValue = 0x00000000;
        InputData.TorValue = 0x0000;
        InputData.PosiValue = 0x00000000;
        InputData.StatusWord = *(ec_slave[1].inputs+1);
        InputData.StatusWord <<= 8;
        InputData.StatusWord |= *(ec_slave[1].inputs);
        InputData.ModeStatus = *(ec_slave[1].inputs + 2);
        InputData.ErrorCode = *(ec_slave[1].inputs + 4);
        InputData.ErrorCode <<= 8;
        InputData.ErrorCode |= *(ec_slave[1].inputs + 3);
        InputData.VelValue |= *(ec_slave[1].inputs+8);
        InputData.VelValue <<= 8;
        InputData.VelValue |= *(ec_slave[1].inputs+7);
        InputData.VelValue <<= 8;
        InputData.VelValue |= *(ec_slave[1].inputs+6);
        InputData.VelValue <<= 8;
        InputData.VelValue |= *(ec_slave[1].inputs+5);
        InputData.TorValue = 0;
        InputData.TorValue |= *(ec_slave[1].inputs+10);
        InputData.TorValue <<= 8;
        InputData.TorValue |= *(ec_slave[1].inputs+9);
        InputData.PosiValue |= *(ec_slave[1].inputs+14);
        InputData.PosiValue <<= 8;
        InputData.PosiValue |= *(ec_slave[1].inputs+13);
        InputData.PosiValue <<= 8;
        InputData.PosiValue |= *(ec_slave[1].inputs+12);
        InputData.PosiValue <<= 8;
        InputData.PosiValue |= *(ec_slave[1].inputs+11);

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
    ec_slave[1].state = EC_STATE_INIT;
    ec_writestate(1);
}

void MainWindow::on_preopButton_clicked()
{
    QString str;
    str.sprintf("Pre-Op");
    ui->requestedlabel->setText(str);
    if(ec_slave[1].state == EC_STATE_BOOT)
    {
        ec_slave[1].state = EC_STATE_INIT;
        ec_writestate(1);
        ec_slave[1].state = EC_STATE_PRE_OP;
        ec_writestate(1);
    }
    else
    {
        ec_slave[1].state = EC_STATE_PRE_OP;
        ec_writestate(1);
    }
}

void MainWindow::on_safeopButton_clicked()
{
     QString str;
     str.sprintf("Safe-Op");
     ui->requestedlabel->setText(str);
    if(ec_slave[1].state == EC_STATE_INIT)
    {
        ec_slave[1].state = EC_STATE_PRE_OP;
        ec_writestate(1);
        ec_slave[1].state = EC_STATE_SAFE_OP;
        ec_writestate(1);
    }
    else if(ec_slave[1].state == EC_STATE_BOOT)
    {
        ec_slave[1].state = EC_STATE_INIT;
        ec_writestate(1);
        ec_slave[1].state = EC_STATE_PRE_OP;
        ec_writestate(1);
        ec_slave[1].state = EC_STATE_SAFE_OP;
        ec_writestate(1);
    }
    else
    {
        ec_slave[1].state = EC_STATE_SAFE_OP;
        ec_writestate(1);
    }

}

void MainWindow::on_opButton_clicked()
{
    QString str;
    str.sprintf("Operational");
    ui->requestedlabel->setText(str);
   if(ec_slave[1].state == EC_STATE_INIT)
   {
       ec_slave[1].state = EC_STATE_PRE_OP;
       ec_writestate(1);

       ec_slave[1].state = EC_STATE_SAFE_OP;
       ec_writestate(1);
       ec_slave[1].state = EC_STATE_OPERATIONAL;
       ec_writestate(1);
   }
   else if(ec_slave[1].state == EC_STATE_BOOT)
   {
       ec_slave[1].state = EC_STATE_INIT;
       ec_writestate(1);
       ec_slave[1].state = EC_STATE_PRE_OP;
       ec_writestate(1);
       ec_slave[1].state = EC_STATE_SAFE_OP;
       ec_writestate(1);
       ec_slave[1].state = EC_STATE_OPERATIONAL;
       ec_writestate(1);
   }
   else if(ec_slave[1].state == EC_STATE_PRE_OP)
   {
       ec_slave[1].state = EC_STATE_SAFE_OP;
       ec_writestate(1);
       ec_slave[1].state = EC_STATE_OPERATIONAL;
       ec_writestate(1);
   }
   else
   {
       ec_slave[1].state = EC_STATE_OPERATIONAL;
       ec_writestate(1);
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
            //OutputData.ModeCmd = 8;
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

void MainWindow::on_forwardButton_clicked()
{
    TargetPosi += 1000;


}

void MainWindow::on_resetButton_pressed()
{
    OutputData.ControlWord |= 0x0080;
}

void MainWindow::on_resetButton_released()
{
    OutputData.ControlWord &= 0xff07;
}

void MainWindow::on_reverseButton_clicked()
{
    TargetPosi -= 1000;
}

void MainWindow::on_resetButton_clicked()
{

}
void MainWindow::on_forwardButton_pressed()
{
   // TargetPosi += 1000;
}

void MainWindow::on_reverseButton_pressed()
{
    //TargetPosi -= 100000;
}

void MainWindow::on_cfgButton_clicked()
{
    initxmll = new initxml(this);
    initxmll->show();
}


