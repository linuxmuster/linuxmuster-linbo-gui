#include "linboconfirmationdialog.h"

LinboConfirmationDialog::LinboConfirmationDialog(QString title, QString question, QWidget* parent) : LinboDialog(parent)
{
    this->setTitle(title);
    this->questionLabel = new QLabel(question, this);
    this->questionLabel->setAlignment(Qt::AlignCenter);

    //= yes
    LinboToolButton* toolButtonCache = new LinboToolButton(tr("yes"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SIGNAL(accepted()));
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));

    //= no
    toolButtonCache = new LinboToolButton(tr("no"));
    this->addToolButton(toolButtonCache);
    connect(toolButtonCache, SIGNAL(clicked()), this, SLOT(autoClose()));
    connect(toolButtonCache, SIGNAL(clicked()), this, SIGNAL(rejected()));

    connect(this, SIGNAL(closedByUser()), this, SIGNAL(rejected()));
}


void LinboConfirmationDialog::resizeEvent(QResizeEvent *event) {
    LinboDialog::resizeEvent(event);

    int rowHeight = gTheme->getSize(LinboGuiTheme::RowHeight);
    int margins = gTheme->getSize(LinboGuiTheme::Margins);

    this->questionLabel->setGeometry(0, (this->height() - rowHeight) / 2, this->width() - margins * 2, rowHeight);
    QFont font = this->questionLabel->font();
    font.setPixelSize(gTheme->getSize(LinboGuiTheme::RowFontSize));
    this->questionLabel->setFont(font);
}
