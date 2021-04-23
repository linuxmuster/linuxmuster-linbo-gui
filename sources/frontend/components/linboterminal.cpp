#include "linboterminal.h"

LinboTerminal::LinboTerminal(QWidget* parent) : QTextEdit(parent)
{
    this->currentHistoryIndex = -1;
    this->fixedPosition = 0;
    this->commandBeforeHistorySwitch.clear();
    this->doNotExitOnProcessExit = false;

    this->setStyleSheet(
        "QTextEdit {"
        "   border: 0 0 0 0;"
        "   background: " + gTheme->getColor(LinboGuiTheme::ElevatedBackgroundColor).name() + ";"
        "   padding-left: 5px;"
        "   color: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";"
        "}");

    this->verticalScrollBar()->setStyleSheet(
        "QScrollBar:vertical {"
        "    background: " + gTheme->getColor(LinboGuiTheme::ElevatedBackgroundColor).name() + ";"
        "    width:10px;    "
        "    margin: 0px 0px 0px 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: " + gTheme->getColor(LinboGuiTheme::TextColor).name() + ";"
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

    connect(this, &QTextEdit::cursorPositionChanged, this, &LinboTerminal::handleCursorPositionChanged);

    this->process = new QProcess(this);
    this->process->setProcessChannelMode(QProcess::MergedChannels);

    connect(this->process, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readOutput()) );
    connect(this->process, SIGNAL(readyReadStandardError()),
            this, SLOT(readOutput()) );
    connect(this->process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(handleProcessFinished(int, QProcess::ExitStatus)));

    this->process->setEnvironment({"PS1=$(whoami)@$(hostname):$(pwd)$ "});
    this->process->start("sh", QStringList("-i"));
    this->moveCursor(QTextCursor::End);
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
        this->process->write(QByteArray());
        accept = false;
    }
    else if( key == Qt::Key_C) {
        accept = false;
        if(event->modifiers().testFlag(Qt::ControlModifier) && event->modifiers().testFlag(Qt::ShiftModifier))
            // copy on ctrl+shift+c
            this->copy();
        else if(event->modifiers().testFlag(Qt::ControlModifier)) {
            // restart process on ctrl+c
            this->setCurrentCommand("^C");
            this->restartProcess();
        }
        else
            accept = true;
    }
    else if(key == Qt::Key_V) {
        accept = false;
        if(event->modifiers().testFlag(Qt::ShiftModifier))
            // paste on ctrl+shift+v
            this->paste();
        else
            accept = true;
    }
    else if(key == Qt::Key_Control || key == Qt::Key_Shift) {
        accept = true;
    }
    else {
        if(textCursor().position() < fixedPosition)
            this->moveCursor(QTextCursor::End);
        accept = true;
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
    while(this->textCursor().position() > this->fixedPosition)
        this->moveCursor(QTextCursor::Left, QTextCursor::MoveAnchor);

    this->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    this->textCursor().removeSelectedText();

    this->insertPlainText(command);
}

void LinboTerminal::readOutput() {
    QString output = this->process->readAll();
    this->append(output);
    this->moveCursor(QTextCursor::End);
    fixedPosition = textCursor().position();
}

void LinboTerminal::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)

    this->process->start("sh", QStringList("-i"));

    if(!this->doNotExitOnProcessExit) {
        this->clear();
        emit this->processExited();
    }

    this->doNotExitOnProcessExit = false;
}

void LinboTerminal::execute(QString command) {

    // handle history
    this->commandHistory.insert(0, command);
    this->currentHistoryIndex = -1;

    // execute command
    if(command == "clear") {
        this->clear();
        this->fixedPosition = 0;
        command = "";
    }

    this->process->write((command + "\n").toUtf8());
}

void LinboTerminal::restartProcess() {
    this->doNotExitOnProcessExit = true;
    this->process->kill();
}

void LinboTerminal::handleCursorPositionChanged()
{
    //qDebug() << "Cursor position changed position: " << textCursor().position() << " fixed position " << fixedPosition;
    if (textCursor().position() < fixedPosition) {
        this->setCursorWidth(0);
    }
    else {
        this->setCursorWidth(8);
    }
}

void LinboTerminal::clearAndRestart() {
    this->clear();
    this->fixedPosition = 0;
    this->commandHistory.clear();
    this->restartProcess();
}
