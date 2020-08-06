#ifndef ROOM_H
#define ROOM_H

#include "CoeffWidget.h"
#include <BuildingWidget.h>
#include <QStandardItem>
#include <vector>

class RoomWidget;
class BuildingWidget;

class ElementData
{
    friend class RoomWidget;

public:
    static const char* orientStr[];
    static const unsigned nOrient;

    enum class orientation_t : int
    {
        hor = 0,
        E,
        SE,
        S,
        SW,
        W,
        NW,
        N,
        NE,
        Orint_undefined
    };

    static const char* behindStr[];
    static const unsigned nBehind;

    enum class behind_t : int
    {
        ExternAir = 0,
        Ground,
        HeatedRoom,
        UnheatedRoom
    };

    ElementData(BuildingWidget* building, std::vector<ElementData>& vec)
        : building(building)
        , elements(vec)
    {
        minus = false;
        orient = orientation_t::hor;
        k_id = 0;
        n = l = w = 0;
        surf_behind = behind_t::ExternAir;
        t_surf = 0;
        dU = 0;
        U = nullptr;
    }
    ~ElementData() {}
    ElementData(BuildingWidget* building, const ElementData& oth, std::vector<ElementData>& vec)
        : building(building)
        , elements(vec)
    {
        elements = oth.elements;
        *this = oth;
    }
    ElementData& operator=(const ElementData& oth)
    {
        minus = oth.minus;
        orient = oth.orient;
        k_id = oth.k_id;
        n = oth.n;
        l = oth.l;
        w = oth.w;
        surf_behind = oth.surf_behind;
        t_surf = oth.t_surf;
        U = oth.U;
        dU = oth.dU;
        m_note = oth.m_note;

        return *this;
    }

    dbm::xml::model xmlModel();

    double Abrutto() { return l * w * n; }
    double Anetto(double Asub) { return Abrutto() - Asub; }
    double Anetto();

    double Q(double tIndoor);

    void setMinus(bool min) { minus = min; }
    void setOrientation(orientation_t o) { orient = o; }
    void setkValueID(unsigned id) { k_id = id; }
    void setCount(unsigned val) { n = val; }
    void setLength(double val) { l = val; }
    void setWidth(double val) { w = val; }
    void setSerfaceBehind(behind_t val) { surf_behind = val; }
    void setTemperatuerBehind(double val) { t_surf = val; }
    void setURef(kElement* k) { U = k; }
    void setdUValue(double val) { dU = val; }
    void setNote(const std::string& str) { m_note = str; }

    bool isMinus() const { return minus; }
    orientation_t orientation() const { return orient; }
    const char* orientationStr() const { return orientStr[(int) orient]; }
    unsigned kElementID() const { return k_id; }
    unsigned count() const { return n; }
    double length() const { return l; }
    double width() const { return w; }
    behind_t surfaceBehind() const { return surf_behind; }
    const char* surfaceBehindStr() const { return behindStr[(int) surf_behind]; }
    double surfaceTempBehind() const { return t_surf; }
    kElement* URef() { return U; }
    double UValue() const { return U->kValue(); }
    double dUValue() const { return dU; }
    double UeqValue() const { return dU + U->kValue(); }
    const std::string& note() const { return m_note; }

private:
    bool minus;
    orientation_t orient;
    unsigned k_id;
    unsigned n;
    double l;
    double w;
    behind_t surf_behind;
    double t_surf;
    kElement* U;
    double dU;
    std::string m_note;

    BuildingWidget* building;
    std::vector<ElementData>& elements;
};

class Room : public QStandardItem
{

public:
    Room(const std::string& name, RoomWidget* roomWidget);
    ~Room();

    Room& operator=(const Room& oth)
    {
        setText(oth.text());
        elements.clear();
        elements = oth.elements;
        A = oth.A;
        h = oth.h;
        t = oth.t;
        n50 = oth.n50;
        ei = oth.ei;
        Ei = oth.Ei;
        n = oth.n;

        return *this;
    }

    dbm::xml::model xmlModel();
    void fromXML(const dbm::xml::node& xml, CoeffWidget* coeffs);
    void toXML(dbm::xml::node& xml);

    ElementData* insertElement(int i);
    ElementData* addElement();
    void removeElement(int i) { elements.erase(elements.begin() + i); }

    ElementData* element(unsigned i)
    {
        std::vector<ElementData>::iterator it = elements.begin() + i;
        return &*it;
    }

    void element2Temp(int i);

    void tmp2Element(int i)
    {
        if (!tmpElement) {
            return;
        }
        elements[i] = *tmpElement;
    }

    void setArea(double val) { A = val; }
    void setHeight(double val) { h = val; }
    void setTemperature(double val) { t = val; }
    void setTightness(double val) { n50 = val; }
    void setBldgProtection(double val) { ei = val; }
    void setBldgHeightFactor(double val) { Ei = val; }
    void setAirExchangeRate(double val) { n = val; }

    double area() const { return A; }
    double height() const { return h; }
    double temperature() const { return t; }
    double tightness() const { return n50; }
    double bldgProtection() const { return ei; }
    double bldgHeightFactor() const { return Ei; }
    double airExchangeRate() const { return n; }

    unsigned nElements() const { return elements.size(); }
    double volume() const { return A * h; }
    double airInfiltration() const { return 2 * volume() * ei * Ei * n50; }
    double airExchange() const { return volume() * airExchangeRate(); }
    double maxAirExchange() const
    {
        double infiltration = airInfiltration();
        double exchange = airExchange();

        return std::max(infiltration, exchange);
    }

    double ventLosses(double tOutdoor)
    {
        return maxAirExchange() / 3600 * 1.2f * 1005 * (t - tOutdoor);
    }

    double transLosses();
    std::tuple<double, double, double> transLosses_n();

private:
    RoomWidget* roomWidget;
    std::vector<ElementData> elements;
    ElementData* tmpElement {nullptr};

    double A;
    double h;
    double t;
    double n50;
    double ei;
    double Ei;
    double n; // air exchange rate
};

#endif // ROOM_H
