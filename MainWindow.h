#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "BuildingWidget.h"
#include "CoeffWidget.h"
#include "OverviewWidget.h"
#include "RoomTree.h"
#include "RoomWidget.h"
#include <QMainWindow>
#include <QTabWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event);

private:
    RoomTree* roomTree;
    QTabWidget* tabs;
    RoomWidget* roomWidget;
    CoeffWidget* coeffWidget;
    BuildingWidget* buildingWidget;
    OverviewWidget* overviewWidget;

    QString currentFileName;
    void saveProject(const QString& fileName);
    void openProject(const QString& fileName);

public slots:
    void newFile();
    void open();
    void save();
    void saveAs();
};

#endif // MAINWINDOW_H
