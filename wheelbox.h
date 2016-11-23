#ifndef _WHEELBOX_H_
#define _WHEELBOX_H_

#include <qwidget.h>

class QwtWheel;
class QLabel;
class QLCDNumber;

class WheelBox: public QWidget
{
    Q_OBJECT
    Q_PROPERTY( QColor theme READ theme WRITE setTheme )

public:
    WheelBox( const QString &title,
        double min, double max, double stepSize,
        QWidget *parent = NULL );

    void setTheme( const QColor & );
    QColor theme() const;

    void setUnit( const QString & );
    QString unit() const;

    void setValue( double value );
    double value() const;

Q_SIGNALS:
    double valueChanged( double );

    //将一个QLCDNumber（数码管显示控件类），一个QwtWheel（滚轮类），一个QLable（标签类）组合成一个WeelBox类
private:
    QLCDNumber *d_number;
    QwtWheel *d_wheel;
    QLabel *d_label;

    QString d_unit;
};

#endif
