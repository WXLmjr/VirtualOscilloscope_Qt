#include "signaldata.h"
#include <qvector.h>
#include <qmutex.h>
#include <qreadwritelock.h>

//PrivateData类，为SignalData的成员类
class SignalData::PrivateData
{
public:
    PrivateData():boundingRect( 1.0, 1.0, -2.0, -2.0 ) // invalid无效的
    {
        values.reserve( 1000 );                        //申请为values分配1000个元素内存空间
    }

    inline void append( const QPointF &sample )
    {
        values.append( sample );                       //在数组的末尾插入一个值

        // adjust the bounding rectangle    调节边界限制矩形
        if ( boundingRect.width() < 0 || boundingRect.height() < 0 )
        {
            boundingRect.setRect( sample.x(), sample.y(), 0.0, 0.0 );
        }
        else
        {
            boundingRect.setRight( sample.x() );

//            怀疑此处逻辑有误，用下面的代码代替例程的代码
//            if ( sample.y() > boundingRect.bottom() )
//                boundingRect.setBottom( sample.y() );

//            if ( sample.y() < boundingRect.top() )
//                boundingRect.setTop( sample.y() );


            if ( sample.y() < boundingRect.bottom() )
                boundingRect.setBottom( sample.y() );

            if ( sample.y() > boundingRect.top() )
                boundingRect.setTop( sample.y() );
        }
    }

    QReadWriteLock lock;                 //多线程同步工具
    QVector<QPointF> values;             //使用QVector模板类定义一个浮点动态数组，值
    QRectF boundingRect;                //实例化一个矩形类对象：边界限制矩形

    QMutex mutex;                        //实例化一个互斥量对象，protecting pendingValues
//    QVector<QPointF> pendingValues;     //使用QVector模板类定义一个浮点动态数组，挂起值
};

SignalData::SignalData()                //SignalData构造函数
{
    d_data = new PrivateData();         //实例化一个PrivateData对象
}

SignalData::~SignalData()               //SignalData析构函数
{
    delete d_data;
}

int SignalData::size() const            //返回QVector<QPointF> values的大小
{
    return d_data->values.size();
}

QPointF SignalData::value( int index ) const    //返回QVector<QPointF> values的某个元素值
{
    return d_data->values[index];
}

QRectF SignalData::boundingRect() const
{
    return d_data->boundingRect;
}

void SignalData::lock()
{
    d_data->lock.lockForRead();
}

void SignalData::unlock()
{
    d_data->lock.unlock();
}

void SignalData::append( const QPointF &sample )//将pendingValues添加到values
{
    d_data->mutex.lock();
//    d_data->pendingValues += sample;//This is an overloaded function. Appends value to the vector.

    const bool isLocked = d_data->lock.tryLockForWrite();
    if ( isLocked )
    {

        /* 经过测试d_data->pendingValues每次只缓存了一个样本点，没有什么存在的意义，注释掉不影响程序功能 */
//        const int numValues = d_data->pendingValues.size();

//        //Returns a pointer to the data stored in the vector.
//        //The pointer can be used to access and modify the items in the vector.
//        const QPointF *pendingValues = d_data->pendingValues.data();

//        for ( int i = 0; i < numValues; i++ )
//            d_data->append( pendingValues[i] );  //将pendingValues添加到values

//        d_data->pendingValues.clear();          //清除挂起值数组

        d_data->append(sample);


        d_data->lock.unlock();
    }

    d_data->mutex.unlock();
}

/* 当一屏数据显示完成，需要重头显示的时需要清楚上一屏幕的旧数据     */
/* 在函数void Plot::incrementInterval()中调用                 */
void SignalData::clearStaleValues( double limit ) //清除values中的旧值
{
    d_data->lock.lockForWrite();

    d_data->boundingRect = QRectF( 1.0, 1.0, -2.0, -2.0 ); // invalid

    const QVector<QPointF> values = d_data->values;
    d_data->values.clear();
    d_data->values.reserve( values.size() );

    int index;
    for ( index = values.size() - 1; index >= 0; index-- )
    {
        if ( values[index].x() < limit )
            break;
    }

    if ( index > 0 )
        d_data->append( values[index++] );

    while ( index < values.size() - 1 )
        d_data->append( values[index++] );

    d_data->lock.unlock();
}

//实例化一个SignalData并返回其引用
SignalData &SignalData::instance()
{
    //定义一个静态局部类对象，生命周期为整个程序
    static SignalData valueVector;
    return valueVector;
}
