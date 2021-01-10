#include "qmodernsvgwidget.h"

QModernSvgWidget::QModernSvgWidget(int height, QString path, QWidget* parent) : QLabel(parent)
{
    QSvgRenderer* renderer = new QSvgRenderer(QString(path));
    renderer->setAspectRatioMode(Qt::KeepAspectRatio);

    int linboLogoWidth = height * (renderer->defaultSize().width() / renderer->defaultSize().height());

    QImage* image = new QImage(linboLogoWidth, height, QImage::Format_ARGB32);
    image->fill("#00000000");

    QPainter painter(image);
    renderer->render(&painter);

    this->setPixmap(QPixmap::fromImage(*image));
    this->setFixedHeight(height);
    this->setFixedWidth(linboLogoWidth);
}
