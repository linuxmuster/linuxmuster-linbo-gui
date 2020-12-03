#include "linboterminaldialog.h"

LinboTerminalDialog::LinboTerminalDialog(QWidget* parent) : QModernDialog(parent)
{
    this->currentHistoryIndex = -1;
    this->commandBeforeHistorySwitch.clear();

    this->textBrowser = new QTextBrowser();
    this->textBrowser->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    this->textBrowser->setStyleSheet("QTextBrowser {background-color: \"#ffffff\"}");

    this->lineEdit = new QModernLineEdit();
    this->lineEdit->installEventFilter(this);
    connect(this->lineEdit, SIGNAL(returnPressed()), this, SLOT(execute()));

    this->mainLayout = new QVBoxLayout(this);
    this->mainLayout->addWidget(this->textBrowser);
    this->mainLayout->addWidget(this->lineEdit);

    this->process = new QProcess(this);
    this->process->setProcessChannelMode(QProcess::MergedChannels);

    connect(this->process, SIGNAL(readyReadStandardOutput()),
             this, SLOT(readOutput()) );
    connect(this->process, SIGNAL(readyReadStandardError()),
             this, SLOT(readOutput()) );
    connect(this->process, SIGNAL(finished(int, QProcess::ExitStatus)),
             this, SLOT(handleProcessFinished(int, QProcess::ExitStatus)));
}

void LinboTerminalDialog::setVisibleAnimated(bool visible) {
    //this->textBrowser->setVisible(visible);
    QModernDialog::setVisibleAnimated(visible);
    if(visible)
        this->process->start("sh", QStringList("-i"));
    else
        this->process->kill();
}


void LinboTerminalDialog::readOutput() {
    QString output = this->process->readAll();
    this->textBrowser->append(output);
}

void LinboTerminalDialog::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    this->textBrowser->clear();
    this->close();
}

void LinboTerminalDialog::execute() {
    QString command = this->lineEdit->text();

    // handle history
    this->commandHistory.insert(0, command);
    this->currentHistoryIndex = -1;

    // handle ui
    this->textBrowser->insertPlainText(command);
    this->process->write((command + "\n").toUtf8());
    this->lineEdit->clear();

    // scroll to bottom
    QScrollBar* sb = this->textBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
}

bool LinboTerminalDialog::eventFilter(QObject* obj, QEvent* event) {
    if(obj == this->lineEdit) {
        if(event != nullptr && event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            if(keyEvent->key() != Qt::Key_Up && keyEvent->key() != Qt::Key_Down)
                return QModernDialog::eventFilter(obj, event);

            int oldHistoryIndex = this->currentHistoryIndex;

            if(keyEvent->key() == Qt::Key_Up) {
                this->currentHistoryIndex += 1;
            }
            else if(keyEvent->key() == Qt::Key_Down) {
                this->currentHistoryIndex -= 1;
            }

            if(this->currentHistoryIndex >= 0 && this->commandHistory.length() > this->currentHistoryIndex) {
                if(oldHistoryIndex == -1)
                    // store current command before bringing up the history
                    this->commandBeforeHistorySwitch = this->lineEdit->text();

                // show requested history command
                this->lineEdit->setText(this->commandHistory[this->currentHistoryIndex]);
            }
            else if(this->currentHistoryIndex == -1) {
                // show last typed command (is not in history yet, as it was not yet executed)
                this->lineEdit->setText(this->commandBeforeHistorySwitch);
                this->commandBeforeHistorySwitch.clear();
            }
            else if(this->commandHistory.length() <= this->currentHistoryIndex) {
                // don't go above history length
                this->currentHistoryIndex = this->commandHistory.length() -1;
            }
            else {
                // don't do below -1
                this->currentHistoryIndex = -1;
            }
        }
    }
    return QModernDialog::eventFilter(obj, event);
}
