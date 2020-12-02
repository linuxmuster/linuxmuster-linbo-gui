#include "../../headers/frontend/linbologindialog.h"

LinboLoginDialog::LinboLoginDialog(QWidget* parent) : QDialog(parent)
{
    //this->setWindowFlags(Qt::FramelessWindowHint);
    this->setModal(true);
    this->setWindowModality(Qt::ApplicationModal);
}
