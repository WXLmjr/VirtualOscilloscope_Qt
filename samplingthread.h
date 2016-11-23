#include <qwt_sampling_thread.h>

class SamplingThread: public QwtSamplingThread
{
    Q_OBJECT

public:
    SamplingThread( QObject *parent = NULL );

    double frequency() const;
    double amplitude() const;

public Q_SLOTS:
    void setAmplitude( double );
    void setFrequency( double );

protected:

    //elapsed参数表示：线程开始以来经过的时间（以毫秒为单位）
    virtual void sample( double elapsed );

private:
    virtual double value( double timeStamp ) const;

    double d_frequency;     //频率属性
    double d_amplitude;     //振幅属性
};
