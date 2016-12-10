#include "wheelbox.h"
#include <qwt_wheel.h>
#include <qlcdnumber.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qevent.h>
#include <qapplication.h>
#include <qdebug.h>


/* 此事件滤波对象的作用是想在整个WheelBox组件区域（包括LCD控件区域）滚动鼠标滚轮时，QwtWheel的值都能改变 */
class Wheel: public QwtWheel
{
public:
    Wheel( WheelBox *parent ):
        QwtWheel( parent )
    {
        setFocusPolicy( Qt::WheelFocus );
        parent->installEventFilter( this );
    }

    virtual bool eventFilter( QObject *object, QEvent *event )
    {
        if ( event->type() == QEvent::Wheel )
        {
            const QWheelEvent *we = static_cast<QWheelEvent *>( event );




            /*
             * void QwtWheel::wheelEvent( QWheelEvent *event )
             * {
             *      if ( !wheelRect().contains( event->pos() ) )
             *      {
             *          event->ignore();
             *          return;
             *      }
             *      ………………
             * }
             *
             * 以上代码是从QwtWheel里面摘抄出来的，可以看出有ignore的分支
             * 看看QPoint( 5, 5 )什么意思？
             */
            QWheelEvent wheelEvent( QPoint( 5, 5 ), we->delta(),
                we->buttons(), we->modifiers(),
                we->orientation() );

            /* 注释掉此行代码，在wheel上面滚动滚轮，程序就不会崩溃了 */
            /*
             * 原因分析，若在Wheel的wheelEvent()函数中ignore了Event，则Event会继续向其父祖件WheelBox传播，
             * 而传向WheelBox的Event会首先进入此事件滤波函数，然后又会通过sendEvent()函数发送给Wheel，再被
             * ignore，再传给父祖件WheelBox……陷入死循环，从而程序崩溃。
             */
//            QApplication::sendEvent( this, &wheelEvent );

            /* WXLmjr于2016-12-11日屏蔽上一行代码，修改为下面这行代码，解决程序崩溃问题，
             * 但直接调用event()函数和调用sendEvent()函数有什么区别？导致了崩溃的问题？
             */
            this->event(&wheelEvent);

            return true;
        }
        return QwtWheel::eventFilter( object, event );
    }


//    virtual void wheelEvent( QWheelEvent *event)
//    {
//        static int i = 0;
////        event->accept();
////        event->ignore();

//        QwtWheel::wheelEvent(event);
//        qDebug() << i++;
//    }
};





WheelBox::WheelBox( const QString &title,
        double min, double max, double stepSize, QWidget *parent ):
    QWidget( parent )
{

    d_number = new QLCDNumber( this );
    d_number->setSegmentStyle( QLCDNumber::Filled );
    d_number->setAutoFillBackground( true );
    d_number->setFixedHeight( d_number->sizeHint().height() * 2 );
    d_number->setFocusPolicy( Qt::WheelFocus );

    QPalette pal( Qt::black );
    pal.setColor( QPalette::WindowText, Qt::green );
    d_number->setPalette( pal );

    d_wheel = new Wheel( this );
    d_wheel->setOrientation( Qt::Vertical );
    d_wheel->setInverted( true );
    d_wheel->setRange( min, max );
    d_wheel->setSingleStep( stepSize );
    d_wheel->setPageStepCount( 5 );
    d_wheel->setFixedHeight( d_number->height() );

    d_number->setFocusProxy( d_wheel );

    QFont font( "Helvetica", 10 );
    font.setBold( true );

    d_label = new QLabel( title, this );
    d_label->setFont( font );

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins( 0, 0, 0, 0 );
    hLayout->setSpacing( 2 );
    hLayout->addWidget( d_number, 10 );
    hLayout->addWidget( d_wheel );

    QVBoxLayout *vLayout = new QVBoxLayout( this );
    vLayout->addLayout( hLayout, 10 );
    vLayout->addWidget( d_label, 0, Qt::AlignTop | Qt::AlignHCenter );

    connect( d_wheel, SIGNAL( valueChanged( double ) ),
        d_number, SLOT( display( double ) ) );
    connect( d_wheel, SIGNAL( valueChanged( double ) ),
        this, SIGNAL( valueChanged( double ) ) );
}

void WheelBox::setTheme( const QColor &color )
{
    d_wheel->setPalette( color );
}

QColor WheelBox::theme() const
{
    return d_wheel->palette().color( QPalette::Window );
}

void WheelBox::setValue( double value )
{
    d_wheel->setValue( value );
    d_number->display( value );
}

double WheelBox::value() const
{
    return d_wheel->value();
}


//bool  WheelBox::eventFilter( QObject *object, QEvent *event )
//{
//    if ( event->type() == QEvent::Wheel )
//    {
//        const QWheelEvent *we = static_cast<QWheelEvent *>( event );

//        QWheelEvent wheelEvent( QPoint( 5, 5 ), we->delta(),
//            we->buttons(), we->modifiers(),
//            we->orientation() );

//        /* 注释掉此行代码，在wheel上面滚动滚轮，程序就不会崩溃了 */
//        QApplication::sendEvent( d_wheel, &wheelEvent );
//        return true;
//    }
//    return QwtWheel::eventFilter( object, event );
//}
