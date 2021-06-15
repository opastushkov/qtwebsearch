#include "websearchview.h"
#include "ui_websearchview.h"

#include <QTimer>
#include <QStandardItemModel>

#include <websearchmodel.h>
#include <synchronizedhash.h>

#define TABLE_UPDATE_INTERVAL (200)
#define COLUMNS_NUMBER (3)
#define URL_INDEX (0)
#define STATUS_INDEX (1)
#define INFO_INDEX (2)

WebSearchView::WebSearchView(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::WebSearchView),
      timer(new QTimer)
{
    ui->setupUi(this);
    ui->stopButton->setEnabled(false);
    ui->URLEdit->setValidator(new QRegExpValidator(QRegExp(URL_REGEX), this));
}

WebSearchView::~WebSearchView()
{
    delete ui;
    delete timer;
}

void WebSearchView::resetButtons()
{
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
}

void WebSearchView::clearView()
{
    ui->tasksStatuses->clear();
    ui->tasksStatuses->setRowCount(0);
    ui->tasksStatuses->setColumnCount(COLUMNS_NUMBER);
    ui->tasksStatuses->setHorizontalHeaderLabels(QStringList{
                                                     "URL",
                                                     "Status",
                                                     "Additional info"
                                                 });
    ui->tasksStatuses->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tasksStatuses->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void WebSearchView::on_startButton_clicked()
{
    clearView();
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);

    workerThread = new QThread;
    webSearchModel = new WebSearchModel(this,
                                    ui->URLEdit->text(),
                                    ui->SearchEdit->text(),
                                    ui->URLSpinBox->value(),
                                    ui->ThreadsSpinBox->value()
    );

    webSearchModel->moveToThread(workerThread);

    connect(webSearchModel, SIGNAL(finished()), webSearchModel, SLOT(deleteLater()));
    connect(workerThread, SIGNAL(started()), webSearchModel, SLOT(startSearch()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    connect(webSearchModel, SIGNAL(finished()), workerThread, SLOT(quit()));
    connect(workerThread, SIGNAL(finished()), this, SLOT(resetButtons()));

    connect(webSearchModel, SIGNAL(updateView()), this, SLOT(lastUpdate()));
    connect(webSearchModel, SIGNAL(updateView()), timer, SLOT(stop()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTable()));

    workerThread->start();

    timer->start(TABLE_UPDATE_INTERVAL);
}

QString WebSearchView::internalStatusToString(const InternalStatus &status) const
{
    switch (status)
    {
        case InternalStatus::IN_PROGRESS:
            return "In progress";
        case InternalStatus::DOWNLOADING:
            return "Downloading";
        case InternalStatus::TEXT_FOUND:
            return "Matched";
        case InternalStatus::TEXT_NOT_FOUND:
            return "Unmatched";
        case InternalStatus::NETWORK_ERROR:
            return "Network error";
        default:
            return "Internal error";
    }
}

Qt::GlobalColor WebSearchView::chooseColor(const InternalStatus &status) const
{
    switch (status)
    {
        case InternalStatus::TEXT_FOUND:
            return Qt::green;
        case InternalStatus::TEXT_NOT_FOUND:
            return Qt::yellow;
        case InternalStatus::NETWORK_ERROR :
            return Qt::red;
        case InternalStatus::INTERNAL_ERROR:
            return Qt::red;
        default:
            return Qt::gray;
    }
}

void WebSearchView::updateTask(const int row, const QString &url, const TaskStatus &task)
{
    Qt::GlobalColor color = chooseColor(task.internalStatus);
    auto site = new QTableWidgetItem(url);
    auto status = new QTableWidgetItem(internalStatusToString(task.internalStatus));
    auto info = new QTableWidgetItem(task.responseStatus);

    site->setBackgroundColor(color);
    status->setBackgroundColor(color);
    info->setBackgroundColor(color);

    ui->tasksStatuses->setItem(row, URL_INDEX, site);
    ui->tasksStatuses->setItem(row, STATUS_INDEX, status);
    ui->tasksStatuses->setItem(row, INFO_INDEX, info);
}

void WebSearchView::updateTable()
{
    // Check if we need additional rows to display all tasks in QTableWidget
    auto tasks = webSearchModel->getStatuses();
    for (int i = ui->tasksStatuses->rowCount(); i < tasks.size(); ++i)
        ui->tasksStatuses->insertRow(ui->tasksStatuses->rowCount());

    int rowNum = 0;
    for (auto it = tasks.begin(); it != tasks.end(); ++it, ++rowNum)
        updateTask(rowNum, it.key(), it.value());

    timer->start(TABLE_UPDATE_INTERVAL);
}

void WebSearchView::lastUpdate()
{
    updateTable();
    timer->stop();
    emit updateFinished();
}

void WebSearchView::on_stopButton_clicked()
{
    emit stopExecution();
    resetButtons();
}
