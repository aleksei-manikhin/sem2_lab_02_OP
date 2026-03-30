#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>
#include <QTableWidgetItem>
#include <QByteArray>

enum TableColumn {
    TABLE_YEAR = 0,
    TABLE_REGION = 1,
    TABLE_NPG = 2,
    TABLE_BIRTH = 3,
    TABLE_DEATH = 4,
    TABLE_GDW = 5,
    TABLE_URBAN = 6
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , dataLoaded(false)
{
    ui->setupUi(this);

    doOperation(INITIALIZE, &context, nullptr);
    setupTable();
    setupConnections();
    setLoadedState(false);
    clearMetricFields();
    showStatus(context.status);
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
}

void MainWindow::setupTable() {
    ui->tableWidget->setColumnCount(7);
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
    ui->maxValueLineEdit_->clear();
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

void MainWindow::showStatus(Status status) {
    QString prefix = (status == STATUS_OK) ? "Status: " : "Error: ";
    statusBar()->showMessage(prefix + statusText(status), 7000);
}

void MainWindow::showLoadSummary() {
    QString summary = QString("Total rows: %1\nValid rows: %2\nInvalid rows: %3")
                          .arg(context.parseInfo.totalRows)
                          .arg(context.parseInfo.validRows)
                          .arg(context.parseInfo.invalidRows);

    QMessageBox msg(this);
    msg.setWindowTitle("Load Result");
    msg.setText(summary);
    msg.setIcon(context.status == STATUS_OK ? QMessageBox::Information : QMessageBox::Warning);
    msg.setStyleSheet(
        "QMessageBox { background-color: #111111; }"
        "QLabel { color: #E8E8E8; font-size: 12px; }"
        "QPushButton { min-width: 90px; padding: 6px; }"
    );
    msg.exec();
}

/* Converts loaded records into table rows, with optional region filtering. */
void MainWindow::fillTable(const QString& regionFilter) {
    Iterator it;
    int row = 0;
    bool useFilter = !regionFilter.trimmed().isEmpty();

    ui->tableWidget->setRowCount(0);
    if (context.list == nullptr)
        return;

    it = begin(context.list);
    while (isSet(&it)) {
        DemographyRecord* record = (DemographyRecord*)get(&it);
        if (record != nullptr) {
            QString region = QString::fromUtf8(record->region);
            bool matches = !useFilter || region.compare(regionFilter, Qt::CaseInsensitive) == 0;
            if (!matches) {
                next(&it);
                continue;
            }

            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, TABLE_YEAR, new QTableWidgetItem(QString::number(record->year)));
            ui->tableWidget->setItem(row, TABLE_REGION, new QTableWidgetItem(region));
            ui->tableWidget->setItem(row, TABLE_NPG, new QTableWidgetItem(QString::number(record->naturalPopulationGrowth)));
            ui->tableWidget->setItem(row, TABLE_BIRTH, new QTableWidgetItem(QString::number(record->birthRate)));
            ui->tableWidget->setItem(row, TABLE_DEATH, new QTableWidgetItem(QString::number(record->deathRate)));
            ui->tableWidget->setItem(row, TABLE_GDW, new QTableWidgetItem(QString::number(record->generalDemographicWeight)));
            ui->tableWidget->setItem(row, TABLE_URBAN, new QTableWidgetItem(QString::number(record->urbanization)));
            row++;
        }

        next(&it);
    }
}

void MainWindow::chooseFileClicked() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "Select CSV file", "", "CSV Files (*.csv);;All Files (*)");

    if (!fileName.isEmpty())
        ui->filePathLineEdit->setText(fileName);
}

void MainWindow::loadDataClicked() {
    AppParams params;
    QByteArray filePathData = ui->filePathLineEdit->text().toLocal8Bit();

    params.str = filePathData.constData();
    params.column = COL_YEAR;

    doOperation(LOAD_DATA, &context, &params);
    showStatus(context.status);
    showLoadSummary();

    if (context.status == STATUS_OK) {
        setLoadedState(true);
        fillTable(ui->regionLineEdit->text());
    } else {
        setLoadedState(false);
        clearMetricFields();
    }
}

void MainWindow::calculateMetricsClicked() {
    AppParams params;
    QByteArray regionData = ui->regionLineEdit->text().trimmed().toUtf8();

    params.str = regionData.constData();
    params.column = (Column)ui->columnSpinBox->value();

    doOperation(CALCULATE_METRICS, &context, &params);
    showStatus(context.status);

    if (context.status == STATUS_OK) {
        ui->minValueLineEdit->setText(QString::number(context.metrics.min));
        ui->maxValueLineEdit_->setText(QString::number(context.metrics.median));
        ui->maxValueLineEdit->setText(QString::number(context.metrics.max));
    } else
        clearMetricFields();
}

void MainWindow::regionEditingFinished() {
    if (dataLoaded)
        fillTable(ui->regionLineEdit->text());
}
