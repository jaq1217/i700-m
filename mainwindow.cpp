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

            for(slave = 1;slave <= ec_slavecount;slave ++)
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
            for(slave = 1;slave <= ec_slavecount; slave++)
            {
                str.sprintf("Name:%s",ec_slave[slave].name);
                ui->infoBrowser->append(str);
            }
            ui->infoBrowser->append("");

//            str.sprintf("Vender ID:%8.8x",ec_slave[1].eep_man);
//            ui->infoBrowser->append(str);
//            str.sprintf("Product Code:%8.8x",ec_slave[1].eep_id);
//            ui->infoBrowser->append(str);

//            for(nSM = 0 ; nSM < EC_MAXSM ; nSM++)
//            {
//                if(ec_slave[1].SM[nSM].StartAddr > 0)
//                {
//                    if(ec_slave[1].SM[nSM].StartAddr > 0)
//                       str.sprintf(" SM%1d StAddr:%4.4x Len:%4d Flag:%8.8x Type:",nSM, ec_slave[1].SM[nSM].StartAddr, ec_slave[1].SM[nSM].SMlength,
//                             (int)ec_slave[1].SM[nSM].SMflags);
//                     if(ec_slave[1].SMtype[nSM] == 1)
//                             str += "MbxWr";
//                     if(ec_slave[1].SMtype[nSM] == 2)
//                             str += "MbxRd";
//                     if(ec_slave[1].SMtype[nSM] == 3)
//                             str += "Outputs";
//                     if(ec_slave[1].SMtype[nSM] == 4)
//                             str += "Inputs";
//                      ui->infoBrowser->append(str);
//                }

//            }
//            ui->infoBrowser->append("");
//            nFMMU = ec_slave[1].FMMUunused;
//            for(nFMMU = 0 ; nFMMU < ec_slave[1].FMMUunused ; nFMMU++)
//            {
//                str.sprintf(" FMMU%1d Ls:%8.8x Ll:%4d Lsb:%d Leb:%d Ps:%4.4x Psb:%d Ty:%2.2x Act:%2.2x", nFMMU,
//                       (int)ec_slave[1].FMMU[nFMMU].LogStart, ec_slave[1].FMMU[nFMMU].LogLength, ec_slave[1].FMMU[nFMMU].LogStartbit,
//                       ec_slave[1].FMMU[nFMMU].LogEndbit, ec_slave[1].FMMU[nFMMU].PhysStart, ec_slave[1].FMMU[nFMMU].PhysStartBit,
//                       ec_slave[1].FMMU[nFMMU].FMMUtype, ec_slave[1].FMMU[nFMMU].FMMUactive);
//                ui->infoBrowser->append(str);
//            }
//            str.sprintf(" FMMUfunc 0:%d 1:%d 2:%d 3:%d",
//                        ec_slave[1].FMMU0func, ec_slave[1].FMMU1func, ec_slave[1].FMMU2func, ec_slave[1].FMMU3func);
//            ui->infoBrowser->append(str);
//            str.sprintf(" MBX length wr: %d rd: %d MBX protocols : %2.2x", ec_slave[1].mbx_l, ec_slave[1].mbx_rl, ec_slave[1].mbx_proto);
//            ui->infoBrowser->append(str);

