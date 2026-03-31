#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QFileDialog>
#include <QGuiApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>
#include <QTableWidgetItem>

#include <string>

#define COLUMN_COUNT 7
#define STATUS_BAR_MESSAGE_TIMEOUT_MS 7000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    doOperation(INITIALIZE, &context, nullptr);
    setupTable();
    setupConnections();
    setLoadedState(false);
    clearMetricFields();
    statusBar()->showMessage(statusText(context.status), STATUS_BAR_MESSAGE_TIMEOUT_MS);
}

MainWindow::~MainWindow()
{
    doOperation(DISPOSE, &context, nullptr);
    delete ui;
}

void MainWindow::setupConnections() {
    connect(ui->chooseFileButton, &QPushButton::clicked, this, &MainWindow::chooseFileClicked);
    connect(ui->loadDataButton, &QPushButton::clicked, this, &MainWindow::loadDataClicked);
    connect(ui->calculateMetricsButton, &QPushButton::clicked, this, &MainWindow::calculateMetricsClicked);
    connect(ui->regionLineEdit, &QLineEdit::editingFinished, this, &MainWindow::regionEditingFinished);
    connect(ui->tableWidget, &QTableWidget::itemDoubleClicked, this, &MainWindow::tableItemDoubleClicked);
}

void MainWindow::setupTable() {
    ui->tableWidget->setColumnCount(COLUMN_COUNT);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(
        {"Year", "Region", "Growth", "Birth", "Death", "Weight", "Urban"});
}

void MainWindow::setLoadedState(bool isLoaded) {
    dataLoaded = isLoaded;
    ui->calculateMetricsButton->setEnabled(isLoaded);
    ui->contentStackedWidget->setCurrentWidget(isLoaded ? ui->tablePage : ui->emptyPage);
    if (!isLoaded)
        ui->tableWidget->setRowCount(0);
}

void MainWindow::clearMetricFields() {
    ui->minValueLineEdit->clear();
    ui->medianValueLineEdit->clear();
    ui->maxValueLineEdit->clear();
}

QString MainWindow::statusText(Status status) const {
    QString text;

    switch (status) {
    case STATUS_OK:
        text = "OK";
        break;
    case ERR_FILE_OPEN:
        text = "Cannot open file";
        break;
    case ERR_INVALID_HEADER:
        text = "Invalid CSV header";
        break;
    case MEMORY_ERR:
        text = "Memory allocation error";
        break;
    case ERR_EMPTY_DATA:
        text = "No valid data found";
        break;
    case ERR_INVALID_REGION:
        text = "Region is not found";
        break;
    case ERR_INVALID_COLUMN:
        text = "Invalid column for metrics";
        break;
    default:
        text = "Unknown error";
        break;
    }

    return text;
}

void MainWindow::showLoadSummary() {
    QString summary = QString("Total rows: %1\nValid rows: %2\nInvalid rows: %3")
                          .arg(context.parseInfo.totalRows)
                          .arg(context.parseInfo.validRows)
                          .arg(context.parseInfo.invalidRows);
    QMessageBox::information(this, "Load Result", summary);
}


void MainWindow::fillTable(const QString& regionFilter) {
    bool isRegionEmpty = regionFilter.trimmed().isEmpty();
    bool isRegionFound = false;
    int row = 0;
    Iterator it;

    ui->tableWidget->setRowCount(0);
    if (context.list == nullptr)
        return;

    it = begin(context.list);
    while (isSet(&it)) {
        DemographyRecord* record = (DemographyRecord*)get(&it);
        if (record != nullptr) {
            QString recordRegion = QString::fromLocal8Bit(record->region);
            if (isRegionEmpty || recordRegion.compare(regionFilter, Qt::CaseInsensitive) == 0) {
                if (!isRegionEmpty)
                    isRegionFound = true;
                ui->tableWidget->insertRow(row);
                ui->tableWidget->setItem(row, COL_YEAR - 1, new QTableWidgetItem(QString::number(record->year)));
                ui->tableWidget->setItem(row, COL_REGION - 1, new QTableWidgetItem(recordRegion));
                ui->tableWidget->setItem(row, COL_NPG - 1, new QTableWidgetItem(QString::number(record->naturalPopulationGrowth)));
                ui->tableWidget->setItem(row, COL_BIRTH_RATE - 1, new QTableWidgetItem(QString::number(record->birthRate)));
                ui->tableWidget->setItem(row, COL_DEATH_RATE - 1, new QTableWidgetItem(QString::number(record->deathRate)));
                ui->tableWidget->setItem(row, COL_GDW - 1, new QTableWidgetItem(QString::number(record->generalDemographicWeight)));
                ui->tableWidget->setItem(row, COL_URBANIZATION - 1, new QTableWidgetItem(QString::number(record->urbanization)));
                row++;
            }
        }

        next(&it);
    }

    if (!isRegionEmpty && !isRegionFound)
        statusBar()->showMessage("Region is not found in loaded data", STATUS_BAR_MESSAGE_TIMEOUT_MS);
}

void MainWindow::chooseFileClicked() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "Select CSV file", "", "CSV Files (*.csv);;All Files (*)");

    if (!fileName.isEmpty())
        ui->filePathLineEdit->setText(fileName);
}

void MainWindow::loadDataClicked() {
    AppParams params;
    std::string filePath = ui->filePathLineEdit->text().toStdString();

    params.str = filePath.c_str();
    params.column = COL_YEAR;

    doOperation(LOAD_DATA, &context, &params);
    statusBar()->showMessage(statusText(context.status), STATUS_BAR_MESSAGE_TIMEOUT_MS);
    showLoadSummary();

    if (context.status == STATUS_OK) {
        setLoadedState(true);
        fillTable(ui->regionLineEdit->text().trimmed());
    } else {
        setLoadedState(false);
        clearMetricFields();
    }
}

void MainWindow::calculateMetricsClicked() {
    AppParams params;
    std::string region = ui->regionLineEdit->text().trimmed().toStdString();

    params.str = region.c_str();
    params.column = (Column)ui->columnSpinBox->value();

    doOperation(CALCULATE_METRICS, &context, &params);
    statusBar()->showMessage(statusText(context.status), STATUS_BAR_MESSAGE_TIMEOUT_MS);

    if (context.status == STATUS_OK) {
        ui->minValueLineEdit->setText(QString::number(context.metrics.min));
        ui->medianValueLineEdit->setText(QString::number(context.metrics.median));
        ui->maxValueLineEdit->setText(QString::number(context.metrics.max));
    } else
        clearMetricFields();
}

void MainWindow::regionEditingFinished() {
    if (dataLoaded)
        fillTable(ui->regionLineEdit->text().trimmed());
}

void MainWindow::tableItemDoubleClicked(QTableWidgetItem *item) {
    if (item && item->column() == COL_REGION - 1)
        QGuiApplication::clipboard()->setText(item->text());
}
