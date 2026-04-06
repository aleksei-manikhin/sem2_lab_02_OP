#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QComboBox>
#include <QEvent>
#include <QFileDialog>
#include <QGuiApplication>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QStatusBar>
#include <QStandardPaths>
#include <QTableWidgetItem>

#include <string>

#define COLUMN_COUNT 7
#define STATUS_BAR_MESSAGE_TIMEOUT_MS 4000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    doOperation(INITIALIZE, &context, nullptr);
    setupTable();
    setupColumnComboBox();
    setupRegionComboBox();
    setupConnections();
    setLoadedState();
    clearMetricFields();
    statusBar()->showMessage(statusText(context.status), STATUS_BAR_MESSAGE_TIMEOUT_MS);
}

MainWindow::~MainWindow()
{
    doOperation(DISPOSE, &context, nullptr);
    delete ui;
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (event != nullptr && event->type() == QEvent::MouseButtonPress) {
        if (ui->regionComboBox != nullptr && watched == ui->regionComboBox->lineEdit())
            ui->regionComboBox->showPopup();
        else if (ui->columnComboBox != nullptr && watched == ui->columnComboBox->lineEdit())
            ui->columnComboBox->showPopup();
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::setupConnections() {
    connect(ui->chooseFileButton, &QPushButton::clicked, this, &MainWindow::chooseFileClicked);
    connect(ui->loadDataButton, &QPushButton::clicked, this, &MainWindow::loadDataClicked);
    connect(ui->calculateMetricsButton, &QPushButton::clicked, this, &MainWindow::calculateMetricsClicked);
    connect(ui->regionComboBox, QOverload<int>::of(&QComboBox::activated), this, [this](int) {
        regionEditingFinished();
    });
    if (ui->regionComboBox->lineEdit() != nullptr) {
        connect(ui->regionComboBox->lineEdit(), &QLineEdit::editingFinished,
                this, &MainWindow::regionEditingFinished);
    }
    connect(ui->tableWidget, &QTableWidget::itemDoubleClicked, this, &MainWindow::tableItemDoubleClicked);
}

void MainWindow::setupTable() {
    ui->tableWidget->setColumnCount(COLUMN_COUNT);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(
        {"Year", "Region", "Growth", "Birth", "Death", "Weight", "Urban"});
}

void MainWindow::setupColumnComboBox() {
    ui->columnComboBox->clear();
    ui->columnComboBox->addItem("Year", COL_YEAR);
    ui->columnComboBox->addItem("Growth", COL_NPG);
    ui->columnComboBox->addItem("Birth", COL_BIRTH_RATE);
    ui->columnComboBox->addItem("Death", COL_DEATH_RATE);
    ui->columnComboBox->addItem("Weight", COL_GDW);
    ui->columnComboBox->addItem("Urban", COL_URBANIZATION);
}

void MainWindow::setupRegionComboBox() {
    ui->regionComboBox->clear();
    ui->regionComboBox->addItem("");
    ui->regionComboBox->setCurrentIndex(0);
    ui->regionComboBox->lineEdit()->installEventFilter(this);
}

void MainWindow::reloadRegionComboBox() {
    QString currentText = ui->regionComboBox->currentText().trimmed();
    Iterator it;

    if (context.list != nullptr) {
        ui->regionComboBox->clear();
        ui->regionComboBox->addItem("");

        it = begin(context.list);
        while (isSet(&it)) {
            DemographyRecord* record = (DemographyRecord*)get(&it);
            if (record != nullptr) {
                QString region = QString::fromLocal8Bit(record->region).trimmed();
                int foundIndex = -1;

                for (int i = 0; i < ui->regionComboBox->count(); i++) {
                    if (ui->regionComboBox->itemText(i).compare(region, Qt::CaseInsensitive) == 0) {
                        foundIndex = i;
                        break;
                    }
                }

                if (!region.isEmpty() && foundIndex < 0)
                    ui->regionComboBox->addItem(region);
            }
            next(&it);
        }
    } else {
        ui->regionComboBox->clear();
        ui->regionComboBox->addItem("");
    }

    if (currentText.isEmpty())
        ui->regionComboBox->setCurrentIndex(0);
    else
        ui->regionComboBox->setEditText(currentText);
}

Column MainWindow::selectedColumn() const {
    QString text = ui->columnComboBox->currentText().trimmed();

    for (int i = 0; i < ui->columnComboBox->count(); i++) {
        if (ui->columnComboBox->itemText(i).compare(text, Qt::CaseInsensitive) == 0)
            return (Column)ui->columnComboBox->itemData(i).toInt();
    }

    return (Column)0;
}

int MainWindow::hasLoadedData() const {
    return (context.list != nullptr && context.list->size > 0);
}

void MainWindow::setLoadedState() {
    int isLoaded = hasLoadedData();
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
    case OK:
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
    size_t totalRows = context.parseInfo.accepted + context.parseInfo.rejected;
    QString summary = QString("Total rows: %1\nValid rows: %2\nInvalid rows: %3")
                          .arg(totalRows)
                          .arg(context.parseInfo.accepted)
                          .arg(context.parseInfo.rejected);
    QMessageBox::information(this, "Load Result", summary);
}


void MainWindow::fillTable(const QString& regionFilter) {
    int isRegionEmpty = regionFilter.trimmed().isEmpty() ? 1 : 0;
    int isRegionFound = 0;
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
                    isRegionFound = 1;
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
        this,
        "Select CSV file",
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
        "CSV Files (*.csv);;All Files (*)");

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
    reloadRegionComboBox();
    setLoadedState();

    if (context.status == OK)
        fillTable(ui->regionComboBox->currentText().trimmed());
    else
        clearMetricFields();

}

void MainWindow::calculateMetricsClicked() {
    AppParams params;
    std::string region = ui->regionComboBox->currentText().trimmed().toStdString();

    params.str = region.c_str();
    params.column = selectedColumn();

    doOperation(CALCULATE_METRICS, &context, &params);
    statusBar()->showMessage(statusText(context.status), STATUS_BAR_MESSAGE_TIMEOUT_MS);

    if (context.status == OK) {
        ui->minValueLineEdit->setText(QString::number(context.metrics.min));
        ui->medianValueLineEdit->setText(QString::number(context.metrics.median));
        ui->maxValueLineEdit->setText(QString::number(context.metrics.max));
    } else
        clearMetricFields();
}

void MainWindow::regionEditingFinished() {
    if (hasLoadedData())
        fillTable(ui->regionComboBox->currentText().trimmed());
}

void MainWindow::tableItemDoubleClicked(QTableWidgetItem *item) {
    if (item && item->column() == COL_REGION - 1)
        QGuiApplication::clipboard()->setText(item->text());
}
