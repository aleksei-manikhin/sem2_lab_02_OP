#ifndef MAINWINDOW_H
#define MAINWINDOW_H

extern "C" {
#include "entrypoint.h"
#include "iterator.h"
}

#include <QMainWindow>
#include <QString>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void chooseFileClicked();
    void loadDataClicked();
    void calculateMetricsClicked();
    void regionEditingFinished();
    void tableItemDoubleClicked(QTableWidgetItem *item);

    Ui::MainWindow *ui;
    AppContext context;

    void setupConnections();
    void setupTable();
    void setupColumnComboBox();
    void setupRegionComboBox();
    void reloadRegionComboBox();
    void setLoadedState();
    int hasLoadedData() const;
    Column selectedColumn() const;
    void fillTable(const QString& regionFilter);
    void clearMetricFields();
    void showLoadSummary();
    QString statusText(Status status) const;
};

#endif // MAINWINDOW_H
