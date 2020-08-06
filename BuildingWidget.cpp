#include "BuildingWidget.h"
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QSpacerItem>

BuildingWidget::BuildingWidget(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    QGridLayout* grid = new QGridLayout;

    leditTightness = new QLineEdit;
    leditOutdoorTemp = new QLineEdit;
    leditThermalBridge = new QLineEdit;

    grid->addWidget(new QLabel(tr("Outdoor temperature")), 0, 0);
    grid->addWidget(leditOutdoorTemp, 0, 1);
    grid->addWidget(new QLabel(tr("Building tightness")), 1, 0);
    grid->addWidget(leditTightness, 1, 1);
    grid->addWidget(new QLabel(tr("Thermal bridges add")), 2, 0);
    grid->addWidget(leditThermalBridge, 2, 1);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setLayout(mainLay);

    QSpacerItem* spacer = new QSpacerItem(100, 100, QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLay->addLayout(grid);
    mainLay->addSpacerItem(spacer);

    connect(this, static_cast<void (QWidget::*)(const QPoint&)>(&QWidget::customContextMenuRequested),
            this, &BuildingWidget::customMenu);
}

double BuildingWidget::buildingTightness() const
{
    return leditTightness->text().toDouble();
}

double BuildingWidget::outdoorTemperature() const
{
    return leditOutdoorTemp->text().toDouble();
}

double BuildingWidget::thermalBridgesAdd() const
{
    return leditThermalBridge->text().toDouble();
}

dbm::xml::model BuildingWidget::xmlModel()
{
    dbm::xml::model model;
    model.add_binding_func(
            BLDG_STR_TIGHTNESS,
            [&](const dbm::xml::node& x) {
                leditTightness->setText(x.value().c_str());
            },
            [&]() {
                return dbm::xml::node(BLDG_STR_TIGHTNESS, leditTightness->text().toStdString());
            });

    model.add_binding_func(
            BLDG_STR_TOUT,
            [&](const dbm::xml::node& x) {
                leditOutdoorTemp->setText(x.value().c_str());
            },
            [&]() {
                return dbm::xml::node(BLDG_STR_TOUT, leditOutdoorTemp->text().toStdString());
            });

    model.add_binding_func(
            BLDG_STR_THERMBRIDGE,
            [&](const dbm::xml::node& x) {
                leditThermalBridge->setText(x.value().c_str());
            },
            [&]() {
                return dbm::xml::node(BLDG_STR_THERMBRIDGE, leditThermalBridge->text().toStdString());
            });

    return model;
}

void BuildingWidget::toXML(dbm::xml::node& xml)
{
    xmlModel().to_xml(xml);
}

void BuildingWidget::fromXML(const dbm::xml::node& xml)
{
    xmlModel().from_xml(xml);
}

void BuildingWidget::customMenu(QPoint point)
{
    qDebug() << "Menu request" << point;
    QMenu* menu = new QMenu(this);
    QAction* actionPrint = new QAction(tr("Print"), this);

    menu->addAction(actionPrint);
    menu->popup(this->mapToGlobal(point));
    connect(actionPrint, &QAction::triggered, this, &BuildingWidget::testslotPrint);
}

void BuildingWidget::testslotPrint()
{
    dbm::xml::node xml(BLDG_STR_HEAD);
    toXML(xml);
    std::string str = xml.to_string("", 4);
    qDebug() << str.c_str();
}
