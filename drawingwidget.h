#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPixmap>

class DrawingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingWidget(QWidget *parent = nullptr);

signals:


protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

public slots:
    void clearWindow();

private:
    int basePenWidth;
    bool clearFlag;
    bool reShapeFlag;
    QPoint lastPoint;
    QPoint endPoint;
    QSize widgetSize;
    QSize mapSize;
    QSize lastWidgetSize;
    QSize lastMapSize;
public:
    QPixmap map;

};

#endif // DRAWINGWIDGET_H
