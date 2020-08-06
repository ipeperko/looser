#include "Floor.h"
#include <QDebug>

Floor::Floor(const std::string& name)
    : QStandardItem(name.c_str())
{
}

Floor::~Floor()
{
}

//Room *Floor::addRoom(const std::string &name)
//{
//    Room* R = new Room(name);
//    appendRow(R);
//    return R;
//}

void Floor::removeRoom(int id)
{
    removeRow(id);
}

void Floor::toXML(dbm::xml::node& xml)
{
    xml.add(FLOOR_STR_FLOORNAME, this->text().toStdString());

    for (int i = 0; i < nRooms(); i++) {
        Room* r = room(i);
        auto& roomItem = xml.add(FLOOR_STR_ROOM);
        r->toXML(roomItem);
    }
}
