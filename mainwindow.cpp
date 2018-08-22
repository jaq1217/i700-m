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
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1c12,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1c13,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1a00,00,FALSE,os,&ob3,EC_TIMEOUTRXM); //????????????????
//            os=sizeof(ob1); ob1 = 0x60410010;
//            ec_SDOwrite(1,0x1a00,01,FALSE,os,&ob1,EC_TIMEOUTRXM);
//            os=sizeof(ob1); ob1 = 0x60640020;
//            ec_SDOwrite(1,0x1a00,02,FALSE,os,&ob1,EC_TIMEOUTRXM);
//            os=sizeof(ob1); ob1 = 0x606C0020;
//            ec_SDOwrite(1,0x1a00,03,FALSE,os,&ob1,EC_TIMEOUTRXM);
//            os=sizeof(ob1); ob1 = 0x603F0010;
//            ec_SDOwrite(1,0x1a00,04,FALSE,os,&ob1,EC_TIMEOUTRXM);
//            os=sizeof(ob1); ob1 = 0x60610010;
//            ec_SDOwrite(1,0x1a00,05,FALSE,os,&ob1,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1a00,06,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1a00,07,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1a00,8,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1a00,9,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1a00,10,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x0a;
//            ec_SDOwrite(1,0x1a00,0,FALSE,os,&ob3,EC_TIMEOUTRXM);//????????

//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1600,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob1); ob1 = 0x60400010;
//            ec_SDOwrite(1,0x1600,01,FALSE,os,&ob1,EC_TIMEOUTRXM);
//            os=sizeof(ob1); ob1 = 0x607a0020;
//            ec_SDOwrite(1,0x1600,02,FALSE,os,&ob1,EC_TIMEOUTRXM);
//            os=sizeof(ob1); ob1 = 0x60810010;
//            ec_SDOwrite(1,0x1600,03,FALSE,os,&ob1,EC_TIMEOUTRXM);
//            os=sizeof(ob1); ob1 = 0x60600010;
//            ec_SDOwrite(1,0x1600,04,FALSE,os,&ob1,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1600,05,FALSE,os,&ob3,EC_TIMEOUTRXM);  //0x00 or 0x00000000?????
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1600,06,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1600,07,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1600,8,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1600,9,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x00;
//            ec_SDOwrite(1,0x1600,10,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x0a;
//            ec_SDOwrite(1,0x1600,0,FALSE,os,&ob3,EC_TIMEOUTRXM);//????????

//            os=sizeof(ob2); ob2 = 0x1600;
//            ec_SDOwrite(1,0x1c12,01,FALSE,os,&ob2,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x01;
//            ec_SDOwrite(1,0x1c12,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
//            os=sizeof(ob2); ob2 = 0x1a00;
//            ec_SDOwrite(1,0x1c13,01,FALSE,os,&ob2,EC_TIMEOUTRXM);
//            os=sizeof(ob3); ob3 = 0x01;
//            ec_SDOwrite(1,0x1c13,00,FALSE,os,&ob3,EC_TIMEOUTRXM);

            printf("SDO end:\n\r");

            ///////////////////
            ec_config_map(&IOmap);
//            ec_configdc();

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
           //str.sprintf(" Osize:%d,Isize:%d,PDOassign:%d",Osize,Isize,PDOassign);
           //ui->infoBrowser->append(str);
           ec_statecheck(1, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);
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
                //SendPdo = true;

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
        *(ec_slave[1].outputs) = OutputData.ControlWord & 0x00ff;
        *(ec_slave[1].outputs + 1 ) = (OutputData.ControlWord & 0xff00) >> 8;

         *(ec_slave[1].outputs + 8 ) = OutputData.ModeCmd & 0x00ff;
         *(ec_slave[1].outputs + 9 ) = (OutputData.ModeCmd & 0x00ff) >> 8;

         *(ec_slave[1].outputs + 2 ) = OutputData.TargetPosi & 0x000000ff;
        *(ec_slave[1].outputs + 3 ) = (OutputData.TargetPosi & 0x0000ff00) >> 8;
        *(ec_slave[1].outputs + 4 ) = (OutputData.TargetPosi & 0x00ff0000) >> 16;
        *(ec_slave[1].outputs + 5 ) = (OutputData.TargetPosi & 0xff000000) >> 24;
         *(ec_slave[1].outputs + 6 ) = 0x55;
         *(ec_slave[1].outputs + 7 ) = 0x55;
