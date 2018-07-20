#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkInterface>
#include <QTimer>
#include <pthread.h>
#include "initxml.h"


#define SINGLE_CODE 0x69070001
#define DOUBLE_CODE 0x69070002
#define EK1100_ID   0x44c2c52
#define EL1008_ID   0x3f03052
#define EL2008_ID   0x7d83052
#define DIO16P16R   0x00000101
#define SINGLE_CONGIG_NUM 288
#define DOUBLE_CONGIG_NUM 592

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    initxml *initxmll;
    char IOmap[4096];
    unsigned int SDODATA2[DOUBLE_CONGIG_NUM * 4];
private slots:
    void on_startButton_clicked();
    void UpStatus();
    void enable_motion_thread();
    void disable_motion_thread();
    void enable_motion_thread2A();
    void disable_motion_thread2A();
    void enable_motion_thread2B();
    void disable_motion_thread2B();
    void Single_SDOWrite(int);
    void Double_SDOWrite(int);

    void on_stopButton_clicked();

    void on_clearButton_clicked();

    void on_initButton_clicked();

    void on_preopButton_clicked();

    void on_safeopButton_clicked();

    void on_opButton_clicked();

    void on_enableButton_clicked();

    void on_forwardButton_clicked();

    void on_resetButton_pressed();

    void on_resetButton_released();

    void on_reverseButton_clicked();

    void on_cfgButton_clicked();


    void on_forwardButton_2A_clicked();

    void on_resetButton_2A_clicked();

    void on_resetButton_2B_clicked();

    void on_enableButton_2A_clicked();

    void on_forwardButton_2B_clicked();

    void on_reverseButton_2A_clicked();

    void on_reverseButton_2B_clicked();

    void on_enableButton_2B_clicked();


private:
    Ui::MainWindow *ui;
    QTimer *timer0, *disable_timer, *enable_timer;

    int Count,MotorStaCount,MotorStaCount2A,MotorStaCount2B;
    int32_t TargetPosi;
    int32_t TargetPosi2A;
    int32_t TargetPosi2B;
    bool MotorEnable;
    bool MotorEnable2A;
    bool MotorEnable2B;
    bool SendPdo;
    uint8_t MotorStatus;
    uint8_t MotorStatus2A;
    uint8_t MotorStatus2B;
    typedef struct
    {
        uint16_t ControlWord;
        uint8_t ModeCmd;
        int32_t TargetPosi;
        uint16_t PosTorLim;
        uint16_t NegTorLim;

    }PdoOutput;
    typedef struct
    {
        uint16_t StatusWord;
        uint16_t LenzeStatus;
        uint8_t ModeStatus;
        uint16_t ErrorCode;
        int32_t VelValue;
        int16_t TorValue;
        int32_t PosiValue;
    }PdoInput;
    PdoOutput OutputData;
    PdoInput InputData;
    PdoOutput OutputData2A;
    PdoInput InputData2A;
    PdoOutput OutputData2B;
    PdoInput InputData2B;




    typedef enum
    {
        MOTOR_NOT_READY         =   0x00,
        MOTOR_SWITCH_DISABLED   =   0x01,
        MOTOR_READY_SWITCH_ON   =   0x02,
        MOTOR_SWITCH_ON         =   0x03,
        MOTOR_ENABLE            =   0x04,
        MOTOR_QUICK_STOP        =   0x05,
        MOTOR_FAULT_REACTION    =   0x06,
        MOTOR_FAULT             =   0x07

    }motorstatus;
};

#endif // MAINWINDOW_H
