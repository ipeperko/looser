#include "RoomWidget.h"
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QMenu>
#include <QObjectList>
#include <QSpacerItem>

RoomWidget::RoomWidget(QWidget* parent, CoeffWidget* coeffWidget, BuildingWidget* buildingWidget)
    : QWidget(parent)
    , coeffWidget(coeffWidget)
    , buildingWidget(buildingWidget)
{
    currRoom = nullptr;
    silentMode = false;

    std::list<QLineEdit*> editWidgets;

    mainLay = new QVBoxLayout;
    QHBoxLayout* topLay = new QHBoxLayout;
    QHBoxLayout* resultLay = new QHBoxLayout;

    QGridLayout* grid = new QGridLayout;
    roomName = new QLabel(tr("Room -"));
    leditArea = new QLineEdit;
    leditHeight = new QLineEdit;
    leditVolume = new QLineEdit;
    leditTemperature = new QLineEdit(QString::number(default_temperature));
    leditOutdoorTemp = new QLineEdit;

    int row = 0;
    QVBoxLayout* tmpVLayout = new QVBoxLayout;
    grid->addWidget(new QLabel(tr("Area")), row, 0);
    grid->addWidget(leditArea, row, 1);
    grid->addWidget(new QLabel("m2"), row, 2);
    row++;
    grid->addWidget(new QLabel(tr("Height")), row, 0);
    grid->addWidget(leditHeight, row, 1);
    grid->addWidget(new QLabel("m"), row, 2);
    row++;
    grid->addWidget(new QLabel(tr("Volume")), row, 0);
    grid->addWidget(leditVolume, row, 1);
    leditVolume->setReadOnly(true);
    grid->addWidget(new QLabel("m3"), row, 2);
    row++;
    grid->addWidget(new QLabel("Temperatura"), row, 0);
    grid->addWidget(leditTemperature, row, 1);
    grid->addWidget(new QLabel("°C"), row, 2);
    row++;
    grid->addWidget(new QLabel(tr("Outdoor temperature")), row, 0);
    grid->addWidget(leditOutdoorTemp, row, 1);
    leditOutdoorTemp->setReadOnly(true);
    grid->addWidget(new QLabel("°C"), row, 2);
    tmpVLayout->addWidget(roomName);
    tmpVLayout->addLayout(grid);
    topLay->addLayout(tmpVLayout);
    topLay->addItem(new QSpacerItem(100, 100, QSizePolicy::Minimum, QSizePolicy::Minimum));

    editWidgets.push_back(leditArea);
    editWidgets.push_back(leditHeight);
    editWidgets.push_back(leditTemperature);

    grid = new QGridLayout;
    leditCoeff_n50 = new QLineEdit;
    leditCoeff_n50->setReadOnly(true);
    leditCoeff_ei = new QLineEdit("0.03");
    leditCoeff_theta = new QLineEdit("1");
    leditInfiltration = new QLineEdit;
    leditInfiltration->setReadOnly(true);
    leditMinAirExchange = new QLineEdit(QString::number(default_air_exchange));
    leditReqAirFlow = new QLineEdit;
    leditReqAirFlow->setReadOnly(true);
    leditMaxAirFlow = new QLineEdit;
    leditMaxAirFlow->setReadOnly(true);

    grid->addWidget(new QLabel(tr("Air exchange coeff n50")), 0, 0);
    grid->addWidget(leditCoeff_n50, 0, 1);
    grid->addWidget(new QLabel("h-1"), 0, 2);
    grid->addWidget(new QLabel(tr("Building wind coeff ei")), 1, 0);
    grid->addWidget(leditCoeff_ei, 1, 1);
    grid->addWidget(new QLabel("-"), 1, 2);
    grid->addWidget(new QLabel(tr("Height correction Ei")), 2, 0);
    grid->addWidget(leditCoeff_theta, 2, 1);
    grid->addWidget(new QLabel("-"), 2, 2);
    grid->addWidget(new QLabel(tr("Air infiltration")), 3, 0);
    grid->addWidget(leditInfiltration, 3, 1);
    grid->addWidget(new QLabel("m3/h"), 3, 2);
    grid->addWidget(new QLabel(tr("Min. air exchange")), 4, 0);
    grid->addWidget(leditMinAirExchange, 4, 1);
    grid->addWidget(new QLabel("h-1"), 4, 2);
    grid->addWidget(new QLabel(tr("Required air volume")), 5, 0);
    grid->addWidget(leditReqAirFlow, 5, 1);
    grid->addWidget(new QLabel("m3/h"), 5, 2);
    grid->addWidget(new QLabel(tr("Max air volume")), 6, 0);
    grid->addWidget(leditMaxAirFlow, 6, 1);
    grid->addWidget(new QLabel("m3/h"), 6, 2);
    topLay->addLayout(grid);
    topLay->addItem(new QSpacerItem(100, 0, QSizePolicy::Minimum, QSizePolicy::Minimum));

    editWidgets.push_back(leditCoeff_ei);
    editWidgets.push_back(leditCoeff_theta);
    editWidgets.push_back(leditMinAirExchange);

    grid = new QGridLayout;
    labelResultVent = new QLabel("0");
    labelResultTrans = new QLabel("0");
    labelResultSum = new QLabel("0");
    labelResultVent_m2 = new QLabel("0");
    labelResultTrans_m2 = new QLabel("0");
    labelResultSum_m2 = new QLabel("0");
    labelResultVent_m3 = new QLabel("0");
    labelResultTrans_m3 = new QLabel("0");
    labelResultSum_m3 = new QLabel("0");
    grid->addWidget(new QLabel(tr("Ventilation losses ")), 0, 0);
    grid->addWidget(labelResultVent, 0, 1);
    grid->addWidget(new QLabel("W"), 0, 2);
    grid->addWidget(labelResultVent_m2, 0, 3);
    grid->addWidget(new QLabel("W/m2"), 0, 4);
    grid->addWidget(labelResultVent_m3, 0, 5);
    grid->addWidget(new QLabel("W/m3"), 0, 6, 1, 1, Qt::AlignLeft);
    grid->addWidget(new QLabel(tr("Transmission losses ")), 1, 0);
    grid->addWidget(labelResultTrans, 1, 1);
    grid->addWidget(new QLabel("W"), 1, 2);
    grid->addWidget(labelResultTrans_m2, 1, 3);
    grid->addWidget(new QLabel("W/m2"), 1, 4);
    grid->addWidget(labelResultTrans_m3, 1, 5);
    grid->addWidget(new QLabel("W/m3"), 1, 6);
    grid->addWidget(new QLabel(tr("Total losses")), 2, 0);
    grid->addWidget(labelResultSum, 2, 1);
    grid->addWidget(new QLabel("W"), 2, 2);
    grid->addWidget(labelResultSum_m2, 2, 3);
    grid->addWidget(new QLabel("W/m2"), 2, 4);
    grid->addWidget(labelResultSum_m3, 2, 5);
    grid->addWidget(new QLabel("W/m3"), 2, 6);
    resultLay->addLayout(grid);
    resultLay->addItem(new QSpacerItem(200, 100));

    roomWTable = new RoomWTable(nullptr, this);

    mainLay->addLayout(topLay);
    mainLay->addWidget(roomWTable);
    mainLay->addLayout(resultLay);
    this->setLayout(mainLay);

    for (auto& it : editWidgets) {
        connect(it, &QLineEdit::editingFinished, this, &RoomWidget::gui2room);
    }
}

