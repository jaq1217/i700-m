#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkInterface>

#include <QTimer>
#include <pthread.h>
#include "initxml.h"
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

private slots:
    void on_startButton_clicked();
    void UpStatus();
    void enable_motion_thread();
    void disable_motion_thread();

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


    void on_resetButton_clicked();

    void on_forwardButton_pressed();

    void on_reverseButton_pressed();

    void on_cfgButton_clicked();

    void on_SDO_read_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer0, *disable_timer, *enable_timer;

    int Count,MotorStaCount;
    int32_t TargetPosi;
    bool MotorEnable;
    bool SendPdo;
    uint8_t MotorStatus;
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



    PdoOutput OutputData = {
        0,      //.ControlWord
        0,      //.ModeCmd
        0,      //.TargetPosi
        0,      //.PosTorLim
        0       //.NegTorLim

    };
    PdoInput InputData ={
        0,      //.StatusWord
        0,      //.LenzeStatus
        0,      //.ModeStatus
        0,      //.ErrorCode
        0,      //.VelValue
        0,      //.TorValue
        0       //.PosiValue
    };
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
