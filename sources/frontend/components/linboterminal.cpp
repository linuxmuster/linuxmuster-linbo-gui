#include "linboterminal.h"

LinboTerminal::LinboTerminal(QWidget* parent) : QTextEdit(parent)
{
    this->currentHistoryIndex = -1;
    this->commandBeforeHistorySwitch.clear();

    this->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    this->setStyleSheet("QTextEdit {"
                        "border: 0 0 0 0;"
                        "background: black;"
                        "padding-left: 5px;"
                        "color: white;"
                        "}"
                        "QScrollBar:vertical {"
                        "    background: black;"
                        "    width:10px;    "
                        "    margin: 0px 0px 0px 0px;"
                        "}"
                        "QScrollBar::handle:vertical {"
                        "    background: lightgrey;"
                        "    min-height: 0px;"
                        "}"
                        "QScrollBar::add-line:vertical {"
                        "    background: grey;"
                        "    height: 0px;"
                        "    subcontrol-position: bottom;"
                        "    subcontrol-origin: margin;"
                        "}"
                        "QScrollBar::sub-line:vertical {"
                        "    background: grey;"
                        "    height: 0 px;"
                        "    subcontrol-position: top;"
                        "    subcontrol-origin: margin;"
                        "}");

    this->setCursorWidth(8);

    this->setFont(QFont("Ubuntu Mono"));

    this->process = new QProcess(this);
    this->process->setProcessChannelMode(QProcess::MergedChannels);

    connect(this->process, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readOutput()) );
    connect(this->process, SIGNAL(readyReadStandardError()),
            this, SLOT(readOutput()) );
    connect(this->process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(handleProcessFinished(int, QProcess::ExitStatus)));

    this->process->start("sh", QStringList("-i"));
}

void LinboTerminal::keyPressEvent(QKeyEvent *event)
{
    bool accept;
    int key = event->key();
    if (key == Qt::Key_Backspace || event->key() == Qt::Key_Left) {
        accept = textCursor().position() > fixedPosition;
    }
    else if (key == Qt::Key_Return) {
        accept = false;
        this->execute(this->getCurrentCommand());
    }
    else if (key == Qt::Key_Up || key == Qt::Key_Down) {
        accept = false;

        int oldHistoryIndex = this->currentHistoryIndex;

        if(key == Qt::Key_Up) {
            this->currentHistoryIndex += 1;
        }
        else if(key == Qt::Key_Down) {
            this->currentHistoryIndex -= 1;
        }

        if(this->currentHistoryIndex >= 0 && this->commandHistory.length() > this->currentHistoryIndex) {
            if(oldHistoryIndex == -1)
                // store current command before bringing up the history
                this->commandBeforeHistorySwitch = this->getCurrentCommand();

            // show requested history command
            this->setCurrentCommand(this->commandHistory[this->currentHistoryIndex]);
        }
        else if(this->currentHistoryIndex == -1) {
            // show last typed command (is not in history yet, as it was not yet executed)
            this->setCurrentCommand(this->commandBeforeHistorySwitch);
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
    else if( key == Qt::Key_Tab) {
        accept = false;
    }
    else {
        accept = textCursor().position() >= fixedPosition;
    }

    if (accept) {
        QTextEdit::keyPressEvent(event);
    }
}

QString LinboTerminal::getCurrentCommand() {
    int count = toPlainText().count() - fixedPosition;
    return toPlainText().right(count);
}

void LinboTerminal::setCurrentCommand(QString command) {
    QString allText = this->toPlainText().left(this->fixedPosition);
    this->setPlainText(allText + command);
    this->moveCursor(QTextCursor::End);
}

void LinboTerminal::readOutput() {
    QString output = this->process->readAll();
    this->append(output);
    fixedPosition = textCursor().position();
}

void LinboTerminal::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    this->clear();
    emit this->processExited();
    this->process->start("sh", QStringList("-i"));
}

void LinboTerminal::execute(QString command) {

    // handle history
    this->commandHistory.insert(0, command);
    this->currentHistoryIndex = -1;

    // execute command
    if(command == "clear") {
        this->clear();
        command = "";
    }

    this->process->write((command + "\n").toUtf8());
}

void LinboTerminal::handleCursorPositionChanged()
{
    //qDebug() << "Cursor position changed position: " << textCursor().position() << " fixed position " << fixedPosition;
    if (textCursor().position() < fixedPosition) {
        //this->moveCursor(QTextCursor::End);
    }
}
