#include "drawingwidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QPixmap>

DrawingWidget::DrawingWidget(QWidget *parent) : QWidget(parent)
{
    lastWidgetSize = widgetSize = this->size();

    basePenWidth = 30;  // 当绘画窗口大小为400*400时的笔画线宽度

    lastMapSize = mapSize = widgetSize;
    map = QPixmap(mapSize);
    map.fill(Qt::white);

    endPoint = lastPoint = QPoint(-100,-100);

    clearFlag = true;
    reShapeFlag = true;

    update();
}

void DrawingWidget::clearWindow()  // 清空绘画板
{
    map = QPixmap(mapSize);    //新建个画布
    map.fill(Qt::white);       //白色填充

    clearFlag = true;
    update();
}

void DrawingWidget::paintEvent(QPaintEvent *event)
{
    QPainter ShowPainter(this);
    if(clearFlag == false)
    {
        QPainter MapPainter(&map);
        QPen pen;
        pen.setWidth(30);
        pen.setCapStyle(Qt::RoundCap);  //用圆形笔头
        MapPainter.setPen(pen);         //把笔给画家

        MapPainter.drawLine(lastPoint,endPoint);//画在内存中的纸上

        lastPoint = endPoint;       //画完后，把这次的坐标记录下来

        ShowPainter.drawPixmap(0,0,map);       //从内存显示出来

        //qDebug() << "正常绘图";
    }

    if(clearFlag == true)
    {
        ShowPainter.drawPixmap(0,0,map);
        clearFlag = false;

        //qDebug() << "清空绘图区";
    }

    if(reShapeFlag == true)    //启动的时候修改绘图窗口大小
    {
        reShapeFlag = false;

        map = QPixmap(mapSize);
        map.fill(Qt::white);
        ShowPainter.drawPixmap(0,0,map);

        //qDebug() << "窗口大小已改变" << "[" << widgetSize << "::" << mapSize << "]";
    }
    //qDebug() << true << false << clearFlag;
}

void DrawingWidget::mouseMoveEvent(QMouseEvent *event)   //鼠标移动事件 获取鼠标移动后的坐标
{
    if(event->buttons()&Qt::LeftButton)
    {
        endPoint = event->pos();
        update();
    }
}

void DrawingWidget::mousePressEvent(QMouseEvent *event) //鼠标按下
{
    if(event->button() == Qt::LeftButton)
    {
        lastPoint = event->pos();
        endPoint = event->pos();
        update();
    }
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent *event)
{

}


void DrawingWidget::resizeEvent(QResizeEvent *event)  //暂时没用，禁止了改变窗口大小
{
    lastWidgetSize = widgetSize;
    widgetSize = this->size();
    lastMapSize = mapSize;
    mapSize = widgetSize;
    reShapeFlag = true;
    update();
}
