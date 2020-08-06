#include <Room.h>
#include <RoomWidget.h>

const char* ElementData::orientStr[] = {"E", "SE", "S", "SW", "W", "NW", "N", "NE", "hor", "-"};
const char* ElementData::behindStr[] = {"e", "g", "h", "u"};

const unsigned ElementData::nOrient = sizeof(ElementData::orientStr) / sizeof(const char*);
const unsigned ElementData::nBehind = sizeof(ElementData::behindStr) / sizeof(const char*);

Room::Room(const std::string& name, RoomWidget* roomWidget)
    : QStandardItem(name.c_str())
    , roomWidget(roomWidget)
{
    tmpElement = nullptr;
    A = h = 0;
    t = 20;
    n50 = 0;
    ei = 0.03;
    Ei = 1;
    n = 0.5;
}

Room::~Room()
{
    delete tmpElement;
}

ElementData* Room::addElement()
{
    elements.push_back(ElementData(roomWidget->building(), elements));
    return &elements.back();
}

void Room::element2Temp(int i)
{
    if (!tmpElement) {
        tmpElement = new ElementData(roomWidget->building(), elements[i], elements);
    }
    else {
        *tmpElement = elements[i];
    }
}

ElementData* Room::insertElement(int i)
{
    auto it = elements.begin() + i;
    elements.insert(it, ElementData(roomWidget->building(), elements));
    it = elements.begin() + i;
    return &*(it);
}

double Room::transLosses()
{
    double Q = 0;
    for (auto& it : elements) {
        Q += it.Q(t);
    }
    return Q;
}

std::tuple<double, double, double> Room::transLosses_n()
{
    double Q = transLosses();
    double Qa = Q / A;
    double Qv = Q / volume();
    return {Q, Qa, Qv};
}

double ElementData::Anetto()
{
    double A = Abrutto();

    if (this == &*elements.begin()) {
        return A;
    }

    if (this != &elements.back()) {

        ElementData* next = this + 1;
        if (next->orientation() == this->orientation() && this->isMinus()) {
            return A;
        }
    }

    ElementData* tmp = this;

    do {
        tmp--;
        if (orientation() == tmp->orientation() && tmp->isMinus()) {
            A -= tmp->Abrutto();
        }
        else {
            break;
        }

    } while (tmp != &*elements.begin());

    return A;
}

double ElementData::Q(double tIndoor)
{
    double q;
    if (surfaceBehind() == behind_t::ExternAir) {
        q = UeqValue() * Anetto() * (tIndoor - building->outdoorTemperature());
    }
    else {
        q = UeqValue() * Anetto() * (tIndoor - t_surf);
    }
    return q;
}

dbm::xml::model Room::xmlModel()
{
    dbm::xml::model model;
    model.add_binding_func(
            "name",
            [&](const dbm::xml::node& node) {
                setText(node.value().c_str());
            },
            [&]() {
                return dbm::xml::node("name", text().toStdString());
            });
    model.add_binding("A", A);
    model.add_binding("h", h);
    model.add_binding("t", t);
    model.add_binding("n50", n50);
    model.add_binding("ei", ei);
    model.add_binding("Ei", Ei);
    model.add_binding("n", n);
    return model;
}

void Room::toXML(dbm::xml::node& xml)
{
    xmlModel().to_xml(xml);

    for (auto& it : elements) {
        auto& item = xml.add("element");
        it.xmlModel().to_xml(item);
    }
}

dbm::xml::model ElementData::xmlModel()
{
    dbm::xml::model elmodel;
    elmodel.add_binding("minus", minus);
    elmodel.add_binding_enum<orientation_t, int>("orient", orient);
    elmodel.add_binding("k_id", k_id);
    elmodel.add_binding("n", n);
    elmodel.add_binding("length", l);
    elmodel.add_binding("width", w);
    elmodel.add_binding_enum<behind_t, int>("surf", surf_behind);
    elmodel.add_binding("t_surf", t_surf);
    elmodel.add_binding("dU", dU);
    elmodel.add_binding<std::string>("note", m_note);
    return elmodel;
}

void Room::fromXML(const dbm::xml::node& xml, CoeffWidget* coeffs)
{
    xmlModel().from_xml(xml);

    for (auto const& it : xml) {

        if (it.tag() != "element")
            continue;

        ElementData* el = addElement();
        el->xmlModel().from_xml(it);

        int pos;
        kElement* k = coeffs->k(el->kElementID(), pos);
        el->setURef(k);
    }
}
