#ifndef _SIGNAL_DATA_H_
#define _SIGNAL_DATA_H_ 1

#include <qrect.h>

class SignalData
{
public:
    //类的成员函数前加static，则无需实例化对象，就可调用此函数
    static SignalData &instance();

    void append( const QPointF &pos );
    void clearStaleValues( double min );

    int size() const;
    QPointF value( int index ) const;

    QRectF boundingRect() const;

    void lock();
    void unlock();

private:
    SignalData();
    SignalData( const SignalData & );
    SignalData &operator=( const SignalData & );

    virtual ~SignalData();

    class PrivateData;
    PrivateData *d_data;
};

#endif
