#ifndef ROOMWIDGET_H
#define ROOMWIDGET_H

#include "BuildingWidget.h"
#include "CoeffWidget.h"
#include "Room.h"
#include "RoomWTable.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

class RoomWidget : public QWidget
{
    friend class RoomWTable;
    Q_OBJECT
public:
    explicit RoomWidget(QWidget* parent = nullptr, CoeffWidget* coeffWidget = nullptr, BuildingWidget* buildingWidget = nullptr);

    Room* currentRoom() { return currRoom; }
    BuildingWidget* building() { return buildingWidget; }

private:
    QVBoxLayout* mainLay;

    QLabel* roomName;

    QLineEdit* leditArea;
    QLineEdit* leditHeight;
    QLineEdit* leditVolume;
    QLineEdit* leditTemperature;
    QLineEdit* leditOutdoorTemp;

    QLineEdit* leditCoeff_n50;
    QLineEdit* leditCoeff_ei;
    QLineEdit* leditCoeff_theta;
    QLineEdit* leditInfiltration;
    QLineEdit* leditMinAirExchange;
    QLineEdit* leditReqAirFlow;
    QLineEdit* leditMaxAirFlow;

    QLabel* labelResultVent;
    QLabel* labelResultTrans;
    QLabel* labelResultSum;
    QLabel* labelResultVent_m2;
    QLabel* labelResultTrans_m2;
    QLabel* labelResultSum_m2;
    QLabel* labelResultVent_m3;
    QLabel* labelResultTrans_m3;
    QLabel* labelResultSum_m3;

private:
    CoeffWidget* coeffWidget;
    BuildingWidget* buildingWidget;
    Room* currRoom;
    RoomWTable* roomWTable;

    static constexpr double default_temperature = 20.0;
    static constexpr double default_air_exchange = 0.5;

    bool silentMode;

public slots:
    void currentRoomChanged(Room* roomptr);

    void updateResult();
    void gui2room();
    void room2gui();
};

#endif // ROOMWIDGET_H