void RoomWidget::currentRoomChanged(Room* roomptr)
{
    int rows = roomWTable->table->rowCount();
    for (int i = 0; i < rows; i++) {
        roomWTable->table->removeRow(0);
    }

    currRoom = roomptr;
    roomName->setText(currRoom->text());

    room2gui();
}

void RoomWidget::room2gui()
{
    Room* r = currRoom;
    if (!r) {
        return;
    }

    silentMode = true;

    leditArea->setText(QString::number(r->area()));
    leditHeight->setText(QString::number(r->height()));
    leditVolume->setText(QString::number(r->volume()));
    leditTemperature->setText(QString::number(r->temperature()));
    double tOutdoor = buildingWidget->outdoorTemperature();
    leditOutdoorTemp->setText(QString::number(tOutdoor));
    leditCoeff_n50->setText(QString::number(buildingWidget->buildingTightness()));
    double valf = leditCoeff_n50->text().toDouble();
    r->setTightness(valf);

    leditCoeff_theta->setText(QString::number(r->bldgHeightFactor()));
    leditCoeff_ei->setText(QString::number(r->bldgProtection()));
    leditInfiltration->setText(QString::number(r->airInfiltration()));
    leditMinAirExchange->setText(QString::number(r->airExchangeRate()));
    leditReqAirFlow->setText(QString::number(r->airExchange()));
    leditMaxAirFlow->setText(QString::number(r->maxAirExchange()));

    roomWTable->room2table();

    updateResult();

    silentMode = false;
}

