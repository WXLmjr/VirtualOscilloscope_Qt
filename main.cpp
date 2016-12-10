#include <qapplication.h>
#include "mainwindow.h"
#include "samplingthread.h"
#include <QPalette>

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    app.setPalette( Qt::cyan );             //用调色板设置应用程序的背景颜色

    MainWindow window;                      //实例化一个MainWindow类对象
    window.resize( 800, 400 );              //重新定义主窗口尺寸

    //继承关系：class SamplingThread: public QwtSamplingThread
    //继承关系：class QWT_EXPORT QwtSamplingThread: public QThread
    SamplingThread samplingThread;                          //实例化一个采样线程对象
    samplingThread.setFrequency( window.frequency() );      //设置频率
    samplingThread.setAmplitude( window.amplitude() );      //设置振幅
    samplingThread.setInterval( window.signalInterval() );  //信号采样间隔

    /* 连接主窗口旋钮变化信号和采样进程的相应槽函数 */
    window.connect( &window, SIGNAL( frequencyChanged( double ) ),
        &samplingThread, SLOT( setFrequency( double ) ) );
    window.connect( &window, SIGNAL( amplitudeChanged( double ) ),
        &samplingThread, SLOT( setAmplitude( double ) ) );
    window.connect( &window, SIGNAL( signalIntervalChanged( double ) ),
        &samplingThread, SLOT( setInterval( double ) ) );

    window.show();                  //显示mainwindow

    samplingThread.start();         //启动采样线程
    window.start();                 //开始window

    bool ok = app.exec();           //进入主事件循环，直道退出程序，结束循环，才能执行下面的程序

    samplingThread.stop();          //停止采样线程
    samplingThread.wait( 1000 );

    return ok;
}
