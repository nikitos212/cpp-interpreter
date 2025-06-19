#pragma once
#include <QMainWindow>
#include <QLineEdit>

class CodeEditor;
class ConsoleWidget;
class ItmoWrapper;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
protected:
    void showEvent(QShowEvent* event) override;
private slots:
    void onRun();
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void onReplExecute();
private:
    void setupMenu();
    bool maybeSave();
    bool saveToPath(const QString& path);

    CodeEditor* editor;
    ConsoleWidget* console;
    ItmoWrapper* engine;
    bool _firstShow = true;
    QString currentFile;
    QLineEdit* replInput; 
};