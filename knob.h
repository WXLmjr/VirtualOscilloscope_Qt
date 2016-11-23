#ifndef _KNOB_H_
#define _KNOB_H_

#include <qwidget.h>

class QwtKnob;
class QLabel;

class Knob: public QWidget                  //继承自QWidget的旋钮类
{
    Q_OBJECT

    Q_PROPERTY( QColor theme READ theme WRITE setTheme )

public:
    Knob( const QString &title,
        double min, double max, QWidget *parent = NULL );

    virtual QSize sizeHint() const;

    void setValue( double value );
    double value() const;

    void setTheme( const QColor & );
    QColor theme() const;

Q_SIGNALS:
    double valueChanged( double );

protected:
    virtual void resizeEvent( QResizeEvent * );

    //将一个QwtKnob（旋钮类）和一个QLabel（标签类）组合成一个Knob类
private:
    QwtKnob *d_knob;
    QLabel *d_label;
};

#endif
