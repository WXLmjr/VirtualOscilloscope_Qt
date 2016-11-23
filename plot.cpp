#include "plot.h"
#include "curvedata.h"
#include "signaldata.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include <qevent.h>

class Canvas: public QwtPlotCanvas      //定义Canvas画布类
{
public:
    Canvas( QwtPlot *plot = NULL ):
        QwtPlotCanvas( plot )
    {
        // The backing store is important, when working with widget
        // overlays ( f.e rubberbands for zooming ).
        // Here we don't have them and the internal
        // backing store of QWidget is good enough.

        setPaintAttribute( QwtPlotCanvas::BackingStore, false );
        setBorderRadius( 10 );

        if ( QwtPainter::isX11GraphicsSystem() )
        {
#if QT_VERSION < 0x050000
            // Even if not liked by the Qt development, Qt::WA_PaintOutsidePaintEvent
            // works on X11. This has a nice effect on the performance.

            setAttribute( Qt::WA_PaintOutsidePaintEvent, true );
#endif

            // Disabling the backing store of Qt improves the performance
            // for the direct painter even more, but the canvas becomes
            // a native window of the window system, receiving paint events
            // for resize and expose operations. Those might be expensive
            // when there are many points and the backing store of
            // the canvas is disabled. So in this application
            // we better don't disable both backing stores.

            if ( testPaintAttribute( QwtPlotCanvas::BackingStore ) )
            {
                setAttribute( Qt::WA_PaintOnScreen, true );
                setAttribute( Qt::WA_NoSystemBackground, true );
            }
        }

        setupPalette();
    }

private:
    void setupPalette()
    {
        QPalette pal = palette();

#if QT_VERSION >= 0x040400
        QLinearGradient gradient;
        gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
        gradient.setColorAt( 0.0, QColor( 0, 49, 110 ) );
        gradient.setColorAt( 1.0, QColor( 0, 87, 174 ) );

        pal.setBrush( QPalette::Window, QBrush( gradient ) );
#else
        pal.setBrush( QPalette::Window, QBrush( color ) );
#endif

        // QPalette::WindowText is used for the curve color
        // 设置曲线的颜色
        pal.setColor( QPalette::WindowText, Qt::yellow );

        setPalette( pal );
    }
};














Plot::Plot( QWidget *parent ):           //定义Plot类的构造函数
    QwtPlot( parent ),
    d_paintedPoints( 0 ),               //初始化Plot类的部分数据成员
    d_interval( 0.0, 10.0 ),
    d_timerId( -1 )
{
    d_directPainter = new QwtPlotDirectPainter();

    setAutoReplot( false );
    setCanvas( new Canvas() );

    plotLayout()->setAlignCanvasToScales( true );

    setAxisTitle( QwtPlot::xBottom, "Time [s]" );
    setAxisScale( QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue() );
    setAxisScale( QwtPlot::yLeft, -200.0, 200.0 );

    /* 画网格 */
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( this );

    /* 画X-Y轴直线*/
    d_origin = new QwtPlotMarker();
    d_origin->setLineStyle( QwtPlotMarker::Cross );  //Crosshair十字光标
    d_origin->setValue( d_interval.minValue() + d_interval.width() / 2.0, 0.0 );
    d_origin->setLinePen( Qt::gray, 0.0, Qt::DashLine );
    d_origin->attach( this );

    /* 新建绘制曲线PlotCurve类对象 */
    d_curve = new QwtPlotCurve();
    d_curve->setStyle( QwtPlotCurve::Lines );
    d_curve->setPen( canvas()->palette().color( QPalette::WindowText ) );
    d_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    d_curve->setPaintAttribute( QwtPlotCurve::ClipPolygons, false );
    d_curve->setData( new CurveData() );  //实例化一个曲线数据CurveData类对象，并将其设置为绘制曲线对象的数据
    d_curve->attach( this );
}

Plot::~Plot()
{
    delete d_directPainter;
}

void Plot::start()
{
    d_clock.start();                //启动用于计时的系统时钟
    d_timerId = startTimer( 10 );
}

