#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QObject>
#include <QRectF>
#include <QPair>

enum struct ScaleType
{
    LINEAR, LINEAR_ORTHONORMAL, X_LOG, Y_LOG, XY_LOG,
};

struct ZeLogBase
{
    double base;
    double powerNumerator, powerDenominator;
};

enum struct GridType
{
    NO_GRID, GRID, GRID_SUBGRID
};

struct GridSettings
{
    GridType gridType;
    double xGridStep, yGridStep;
    unsigned int horSubGridLineCount, verSubGridLineCount;
    // the number of lines of the sub grid between two lines of the main one.
};

struct MainGridLine
{
    double pos; // no need to differenciate between log and linear view since it's seamless for the plotter
    QString logBase;
    int powerNumerator, powerDenominator;

};

struct SubGridLine
{
    double pos;
    int numerator, denominator;
};

struct ZeGridDescription
{
    OneDirectionGrid horizontal, vertical;
};

struct OneDirectionGrid
{
    QList<MainGridLine> mainGrid;
    QList<SubGridLine> subGrid;
};

class GraphView : public QObject
{
    Q_OBJECT
public:
    explicit GraphView(QObject *parent = 0);
    GraphView(const GraphView &other, QObject *parent = 0);

    GraphView& operator=(const GraphView &other);


    void zoomYview(double ratio);
    void zoomXview(double ratio);
    void zoomView(QPointF center, double ratio);
    void translateView(QPointF vec);

    void setlgXmin(double val);
    void setlgXmax(double val);
    void setlgYmin(double val);
    void setlgYmax(double val);

    void setXmin(double val);
    void setXmax(double val);
    void setYmin(double val);
    void setYmax(double val);

    const void realYvalueFromView(double viewY);
    const void realXvalueFromView(double viewX);

    const QRectF rect();
    const QRectF lgRect();

    const QRectF viewRect();

    const ScaleType viewType();

    ZeGridDescription getGrid(QRect graphRectPx);

signals:

public slots:

protected:
    OneDirectionGrid getOneDirectionLinearGrid(int pxWidth)


    double Xmin, Xmax, Ymin, Ymax;
    double lgXmin, lgXmax, lgYmin, lgYmax;
    ScaleType m_viewType;
    double xLogBase, yLogBase;

    GridSettings gridSettings;
};

#endif // GRAPHVIEW_H
