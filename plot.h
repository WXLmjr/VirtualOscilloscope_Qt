#include <qwt_plot.h>
#include <qwt_interval.h>
#include <qwt_system_clock.h>

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotDirectPainter;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget * = NULL );
    virtual ~Plot();

    void start();
    virtual void replot();                              //重绘

    virtual bool eventFilter( QObject *, QEvent * );    //事件滤波器

public Q_SLOTS:
    void setIntervalLength( double );               //设置区间长度

protected:
    virtual void showEvent( QShowEvent * );         //显示事件
    virtual void resizeEvent( QResizeEvent * );     //重定义大小事件
    virtual void timerEvent( QTimerEvent * );       //定时器事件，覆盖QObject类的timerEvent()函数

private:
    void updateCurve();         //更新曲线
    void incrementInterval();   //增加区间

    /*
     * 一个Marker可以是一条水平线、一条垂直线、一个符号、一个标签或他们的任何组合，
     * 它可以画在一个中心点的周围，也可以画在一个边界矩形内部。
     */
    QwtPlotMarker *d_origin;    //定义一个Qwt标识类对象指针，标识坐标原点

    /*
     * QwtPlotCurve：一个绘图项，表示一系列的点。
     * 曲线是在X-Y平面上的一系列点的表示。它支持不同的显示风格，插值（铁条）和符号。
     */
    QwtPlotCurve *d_curve;      //定义一个Qwt曲线类对象指针

    int d_paintedPoints;        //已经画了的点

    /*
     * 增量式Painter对象
     * qwtplotdirectpainter提供了一个API paint子集（f.e补充点），无需擦除/重绘图的画布。
     */
    QwtPlotDirectPainter *d_directPainter;

    /* 区间由两个double类型的数表示，分别代表上下限 */
    QwtInterval d_interval;     //实例化一个Qwt区间类对象
    int d_timerId;              //定时器ID

    /*
     * QwtSystemClock提供一个高分辨率时钟
     * 有时候QTime（毫秒分辨率）提供的分辨率对进行时间测量不够精确。
     * QwtSystemClock提供了QTime功能的子集，使用更好分辨率的定时器（在允许的情况下）
     */
    QwtSystemClock d_clock;     //实例化一个系统时钟对象
};
