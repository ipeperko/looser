#ifndef BUILDINGGENERAL_H
#define BUILDINGGENERAL_H

#include "looserdefs.h"
#include <QLineEdit>
#include <QWidget>

#define BLDG_STR_HEAD "building"
#define BLDG_STR_TIGHTNESS "tightness"
#define BLDG_STR_TOUT "tout"
#define BLDG_STR_THERMBRIDGE "thermbridge"

class BuildingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BuildingWidget(QWidget* parent = 0);
    ~BuildingWidget() {}

    double buildingTightness() const;
    double outdoorTemperature() const;
    double thermalBridgesAdd() const;

    dbm::xml::model xmlModel();
    void toXML(dbm::xml::node& xml);
    void fromXML(const dbm::xml::node& xml);

private:
    QLineEdit* leditTightness;
    QLineEdit* leditOutdoorTemp;
    QLineEdit* leditThermalBridge;

signals:

public slots:

private slots:
    void customMenu(QPoint point);
    void testslotPrint();
};

#endif // BUILDINGGENERAL_H