void RoomWidget::gui2room()
{
    Room* r = currRoom;
    if (!r || silentMode) {
        return;
    }

    silentMode = true;

    double valf;
    double airFlow;
    //	double Qv, Qt, Qtotal;

    valf = leditArea->text().toDouble();
    r->setArea(valf);

    valf = leditHeight->text().toDouble();
    r->setHeight(valf);

    leditVolume->setText(QString::number(r->volume()));

    valf = leditTemperature->text().toDouble();
    r->setTemperature(valf);

    double tOutdoor = buildingWidget->outdoorTemperature();
    leditOutdoorTemp->setText(QString::number(tOutdoor));

    leditCoeff_n50->setText(QString::number(buildingWidget->buildingTightness()));
    valf = leditCoeff_n50->text().toDouble();
    r->setTightness(valf);

    valf = leditCoeff_theta->text().toDouble();
    r->setBldgHeightFactor(valf);

    valf = leditCoeff_ei->text().toDouble();
    r->setBldgProtection(valf);

    leditInfiltration->setText(QString::number(r->airInfiltration()));

    valf = leditMinAirExchange->text().toDouble();
    r->setAirExchangeRate(valf);

    leditReqAirFlow->setText(QString::number(r->airExchange()));

    airFlow = r->maxAirExchange();
    leditMaxAirFlow->setText(QString::number(airFlow));

    updateResult();

    silentMode = false;
}

void RoomWidget::updateResult()
{
    double tOutdoor = buildingWidget->outdoorTemperature();

    double Qvent = currRoom->ventLosses(tOutdoor); //airFlow / 3600 * 1.2f * 1005 * (r->temperature() - buildingWidget->outdoorTemperature());
    double Qtrans = currRoom->transLosses();
    double Qtotal = Qtrans + Qvent;

    double A = currRoom->area();
    double Qvent_m2 = Qvent / A;
    double Qtrans_m2 = Qtrans / A;
    double Qtotal_m2 = Qtotal / A;

    double V = currRoom->volume();
    double Qvent_m3 = Qvent / V;
    double Qtrans_m3 = Qtrans / V;
    double Qtotal_m3 = Qtotal / V;

    labelResultVent->setText(QString::number(Qvent));
    labelResultTrans->setText(QString::number(Qtrans));
    labelResultSum->setText(QString::number(Qtotal));

    labelResultVent_m2->setText(QString::number(Qvent_m2));
    labelResultTrans_m2->setText(QString::number(Qtrans_m2));
    labelResultSum_m2->setText(QString::number(Qtotal_m2));

    labelResultVent_m3->setText(QString::number(Qvent_m3));
    labelResultTrans_m3->setText(QString::number(Qtrans_m3));
    labelResultSum_m3->setText(QString::number(Qtotal_m3));
}
