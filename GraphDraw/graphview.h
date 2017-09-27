#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QObject>
#include <QWidget>
#include <QRectF>
#include <QPair>

enum struct ZeScaleType
{
    LINEAR, LINEAR_ORTHONORMAL, X_LOG, Y_LOG, XY_LOG,
};

struct ZeLogBase
{
    double base;
    double powerNumerator, powerDenominator;
};

enum struct ZeGridType
{
    NO_GRID, GRID, GRID_SUBGRID
};

struct ZeGridSettings
{
    ZeGridType gridType;
    unsigned int horSubGridLineCount, verSubGridLineCount;
    // the number of lines of the sub grid between two lines of the main one.
};

struct ZeTickCoordinate
{
    QString baseStr;
    double base;
    double baseMultiplier;
    int powerNumerator, powerDenominator;
};

struct ZeMainGridLine
{
    double pos; // no need to differenciate between log and linear view since it's seamless for the plotter
    ZeTickCoordinate coordinate;
};

struct ZeSubGridLine
{
    double pos;
    int numerator, denominator;
};

struct ZeOneDirectionGrid
{
    QList<ZeMainGridLine> mainGrid;
    QList<ZeSubGridLine> subGrid;
};

struct ZeGrid
{
    ZeOneDirectionGrid horizontal, vertical;
};


class ZeGraphView : public QObject
{
    Q_OBJECT
public:
    explicit ZeGraphView(QWidget *viewWidget = 0, QObject *parent = 0);
    ZeGraphView(const ZeGraphView &other, QObject *parent = 0);

    ZeGraphView& operator=(const ZeGraphView &other);


    void zoomYview(double ratio);
    void zoomXview(double ratio);
    void zoomView(QPointF center, double ratio);
    void translateView(QPointF vec);
    
    void setViewXmin(double val);
    void setViewXmax(double val);
    void setViewYmin(double val);
    void setViewYmax(double val);

    void setlgXmin(double val);
    void setlgXmax(double val);
    void setlgYmin(double val);
    void setlgYmax(double val);

    void setXmin(double val);
    void setXmax(double val);
    void setYmin(double val);
    void setYmax(double val);

    double getXmin();
    double getXmax();
    double getYmin();
    double getYmax();

    double viewToUnit_y(double viewY) const;
    double unitToView_y(double unitY) const ;

    double viewToUnit_x(double viewX) const ;
    double unitToView_x(double unitX) const ;

    QRectF rect() const ;
    QRectF lgRect() const ;
    QRectF viewRect() const ;
    void setViewRect(QRectF rect);

    ZeScaleType viewType() const ;

    ZeGrid getGrid();
    ZeGridType gridType() const;
    void setGridType(ZeGridType type) const;

signals:

public slots:
    void setWindowRect(QRectF rect);

protected:
    ZeOneDirectionGrid getOneDirectionLinearGrid(int pxWidth, double &step, double min, double max,
                                               int subGridlineCount, double minTickDist, double maxTickDist);
    void verifyOrthonormality();

    double Xmin, Xmax, Ymin, Ymax;
    double lgXmin, lgXmax, lgYmin, lgYmax;
    ZeScaleType scaleType;
    QWidget *viewWidget;

    double xLogBase, yLogBase;    
    double xGridStep, yGridStep;

    ZeGridSettings gridSettings;
};

#endif // GRAPHVIEW_H