//        *(ec_slave[1].outputs + 19) = OutputData.PosTorLim & 0x00ff;
//        *(ec_slave[1].outputs + 20 ) = (OutputData.PosTorLim & 0xff00) >> 8;
//        *(ec_slave[1].outputs + 21) = OutputData.NegTorLim & 0x00ff;
//        *(ec_slave[1].outputs + 22 ) = (OutputData.NegTorLim & 0xff00) >> 8;
        ec_send_processdata();
        wkc = ec_receive_processdata(EC_TIMEOUTRET);
        expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;

        //str.sprintf("IN PosiValue:%x,TargetPosi:%x,OUT TargetPosi:%x",InputData.PosiValue,TargetPosi,OutputData.TargetPosi);
        //ui->infoBrowser->append(str);

        InputData.StatusWord = 0x0000;
        InputData.LenzeStatus = 0x0000;
        InputData.ModeStatus = 0x0000;
        InputData.ErrorCode = 0x0000;
        InputData.VelValue = 0x00000000;
        InputData.TorValue = 0x0000;
        InputData.PosiValue = 0x00000000;
        InputData.StatusWord = *(ec_slave[1].inputs+1);
        InputData.StatusWord <<= 8;
        InputData.StatusWord |= *(ec_slave[1].inputs);
//        InputData.LenzeStatus = *(ec_slave[1].inputs+3);
//        InputData.LenzeStatus <<= 8;
//        InputData.LenzeStatus |= *(ec_slave[1].inputs+2);
        InputData.ModeStatus = *(ec_slave[1].inputs + 13);
        InputData.ModeStatus <<= 8;
        InputData.ModeStatus = *(ec_slave[1].inputs + 12);
        InputData.ErrorCode = *(ec_slave[1].inputs + 11);
        InputData.ErrorCode <<= 8;
        InputData.ErrorCode |= *(ec_slave[1].inputs + 10);
        InputData.VelValue |= *(ec_slave[1].inputs+9);
        InputData.VelValue <<= 8;
        InputData.VelValue |= *(ec_slave[1].inputs+8);
        InputData.VelValue <<= 8;
        InputData.VelValue |= *(ec_slave[1].inputs+7);
        InputData.VelValue <<= 8;
        InputData.VelValue |= *(ec_slave[1].inputs+6);
//        InputData.TorValue = 0;
//        InputData.TorValue |= *(ec_slave[1].inputs+12);
//        InputData.TorValue <<= 8;
//        InputData.TorValue |= *(ec_slave[1].inputs+11);
        InputData.PosiValue |= *(ec_slave[1].inputs+5);
        InputData.PosiValue <<= 8;
        InputData.PosiValue |= *(ec_slave[1].inputs+4);
        InputData.PosiValue <<= 8;
        InputData.PosiValue |= *(ec_slave[1].inputs+3);
        InputData.PosiValue <<= 8;
        InputData.PosiValue |= *(ec_slave[1].inputs+2);

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
//            OutputData.NegTorLim = 100;
//            OutputData.PosTorLim = 100;
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
    QString str;
    TargetPosi += 5000;
    str.sprintf("TargetPosi = %x",TargetPosi);
    ui->infoBrowser->append(str);

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
    QString str;
    TargetPosi -= 5000;
    str.sprintf("TargetPosi = %x",TargetPosi);
    ui->infoBrowser->append(str);
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