/* 重绘 */
void Plot::replot()
{
    //用法：static_cast < type-id > ( expression )
    //该运算符把expression转换为type-id类型，但没有运行时类型检查来保证转换的安全性。
    //将QwtSeriesData指针类型显示强制转换为CurveData指针类型
    //CurveData类继承自QwtSeriesData类
    CurveData *data = static_cast<CurveData *>( d_curve->data() );
    data->values().lock();

    QwtPlot::replot();
    d_paintedPoints = data->size();

    data->values().unlock();
}

/* 设置区间长度并重绘 */
void Plot::setIntervalLength( double interval )
{
    if ( interval > 0.0 && interval != d_interval.width() )
    {
        //设置区间的最大值
        d_interval.setMaxValue( d_interval.minValue() + interval );

        //设置X轴的比例
        setAxisScale( QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue() );

        //重绘
        replot();
    }
}

/* 更新曲线，在定时器事件中周期性调用 */
void Plot::updateCurve()
{
    CurveData *data = static_cast<CurveData *>( d_curve->data() );
    data->values().lock();

    const int numPoints = data->size();
    if ( numPoints > d_paintedPoints )
    {
        const bool doClip = !canvas()->testAttribute( Qt::WA_PaintOnScreen );
        if ( doClip )
        {
            /*
                Depending on the platform setting a clip might be an important
                performance issue. F.e. for Qt Embedded this reduces the
                part of the backing store that has to be copied out - maybe
                to an unaccelerated frame buffer device.
            */

            const QwtScaleMap xMap = canvasMap( d_curve->xAxis() );
            const QwtScaleMap yMap = canvasMap( d_curve->yAxis() );

            QRectF br = qwtBoundingRect( *data,
                d_paintedPoints - 1, numPoints - 1 );

            const QRect clipRect = QwtScaleMap::transform( xMap, yMap, br ).toRect();

            /* 设置增量式画家的剪切区域 */
            d_directPainter->setClipRegion( clipRect );
        }

        /* 使用增量式画家，绘制一系列点 */
        /*
         * 由此可以看出，如果没有缩放绘图比例，紧紧增加了一系列点，则不会调用replot函数，而是
         * 调用增量式绘图函数，这样可以更高效
         */
        d_directPainter->drawSeries( d_curve, d_paintedPoints - 1, numPoints - 1 );
        d_paintedPoints = numPoints;
    }

    data->values().unlock();
}

/* 增加区间，并重绘：当一屏数据显示完成，需要重头显示的时候调用 */
void Plot::incrementInterval()
{
    d_interval = QwtInterval( d_interval.maxValue(),
        d_interval.maxValue() + d_interval.width() );

    CurveData *data = static_cast<CurveData *>( d_curve->data() );
    data->values().clearStaleValues( d_interval.minValue() );

    // To avoid, that the grid is jumping, we disable
    // the autocalculation of the ticks and shift them
    // manually instead.

    QwtScaleDiv scaleDiv = axisScaleDiv( QwtPlot::xBottom );
    scaleDiv.setInterval( d_interval );

    for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
    {
        QList<double> ticks = scaleDiv.ticks( i );
        for ( int j = 0; j < ticks.size(); j++ )
            ticks[j] += d_interval.width();
        scaleDiv.setTicks( i, ticks );
    }
    setAxisScaleDiv( QwtPlot::xBottom, scaleDiv );

    d_origin->setValue( d_interval.minValue() + d_interval.width() / 2.0, 0.0 );

    d_paintedPoints = 0;
    replot();
}

void Plot::timerEvent( QTimerEvent *event ) //定时器事件
{
    if ( event->timerId() == d_timerId )
    {
        updateCurve();                      //更新曲线

        const double elapsed = d_clock.elapsed() / 1000.0;
        if ( elapsed > d_interval.maxValue() )  //如果已经过去的时间大于区间的最大值，则更新区间
            incrementInterval();                 //增加区间

        return;
    }

    QwtPlot::timerEvent( event );
}

void Plot::resizeEvent( QResizeEvent *event )     //重定义尺寸事件
{
    d_directPainter->reset();
    QwtPlot::resizeEvent( event );
}

void Plot::showEvent( QShowEvent * )
{
    replot();
}

bool Plot::eventFilter( QObject *object, QEvent *event )    //事件滤波器
{
    if ( object == canvas() && 
        event->type() == QEvent::PaletteChange )
    {
        d_curve->setPen( canvas()->palette().color( QPalette::WindowText ) );
    }

    return QwtPlot::eventFilter( object, event );
}
