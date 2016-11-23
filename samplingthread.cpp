#include "samplingthread.h"
#include "signaldata.h"
#include <qwt_math.h>
#include <math.h>

#if QT_VERSION < 0x040600
#define qFastSin(x) ::sin(x)
#endif

SamplingThread::SamplingThread( QObject *parent ):
    QwtSamplingThread( parent ),
    d_frequency( 5.0 ),
    d_amplitude( 20.0 )
{
}

void SamplingThread::setFrequency( double frequency )
{
    d_frequency = frequency;
}

double SamplingThread::frequency() const
{
    return d_frequency;
}

void SamplingThread::setAmplitude( double amplitude )
{
    d_amplitude = amplitude;
}

double SamplingThread::amplitude() const
{
    return d_amplitude;
}

//elapsed代表随着时间流逝，x轴的值。
void SamplingThread::sample( double elapsed )
{
    if ( d_frequency > 0.0 )
    {
        //通过x的值，计算y轴的值。
        const QPointF s( elapsed, value( elapsed ) );

        //将产生的采样点添加到静态局部类SignalData对象valueVector
        SignalData::instance().append( s );
    }
}

//y轴与x轴的函数关系，此处为正弦函数y = Asin(wt)
double SamplingThread::value( double timeStamp ) const
{
    const double period = 1.0 / d_frequency;

    /* 产生正弦波形的代码 */
    const double x = ::fmod( timeStamp, period );   //fmod() 用来对浮点数进行取模（求余）
    const double v = d_amplitude * qFastSin( x / period * 2 * M_PI );

    /* 产生正切函数波形的代码 */
//    const double x = ::fmod( timeStamp, period );   //fmod() 用来对浮点数进行取模（求余）
//    const double v = (qFastSin( x / period * 2 * M_PI )) / (qFastCos( x / period * 2 * M_PI ));

    /* 产生三角波的代码 */
//    const double x = ::fmod( timeStamp, period );   //fmod() 用来对浮点数进行取模（求余）
//    double v = 0;
//    if(x<(period/2.0)) {
//        v = d_amplitude * (2.0*x/period) - d_amplitude/2.0;
//    } else {
//        v = d_amplitude * (-2.0*x/period + 2) - d_amplitude/2.0;
//    }

    /* 产生锯齿波的代码 */
//    const double x = ::fmod( timeStamp, period );   //fmod() 用来对浮点数进行取模（求余）
//    const double v = d_amplitude * x/period - d_amplitude/2.0;

    return v;
}
