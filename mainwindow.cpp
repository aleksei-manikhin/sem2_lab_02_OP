#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(
        {"Year", "Region", "Growth", "Birth", "Death", "Weight", "Urban"});




    //Заглушка таблицы
    ui->tableWidget->setRowCount(5);
    QStringList regions = {
        "Republic of Adygea",
        "Moscow",
        "Saint Petersburg",
        "Kazan",
        "Novosibirsk"
    };

    for (int row = 0; row < 5; row++)
    {
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(2019 + row)));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(regions[row]));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(-3.2 + row)));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(10.1 + row)));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(13.3 + row)));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(1.2 + row)));
        ui->tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(62 + row)));
    }
    //Конец заглушки таблицы

}

MainWindow::~MainWindow()
{
    delete ui;
}
