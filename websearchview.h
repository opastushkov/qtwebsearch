#ifndef WEBSEARCHVIEW_H
#define WEBSEARCHVIEW_H

#include <QMainWindow>

enum class InternalStatus;
class WebSearchModel;
class TaskStatus;

QT_BEGIN_NAMESPACE
namespace Ui { class WebSearchView; }
QT_END_NAMESPACE

class WebSearchView : public QMainWindow
{
    Q_OBJECT

public:
    WebSearchView(QWidget *parent = nullptr);
    ~WebSearchView();

private:
    WebSearchModel *webSearchModel;
    Ui::WebSearchView *ui;
    QThread *workerThread;
    QTimer *timer;

    QString internalStatusToString(const InternalStatus &status) const;
    Qt::GlobalColor chooseColor(const InternalStatus &status) const;
    void updateTask(const int row, const QString &url, const TaskStatus &task);
    void clearView();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void updateTable();
    void resetButtons();
    void lastUpdate();

signals:
    void updateFinished();
    void stopExecution();
};
#endif // WEBSEARCHVIEW_H