//           ///////
//           ui->infoBrowser->append("");
//           str.sprintf(" ///////////////////////////////////////////////");
//           ui->infoBrowser->append(str);
//           ec_readPDOmapCA(1,&Osize,&Isize);
//           ec_readPDOassign(1, 0x1c12);//prarm:(slave,index)
//           //str.sprintf(" Osize:%d,Isize:%d,PDOassign:%d",Osize,Isize,PDOassign);
//           //ui->infoBrowser->append(str);
//           ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);
//           str.sprintf("Obytes:%d, Ivytes:%d, Obits:%d, Ibits:%d",ec_slave[0].Obytes,ec_slave[0].Ibytes,ec_slave[0].Obits,ec_slave[0].Ibits);
//           ui->infoBrowser->append(str);

           //ready to OP state
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            ec_writestate(0);
            chk = 100;
            do
            {
                 ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
            }while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

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
    //clear sm
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1c12,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1c13,00,FALSE,os,&ob3,EC_TIMEOUTRXM);
    //clear and config axis A
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1a06,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//??
    os=sizeof(ob1); ob1 = 0x28330010;
    ec_SDOwrite(id,0x1a06,01,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob1); ob1 = 0x60fd0020;
    ec_SDOwrite(id,0x1a06,02,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0x02;
    ec_SDOwrite(id,0x1a06,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//??
    //clear and config axis B
    os=sizeof(ob3); ob3 = 0;
    ec_SDOwrite(id,0x1a16,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//??
    os=sizeof(ob1); ob1 = 0x28330010;
    ec_SDOwrite(id,0x1a16,01,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob1); ob1 = 0x60fd0020;
    ec_SDOwrite(id,0x1a16,02,FALSE,os,&ob1,EC_TIMEOUTRXM);
    os=sizeof(ob3); ob3 = 0x02;
    ec_SDOwrite(id,0x1a16,00,FALSE,os,&ob3,EC_TIMEOUTRXM);//??
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
    ec_SDOwrite(id,0x1c13,00,FALSE,os,&ob2,EC_TIMEOUTRXM);

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

        for(slave = 1; slave <= ec_slavecount; slave ++)
        {
            if(ec_slave[slave].eep_id == SINGLE_CODE)
            {
                OutputData.TargetPosi = TargetPosi;
               *(ec_slave[slave].outputs) = OutputData.ControlWord & 0x00ff;
               *(ec_slave[slave].outputs + 1 ) = (OutputData.ControlWord & 0xff00) >> 8;
               *(ec_slave[slave].outputs + 4 ) = OutputData.ModeCmd;
               *(ec_slave[slave].outputs + 7 ) = OutputData.TargetPosi & 0x000000ff;
               *(ec_slave[slave].outputs + 8 ) = (OutputData.TargetPosi & 0x0000ff00) >> 8;
               *(ec_slave[slave].outputs + 9 ) = (OutputData.TargetPosi & 0x00ff0000) >> 16;
               *(ec_slave[slave].outputs + 10 ) = (OutputData.TargetPosi & 0xff000000) >> 24;
               *(ec_slave[slave].outputs + 19) = OutputData.PosTorLim & 0x00ff;
               *(ec_slave[slave].outputs + 20 ) = (OutputData.PosTorLim & 0xff00) >> 8;
               *(ec_slave[slave].outputs + 21) = OutputData.NegTorLim & 0x00ff;
               *(ec_slave[slave].outputs + 22 ) = (OutputData.NegTorLim & 0xff00) >> 8;
            }
            else if(ec_slave[slave].eep_id == DOUBLE_CODE)
            {
                //testing jaq
                OutputData2A.TargetPosi = TargetPosi;//
                *(ec_slave[slave].outputs) = OutputData2A.ControlWord & 0x00ff;
                *(ec_slave[slave].outputs + 1 ) = (OutputData2A.ControlWord & 0xff00) >> 8;
                *(ec_slave[slave].outputs + 4 ) = OutputData2A.ModeCmd;
                *(ec_slave[slave].outputs + 7 ) = OutputData2A.TargetPosi & 0x000000ff;
                *(ec_slave[slave].outputs + 8 ) = (OutputData2A.TargetPosi & 0x0000ff00) >> 8;
                *(ec_slave[slave].outputs + 9 ) = (OutputData2A.TargetPosi & 0x00ff0000) >> 16;
                *(ec_slave[slave].outputs + 10 ) = (OutputData2A.TargetPosi & 0xff000000) >> 24;
                *(ec_slave[slave].outputs + 19 ) = OutputData2A.PosTorLim & 0x00ff;
                *(ec_slave[slave].outputs + 20 ) = (OutputData2A.PosTorLim & 0xff00) >> 8;
                *(ec_slave[slave].outputs + 21 ) = OutputData2A.NegTorLim & 0x00ff;
                *(ec_slave[slave].outputs + 22 ) = (OutputData2A.NegTorLim & 0xff00) >> 8;
                OutputData2B.TargetPosi = TargetPosi;//
                *(ec_slave[slave].outputs + 23 ) = OutputData2B.ControlWord & 0x00ff;
                *(ec_slave[slave].outputs + 24 ) = (OutputData2B.ControlWord & 0xff00) >> 8;
                *(ec_slave[slave].outputs + 27 ) = OutputData2B.ModeCmd;
                *(ec_slave[slave].outputs + 30 ) = OutputData2B.TargetPosi & 0x000000ff;
                *(ec_slave[slave].outputs + 31 ) = (OutputData2B.TargetPosi & 0x0000ff00) >> 8;
                *(ec_slave[slave].outputs + 32 ) = (OutputData2B.TargetPosi & 0x00ff0000) >> 16;
                *(ec_slave[slave].outputs + 33 ) = (OutputData2B.TargetPosi & 0xff000000) >> 24;
                *(ec_slave[slave].outputs + 42 ) = OutputData2B.PosTorLim & 0x00ff;
                *(ec_slave[slave].outputs + 43 ) = (OutputData2B.PosTorLim & 0xff00) >> 8;
                *(ec_slave[slave].outputs + 44 ) = OutputData2B.NegTorLim & 0x00ff;
                *(ec_slave[slave].outputs + 45 ) = (OutputData2B.NegTorLim & 0xff00) >> 8;
            }
            else
            {
                //nothing
            }
        }
        ec_send_processdata();
        wkc = ec_receive_processdata(EC_TIMEOUTRET);
        expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;

        for(slave = 1; slave <= ec_slavecount; slave ++)
        {
            if(ec_slave[slave].eep_id == SINGLE_CODE)
            {
                InputData.StatusWord = 0x0000;
                InputData.LenzeStatus = 0x0000;
                InputData.ModeStatus = 0x00;
                InputData.ErrorCode = 0x0000;
                InputData.VelValue = 0x00000000;
                InputData.TorValue = 0x0000;
                InputData.PosiValue = 0x00000000;
                InputData.StatusWord = *(ec_slave[slave].inputs+1);
                InputData.StatusWord <<= 8;
                InputData.StatusWord |= *(ec_slave[slave].inputs);
                InputData.LenzeStatus = *(ec_slave[slave].inputs+3);
                InputData.LenzeStatus <<= 8;
                InputData.LenzeStatus |= *(ec_slave[slave].inputs+2);
                InputData.ModeStatus = *(ec_slave[slave].inputs + 4);
                InputData.ErrorCode = *(ec_slave[slave].inputs + 6);
                InputData.ErrorCode <<= 8;
                InputData.ErrorCode |= *(ec_slave[slave].inputs + 5);
                InputData.VelValue |= *(ec_slave[slave].inputs+10);
                InputData.VelValue <<= 8;
                InputData.VelValue |= *(ec_slave[slave].inputs+9);
                InputData.VelValue <<= 8;
                InputData.VelValue |= *(ec_slave[slave].inputs+8);
                InputData.VelValue <<= 8;
                InputData.VelValue |= *(ec_slave[slave].inputs+7);
                InputData.TorValue = 0;
                InputData.TorValue |= *(ec_slave[slave].inputs+12);
                InputData.TorValue <<= 8;
                InputData.TorValue |= *(ec_slave[slave].inputs+11);
                InputData.PosiValue |= *(ec_slave[slave].inputs+16);
                InputData.PosiValue <<= 8;
                InputData.PosiValue |= *(ec_slave[slave].inputs+15);
                InputData.PosiValue <<= 8;
                InputData.PosiValue |= *(ec_slave[slave].inputs+14);
                InputData.PosiValue <<= 8;
                InputData.PosiValue |= *(ec_slave[slave].inputs+13);
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
            else if(ec_slave[slave].eep_id == DOUBLE_CODE)
            {

                //testing jaq
                InputData2A.StatusWord = 0x0000;
                InputData2A.LenzeStatus = 0x0000;
                InputData2A.ModeStatus = 0x00;
                InputData2A.ErrorCode = 0x0000;
                InputData2A.VelValue = 0x00000000;
                InputData2A.TorValue = 0x0000;
                InputData2A.PosiValue = 0x00000000;
                InputData2B.StatusWord = 0x0000;
                InputData2B.LenzeStatus = 0x0000;
                InputData2B.ModeStatus = 0x00;
                InputData2B.ErrorCode = 0x0000;
                InputData2B.VelValue = 0x00000000;
                InputData2B.TorValue = 0x0000;
                InputData2B.PosiValue = 0x00000000;

                InputData2A.StatusWord = *(ec_slave[slave].inputs+1);
                InputData2A.StatusWord <<= 8;
                InputData2A.StatusWord |= *(ec_slave[slave].inputs);
                InputData2A.LenzeStatus = *(ec_slave[slave].inputs+3);
                InputData2A.LenzeStatus <<= 8;
                InputData2A.LenzeStatus |= *(ec_slave[slave].inputs+2);
                InputData2A.ModeStatus = *(ec_slave[slave].inputs + 4);
                InputData2A.ErrorCode = *(ec_slave[slave].inputs + 6);
                InputData2A.ErrorCode <<= 8;
                InputData2A.ErrorCode |= *(ec_slave[slave].inputs + 5);
                InputData2A.VelValue |= *(ec_slave[slave].inputs+10);
                InputData2A.VelValue <<= 8;
                InputData2A.VelValue |= *(ec_slave[slave].inputs+9);
                InputData2A.VelValue <<= 8;
                InputData2A.VelValue |= *(ec_slave[slave].inputs+8);
                InputData2A.VelValue <<= 8;
                InputData2A.VelValue |= *(ec_slave[slave].inputs+7);
                InputData2A.TorValue = 0;
                InputData2A.TorValue |= *(ec_slave[slave].inputs+12);
                InputData2A.TorValue <<= 8;
                InputData2A.TorValue |= *(ec_slave[slave].inputs+11);
                InputData2A.PosiValue |= *(ec_slave[slave].inputs+16);
                InputData2A.PosiValue <<= 8;
                InputData2A.PosiValue |= *(ec_slave[slave].inputs+15);
                InputData2A.PosiValue <<= 8;
                InputData2A.PosiValue |= *(ec_slave[slave].inputs+14);
                InputData2A.PosiValue <<= 8;
                InputData2A.PosiValue |= *(ec_slave[slave].inputs+13);

                InputData2B.StatusWord = *(ec_slave[slave].inputs + 18);
                InputData2B.StatusWord <<= 8;
                InputData2B.StatusWord |= *(ec_slave[slave].inputs + 17);
                InputData2B.LenzeStatus = *(ec_slave[slave].inputs + 20);
                InputData2B.LenzeStatus <<= 8;
                InputData2B.LenzeStatus |= *(ec_slave[slave].inputs + 19);
                InputData2B.ModeStatus = *(ec_slave[slave].inputs + 21);
                InputData2B.ErrorCode = *(ec_slave[slave].inputs + 23);
                InputData2B.ErrorCode <<= 8;
                InputData2B.ErrorCode |= *(ec_slave[slave].inputs + 22);
                InputData2B.VelValue |= *(ec_slave[slave].inputs + 27);
                InputData2B.VelValue <<= 8;
                InputData2B.VelValue |= *(ec_slave[slave].inputs + 26);
                InputData2B.VelValue <<= 8;
                InputData2B.VelValue |= *(ec_slave[slave].inputs + 25);
                InputData2B.VelValue <<= 8;
                InputData2B.VelValue |= *(ec_slave[slave].inputs + 24);
                InputData2B.TorValue = 0;
                InputData2B.TorValue |= *(ec_slave[slave].inputs + 29);
                InputData2B.TorValue <<= 8;
                InputData2B.TorValue |= *(ec_slave[slave].inputs + 28);
                InputData2B.PosiValue |= *(ec_slave[slave].inputs + 33);
                InputData2B.PosiValue <<= 8;
                InputData2B.PosiValue |= *(ec_slave[slave].inputs + 32);
                InputData2B.PosiValue <<= 8;
                InputData2B.PosiValue |= *(ec_slave[slave].inputs + 31);
                InputData2B.PosiValue <<= 8;
                InputData2B.PosiValue |= *(ec_slave[slave].inputs + 30);



                str.sprintf("0x%04x",InputData2A.StatusWord);
                ui->Stawordlabel->setText(str);//status word
                str.sprintf("0x%04x",InputData2A.LenzeStatus);
                ui->LenzeStalabel->setText(str);//lenze status
                str.sprintf("0x%02x",InputData2A.ModeStatus);
                ui->MotStalabel->setText(str);//mode of operation display
                str.sprintf("0x%04x",InputData2A.ErrorCode);
                ui->errcodelabel->setText(str);//error code
                str.sprintf("%d",InputData2A.VelValue);
                ui->VelVallabel->setText(str);//velocity actual value
                str.sprintf("%d",InputData2A.TorValue);
                ui->TorVallabel->setText(str);//Torque actual value
                str.sprintf("%d",InputData2A.PosiValue);
                ui->Posilabel->setText(str);//position actual value
            }
            else
            {
                //nothing
            }
        }

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
    TargetPosi += 10000;


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
    TargetPosi -= 10000;
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


