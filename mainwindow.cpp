#include "mainwindow.h"
#include "plot.h"
#include "knob.h"
#include "wheelbox.h"
#include <qwt_scale_engine.h>
#include <qlabel.h>
#include <qlayout.h>

MainWindow::MainWindow( QWidget *parent ):
    QWidget( parent )
{
    const double intervalLength = 1.0; // seconds

    /* 实例化主窗口的各种控件 */
    d_plot = new Plot( this );
    d_plot->setIntervalLength( intervalLength );

    d_amplitudeKnob = new Knob( "Amplitude", 0.0, 200.0, this );
    d_amplitudeKnob->setValue( 100.0 );

    d_frequencyKnob = new Knob( "Frequency [Hz]", 0.1, 20.0, this );
    d_frequencyKnob->setValue( 10.0 );

    d_intervalWheel = new WheelBox( "Displayed [s]", 1.0, 100.0, 1.0, this );
    d_intervalWheel->setValue( intervalLength );

    d_timerWheel = new WheelBox( "Sample Interval [ms]", 0.0, 20.0, 0.1, this );
    d_timerWheel->setValue( 1.0 );


    //主窗口布局
    QVBoxLayout* vLayout1 = new QVBoxLayout();
    vLayout1->addWidget( d_intervalWheel );
    vLayout1->addWidget( d_timerWheel );
    vLayout1->addStretch( 10 );
    vLayout1->addWidget( d_amplitudeKnob );
    vLayout1->addWidget( d_frequencyKnob );

    //主窗口布局
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->addWidget( d_plot, 10 );
    layout->addLayout( vLayout1 );

    /* 一个信号可以连接到另一个信号，用来传递一个信号 */
    connect( d_amplitudeKnob, SIGNAL( valueChanged( double ) ),
        SIGNAL( amplitudeChanged( double ) ) );
    connect( d_frequencyKnob, SIGNAL( valueChanged( double ) ),
        SIGNAL( frequencyChanged( double ) ) );
    connect( d_timerWheel, SIGNAL( valueChanged( double ) ),
        SIGNAL( signalIntervalChanged( double ) ) );

    /*
     * 连接采样间隔调节齿轮箱的valueChanged信号与画波形控件
     * d_plot的槽函数setIntervalLength用来设置采样间隔长度
     */
    connect( d_intervalWheel, SIGNAL( valueChanged( double ) ),
        d_plot, SLOT( setIntervalLength( double ) ) );
}

void MainWindow::start()
{
    d_plot->start();
}

double MainWindow::frequency() const
{
    return d_frequencyKnob->value();
}

double MainWindow::amplitude() const
{
    return d_amplitudeKnob->value();
}

double MainWindow::signalInterval() const
{
    return d_timerWheel->value();
}
