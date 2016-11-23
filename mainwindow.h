#include <qwidget.h>

class Plot;
class Knob;
class WheelBox;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow( QWidget * = NULL );     //构造函数

    void start();                       //调用d_plot控件的start()函数

    double amplitude() const;           //返回振幅旋钮的值
    double frequency() const;           //返回频率旋钮的值
    double signalInterval() const;      //返回显示时间齿轮箱的值

Q_SIGNALS:
    void amplitudeChanged( double );
    void frequencyChanged( double );
    void signalIntervalChanged( double );

private:
    Knob *d_frequencyKnob;          //信号频率调节旋钮
    Knob *d_amplitudeKnob;          //信号振幅调节旋钮
    WheelBox *d_timerWheel;         //显示时间调节齿轮箱
    WheelBox *d_intervalWheel;      //采样间隔调节齿轮箱

    Plot *d_plot;                   //画波形控件
};
