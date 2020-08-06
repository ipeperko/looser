#ifndef OVERVIEWWIDGET_H
#define OVERVIEWWIDGET_H

#include "BuildingWidget.h"
#include "RoomTree.h"
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

class OverviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverviewWidget(QWidget* parent = 0, RoomTree* tree = 0, BuildingWidget* building = 0);

private:
    RoomTree* tree;
    BuildingWidget* building;

    QPushButton* buttCalc;
    QTextEdit* area;

signals:

public slots:

    void calculate();
};

#endif // OVERVIEWWIDGET_H
