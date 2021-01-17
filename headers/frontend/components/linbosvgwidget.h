#ifndef QMODERNSVGWIDGET_H
#define QMODERNSVGWIDGET_H

#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QSvgRenderer>
#include <QPainter>

class LinboSvgWidget : public QLabel
{
    Q_OBJECT
public:
    LinboSvgWidget(int height, QString path, QWidget* parent = nullptr);
};

#endif // QMODERNSVGWIDGET_H
