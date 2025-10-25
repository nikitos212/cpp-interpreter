#include "MainWindow.h"
#include "CodeEditor.h"
#include "ConsoleWidget.h"
#include "ItmoWrapper.h"
#include "interpreter/interpreter.h"
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include <QShowEvent>
#include <QFont>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , engine(new ItmoWrapper),
    interpreter(out)
{
    setMinimumSize(750, 900);
    resize(900, 1000);

    setupMenu();

    auto* toolbar = addToolBar("Run");
    QAction* runAct = toolbar->addAction(QIcon("res/play.png"), "");
    runAct->setToolTip("Run");
    toolbar->setIconSize(QSize(24,24));
    connect(runAct, &QAction::triggered, this, &MainWindow::onRun);

    editor = new CodeEditor;
    console = new ConsoleWidget;

    replInput = new QLineEdit;
    replInput->setPlaceholderText(tr("REPL: введите команду и нажмите Enter"));
    connect(replInput, &QLineEdit::returnPressed, this, &MainWindow::onReplExecute);

    QWidget* replWidget = new QWidget;
    QVBoxLayout* vlay = new QVBoxLayout(replWidget);
    vlay->setContentsMargins(0,0,0,0);
    vlay->setSpacing(2);
    vlay->addWidget(console);
    vlay->addWidget(replInput);

    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(editor);
    splitter->addWidget(replWidget);
    splitter->setStretchFactor(0, 7);
    splitter->setStretchFactor(1, 3);

    setCentralWidget(splitter);
}

void MainWindow::setupMenu() {
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), QKeySequence::New, this, &MainWindow::newFile);
    fileMenu->addAction(tr("&Open..."), QKeySequence::Open, this, &MainWindow::openFile);
    fileMenu->addAction(tr("&Save"), QKeySequence::Save, this, &MainWindow::saveFile);
    fileMenu->addAction(tr("Save &As..."), QKeySequence::SaveAs, this, &MainWindow::saveFileAs);
}

void MainWindow::newFile() {
    editor->clear();
}

void MainWindow::openFile() {
    if (!maybeSave()) return;
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("ITMOScript Files (*.is);;All Files (*)"));
    if (!path.isEmpty()) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            editor->setPlainText(QString::fromUtf8(file.readAll()));
            currentFile = path;
        }
    }
}

bool MainWindow::saveToPath(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QByteArray data = editor->toPlainText().toUtf8();
    file.write(data);
    currentFile = path;
    return true;
}

void MainWindow::saveFile() {
    if (currentFile.isEmpty()) {
        saveFileAs();
    } else {
        if (!saveToPath(currentFile))
            QMessageBox::warning(this, tr("Save Error"), tr("Cannot write to file %1").arg(currentFile));
    }
}

void MainWindow::saveFileAs() {
    QString path = QFileDialog::getSaveFileName(this, tr("Save File As"), QString(), tr("ITMOScript Files (*.is);;All Files (*)")) + ".is";
    if (!path.isEmpty()) {
        if (!saveToPath(path))
            QMessageBox::warning(this, tr("Save Error"), tr("Cannot write to file %1").arg(path));
    }
}

bool MainWindow::maybeSave() {
    if (editor->document()->isModified()) {
        auto ret = QMessageBox::warning(
            this, tr("Unsaved Changes"), tr("The document has been modified. Save changes?"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
        );
        if (ret == QMessageBox::Yes) {
            saveFile();
            return true;
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);

    if (_firstShow) {
        _firstShow = false;

        QScreen* screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect avail = screen->availableGeometry();
            int x = avail.x() + (avail.width()  - width()) / 2;
            int y = avail.y() + (avail.height() - height()) / 2;
            move(x, y);
        }
    }
}

void MainWindow::onRun() {
    console->clear();
    QString code = editor->toPlainText();
    QString output = engine->run(code);
    console->append(output);
}

bool startsBlock(const std::string& t) {
  return t.rfind("if ",0) == 0 || t.rfind("for ",0) == 0 ||
         t.rfind("while ",0) == 0 || (t.find("= function") != std::string::npos) ||
         (t.find("= [") != std::string::npos);
}

bool endsBlock(const std::string& t) {
  return t.find("end if") != std::string::npos || t.find("end for") != std::string::npos ||
         t.find("end while") != std::string::npos || t.rfind("end function",0) == 0 ||
         t.find("]") != std::string::npos;
}

void MainWindow::onReplExecute() {
    QString qcmd = replInput->text().trimmed();
    if (qcmd.isEmpty()) return;
    replInput->clear();
    console->append("> " + qcmd);

    std::string cmd = qcmd.toUtf8().toStdString();
    out.str(""); 
    out.clear();

    if (cmd.rfind("//", 0) == 0) return;

    auto starts = [&](const std::string& t){ return startsBlock(t); };
    auto ends   = [&](const std::string& t){ return endsBlock(t); };

    if (!isUnclosed) {
        if (starts(cmd)) {
            block = cmd + "\n";
            depth = 1;
            if (ends(cmd)) {
                try {
                    interpreter.interpr(block);
                } catch (const std::exception& e) {
                    out << "Error: " << e.what() << std::endl;
                }
            } else {
                isUnclosed = true;
            }
        } else {
            try {
                interpreter.interpr(cmd);
            } catch (const std::exception& e) {
                out << "Error: " << e.what() << std::endl;
            }
        }
    } else {
        block += cmd + "\n";
        if (starts(cmd)) ++depth;
        if (ends(cmd))   --depth;
        if (depth == 0) {
            isUnclosed = false;
            try {
                interpreter.interpr(block);
            } catch (const std::exception& e) {
                out << "Error: " << e.what() << std::endl;
            }
        }
    }

    if (!isUnclosed) {
        auto outStr = out.str();
        depth = 0;
        if (!outStr.empty())
            console->append(QString::fromUtf8(outStr.c_str()));
    }
}