#ifndef FLOOR_H
#define FLOOR_H

#include "Room.h"
#include <QStandardItem>

#define FLOOR_STR_FLOORNAME "name"
#define FLOOR_STR_ROOM "room"

class Floor : public QStandardItem
{
public:
    explicit Floor(const std::string& name = "");
    ~Floor();
    void removeRoom(int id);

    Room* room(int id)
    {
        looser_assert(id < nRooms(), "Room id overflow");
        return dynamic_cast<Room*>(child(id));
    }

    int nRooms() const { return rowCount(); }

    void fromXML(const dbm::xml::node& xml);
    void toXML(dbm::xml::node& xml);

private:
};

#endif // FLOOR_H
