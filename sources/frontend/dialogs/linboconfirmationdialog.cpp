#include "linboconfirmationdialog.h"

LinboConfirmationDialog::LinboConfirmationDialog(QString title, QString question, QWidget* parent) : QModernDialog(parent)
{
    this->setTitle(title);
    this->questionLabel = new QLabel(question, this);
    this->questionLabel->setAlignment(Qt::AlignCenter);

    this->yesButton = new QModernPushButton("", "yes", this);
    connect(this->yesButton, SIGNAL(clicked()), this, SIGNAL(accepted()));
    connect(this->yesButton, SIGNAL(clicked()), this, SLOT(autoClose()));

    this->noButton = new QModernPushButton("", "no", this);
    connect(this->noButton, SIGNAL(clicked()), this, SLOT(autoClose()));
    connect(this->noButton, SIGNAL(clicked()), this, SIGNAL(rejected()));

    connect(this, SIGNAL(closedByUser()), this, SIGNAL(rejected()));
}


void LinboConfirmationDialog::resizeEvent(QResizeEvent *event) {
    QModernDialog::resizeEvent(event);

    int buttonHeight = this->parentWidget()->height() * 0.06;
    int margins = buttonHeight * 0.4;
    int questionLabelHeight = this->height() - buttonHeight - margins;

    this->questionLabel->setGeometry(0, 0, this->width(), questionLabelHeight);
    QFont font = this->questionLabel->font();
    font.setPixelSize(buttonHeight * 0.4);
    this->questionLabel->setFont(font);

    this->yesButton->setGeometry(margins, this->height() - buttonHeight - margins, this->width() * 0.5 - 2 * margins, buttonHeight);
    this->yesButton->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
    this->noButton->setGeometry(this->width() * 0.5 + margins, this->height() - buttonHeight - margins, this->width() * 0.5 - 2 * margins, buttonHeight);
    this->noButton->setStyleSheet("QLabel { color: #394f5e; font-weight: bold;}");
}
