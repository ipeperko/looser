#ifndef ROOMTREE_H
#define ROOMTREE_H

#include "CoeffWidget.h"
#include "Floor.h"
#include "RoomWidget.h"
#include <QStandardItemModel>
#include <QTreeView>
#include <QTreeWidget>
#include <QWidget>

#define TREE_STR_FLOOR "floor"
#define TREE_STR_FLOORNAME "name"

class RoomTree : public QWidget
{
    Q_OBJECT
public:
    explicit RoomTree(QWidget* parent = 0, CoeffWidget* coeffs = 0, RoomWidget* roomWidget = 0);
    ~RoomTree();

    void fromXML(const dbm::xml::node& xml);
    void toXML(dbm::xml::node& xml);

    int nFloors() const { return rootNode->rowCount(); }

    Floor* floor(int id);
    Floor* floor(QModelIndex index, int& id);
    Room* room(QModelIndex index, int& floor_id, int& id);
    Room* currentRoom();

private:
    QTreeView* treeView;
    QStandardItemModel* standardModel;
    QStandardItem* rootNode;

    CoeffWidget* coeffs;
    RoomWidget* roomWidget;

    Room* tmpRoom; // for copy/paste

signals:

public slots:
    void menuRequest(QPoint point);
    Floor* addFloor(const std::string& name = "New room");
    void removeFloor(int id);
    void removeFloor();
    Room* addRoom(int floor_id, const std::string& name = "");
    Room* addRoom(Floor* floor, const std::string& name = "");
    Room* addRoom();
    Room* duplicateRoom();
    void removeRoom();
    void removeRoom(int floor_id, int id);

    void copyRoom();
    void pasteRoom();

    void dataChanged(QModelIndex s, QModelIndex s2);

    void selectionChanged();
    void selectionChanged(QModelIndex s);
    void testPrint();
};

#endif // ROOMTREE_H
