#ifndef QMODERNSVGWIDGET_H
#define QMODERNSVGWIDGET_H

#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QSvgRenderer>
#include <QPainter>

class QModernSvgWidget : public QLabel
{
    Q_OBJECT
public:
    QModernSvgWidget(int height, QString path, QWidget* parent = nullptr);
};

#endif // QMODERNSVGWIDGET_H
