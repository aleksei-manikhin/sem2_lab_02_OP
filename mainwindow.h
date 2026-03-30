#ifndef MAINWINDOW_H
#define MAINWINDOW_H

extern "C" {
#include "entrypoint.h"
#include "iterator.h"
}

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void chooseFileClicked();
    void loadDataClicked();
    void calculateMetricsClicked();
    void regionEditingFinished();

private:
    Ui::MainWindow *ui;
    AppContext context;
    bool dataLoaded;

    void setupConnections();
    void setupTable();
    void setLoadedState(bool isLoaded);
    void fillTable(const QString& regionFilter);
    void clearMetricFields();
    void showLoadSummary();
    void showStatus(Status status);
    QString statusText(Status status) const;
};

#endif // MAINWINDOW_H
