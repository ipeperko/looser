#include "RoomTree.h"
#include <QDebug>
#include <QHeaderView>
#include <QLayout>
#include <QList>
#include <QMenu>

RoomTree::RoomTree(QWidget* parent, CoeffWidget* coeffs, RoomWidget* roomWidget)
    : QWidget(parent)
    , coeffs(coeffs)
    , roomWidget(roomWidget)
{
    tmpRoom = new Room("Temp room", roomWidget);

    treeView = new QTreeView;
    standardModel = new QStandardItemModel;
    rootNode = standardModel->invisibleRootItem();

    treeView->setModel(standardModel);
    treeView->expandAll();
    treeView->setHeaderHidden(true);

    QVBoxLayout* lay = new QVBoxLayout;
    lay->addWidget(treeView);
    this->setLayout(lay);

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(menuRequest(QPoint)));
    connect(standardModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(dataChanged(QModelIndex, QModelIndex)));
    connect(treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(selectionChanged(QModelIndex)));
}

RoomTree::~RoomTree()
{
    delete tmpRoom;
}

void RoomTree::dataChanged(QModelIndex s, QModelIndex s2)
{
    qDebug() << "Changed name" << s << s2;

    if (standardModel->invisibleRootItem() == s.internalPointer()) {
        qDebug("Floor name changed");
    }
    else {
        qDebug("Room name changed");
    }

    selectionChanged(s);
}

void RoomTree::selectionChanged()
{
    QModelIndex s = treeView->currentIndex();
    if (!s.isValid())
        return;

    selectionChanged(s);
}

void RoomTree::selectionChanged(QModelIndex s)
{
    QModelIndex parent = s.parent();
    bool isValid = parent.isValid();

    if (isValid) {
        int floorId = parent.row();
        int roomId = s.row();
        Floor* F = dynamic_cast<Floor*>(rootNode->child(floorId));
        Room* R = F->room(roomId);
        if (R) {
            roomWidget->currentRoomChanged(R);
        }
    }
}

Room* RoomTree::currentRoom()
{
    int floor_id = -1;
    int room_id = -1;

    Room* _room = nullptr;
    QModelIndex index = treeView->currentIndex();

    if (!index.parent().isValid()) {
        qDebug() << "Room not selected";
    }
    else {
        _room = room(index, floor_id, room_id);
        qDebug() << "Current selection : floor" << floor_id << " room" << room_id;
    }

    return _room;
}

Floor* RoomTree::floor(QModelIndex index, int& id)
{
    QModelIndex parentIndex = index.parent();
    if (parentIndex.isValid()) {
        id = parentIndex.row();
    }
    else {
        id = index.row();
    }

    return floor(id);
}

Room* RoomTree::room(QModelIndex index, int& floor_id, int& id)
{
    Floor* f = floor(index, floor_id);
    qDebug() << "Floor id" << floor_id;

    if (!f) {
        return nullptr;
    }

    id = index.row();
    Room* R = dynamic_cast<Room*>(f->child(id));
    qDebug() << "Room id " << id;
    return R;
}

void RoomTree::removeRoom()
{
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }

    int floor_id = 0;
    Floor* f = floor(treeView->currentIndex(), floor_id);
    if (f) {
        qDebug() << "Removing room" << index.parent().data(Qt::DisplayRole).toString() << index.row();
        removeRoom(floor_id, index.row());
    }
}

void RoomTree::removeRoom(int floor_id, int id)
{
    if (floor_id >= nFloors()) {
        return;
    }

    Floor* f = dynamic_cast<Floor*>(rootNode->child(floor_id));
    f->removeRoom(id);
}

void RoomTree::copyRoom()
{
    qDebug("Copy room");

    QModelIndex index = treeView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        qWarning("Index not valid!");
        return;
    }

    Room* srcRoom = currentRoom();
    if (!srcRoom) {
        qWarning("Cannot copy NULL");
        return;
    }

    *tmpRoom = *srcRoom;
    qDebug("Copy room ok");
}

void RoomTree::pasteRoom()
{
    QModelIndex index = treeView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        qWarning("Cannot paste room - index not valid");
        return;
    }

    Room* destRoom = currentRoom();
    if (!destRoom) {
        qWarning("Cannot paste to NULL");
        return;
    }

    if (!tmpRoom) {
        qDebug("Cannot paste - tmpRoom is NULL");
        return;
    }

    *destRoom = *tmpRoom;
    qDebug("Paste room ok");
}

Room* RoomTree::duplicateRoom()
{
    QModelIndex index = treeView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        return nullptr;
    }

    Room* src = currentRoom();
    if (!src) {
        qDebug("Cannot duplicate NULL");
        return nullptr;
    }

    int floor_id = 0;
    Floor* f = floor(treeView->selectionModel()->currentIndex(), floor_id);
    if (f) {
        //qDebug() << index.parent().data(Qt::DisplayRole).toString() << index.row();
        Room* newRoom = addRoom(floor_id, src->text().toStdString().c_str());
        *newRoom = *src;
        return newRoom;
    }

    return nullptr;
}

Room* RoomTree::addRoom()
{
    qDebug("Adding room");

    QModelIndex index = treeView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        return nullptr;
    }

    int floor_id = 0;
    Floor* f = floor(treeView->selectionModel()->currentIndex(), floor_id);
    if (f) {
        qDebug() << index.parent().data(Qt::DisplayRole).toString() << index.row();
        return addRoom(floor_id, "new room");
    }

    return nullptr;
}

Room* RoomTree::addRoom(int floor_id, const std::string& name)
{
    Floor* f = floor(floor_id);
    if (!f) {
        return nullptr;
    }

    return addRoom(f, name);
}

Room* RoomTree::addRoom(Floor* floor, const std::string& name)
{
    if (!floor)
        return nullptr;

    Room* R = new Room(name, roomWidget);
    floor->appendRow(R);
    return R;
}

Floor* RoomTree::floor(int id)
{
    if (id >= nFloors()) {
        return nullptr;
    }

    return dynamic_cast<Floor*>(rootNode->child(id));
}

Floor* RoomTree::addFloor(const std::string& name)
{
    qDebug("Adding floor");
    Floor* f = new Floor(name);
    rootNode->appendRow(f);
    return f;
}

void RoomTree::removeFloor()
{
    int idx = treeView->currentIndex().row();
    removeFloor(idx);
}

void RoomTree::removeFloor(int id)
{
    if (id >= nFloors()) {
        return;
    }
    rootNode->removeRow(id);
}

void RoomTree::menuRequest(QPoint point)
{
    qDebug() << "Menu request " << point;

    QModelIndex index = treeView->indexAt(point);
    qDebug() << "index: " << index.internalPointer() << standardModel->invisibleRootItem();

    QAction* actionAddFloor = nullptr;
    QAction* actionRemoveFloor = nullptr;
    QAction* actionAddRoom = nullptr;
    QAction* actionCopyRoom = nullptr;
    QAction* actionDuplicateRoom = nullptr;
    QAction* actionPasteRoom = nullptr;
    QAction* actionRemoveRoom = nullptr;
    QAction* actionTestPrint = new QAction("PrintMe", this);
    QMenu* menu = nullptr;

    if (!index.isValid()) {
        menu = new QMenu(this);
        actionAddFloor = new QAction("Add floor", this);
        menu->addAction(actionAddFloor);
    }
    else if (index.column() == 0) {
        menu = new QMenu(this);

        QModelIndex parentIndex = index.parent();

        if (!parentIndex.isValid()) {

            actionAddFloor = new QAction("Add floor", this);
            actionRemoveFloor = new QAction("Remove floor", this);
            actionAddRoom = new QAction("Add room", this);
            menu->addAction(actionAddFloor);
            menu->addAction(actionRemoveFloor);
            menu->addAction(actionAddRoom);
        }
        else {

            actionAddRoom = new QAction("Add room", this);
            actionRemoveRoom = new QAction("Remove room", this);
            actionDuplicateRoom = new QAction("Duplicate room", this);
            actionCopyRoom = new QAction("Copy room", this);
            actionPasteRoom = new QAction("Paste room", this);
            menu->addAction(actionAddRoom);
            menu->addAction(actionRemoveRoom);
            menu->addAction(actionDuplicateRoom);
            menu->addAction(actionCopyRoom);
            menu->addAction(actionPasteRoom);
        }
    }
    else {
    }

    menu->addAction(actionTestPrint);
    menu->popup(treeView->viewport()->mapToGlobal(point));

    if (actionAddFloor) {
        QObject::connect(actionAddFloor, SIGNAL(triggered()), this, SLOT(addFloor()));
    }
    if (actionRemoveFloor) {
        QObject::connect(actionRemoveFloor, SIGNAL(triggered()), this, SLOT(removeFloor()));
    }
    if (actionDuplicateRoom) {
        QObject::connect(actionDuplicateRoom, SIGNAL(triggered()), this, SLOT(duplicateRoom()));
    }
    if (actionAddRoom) {
        QObject::connect(actionAddRoom, SIGNAL(triggered()), this, SLOT(addRoom()));
    }
    if (actionRemoveRoom) {
        QObject::connect(actionRemoveRoom, SIGNAL(triggered()), this, SLOT(removeRoom()));
    }
    if (actionCopyRoom) {
        QObject::connect(actionCopyRoom, SIGNAL(triggered()), this, SLOT(copyRoom()));
    }
    if (actionPasteRoom) {
        QObject::connect(actionPasteRoom, SIGNAL(triggered()), this, SLOT(pasteRoom()));
    }

    connect(actionTestPrint, SIGNAL(triggered()), this, SLOT(testPrint()));
}

void RoomTree::fromXML(const dbm::xml::node& xml)
{
    while (rootNode->rowCount()) {
        rootNode->removeRow(0);
    }

    unsigned fIdx = 0;
    for (auto it = xml.begin(); it != xml.end(); it++, fIdx++) {

        looser_assert(it->tag() == "floor", "Tag name error (" + it->tag() + " - expected 'floor')");
        auto name = it->find("name");
        Floor* floor = addFloor(name->value());

        for (auto jt = it->begin(); jt != it->end(); ++jt) {
            if (jt->tag() == "room") {
                Room* r = addRoom(floor);
                looser_assert(r, "No room added to floor id " + std::to_string(nFloors() - 1));
                r->fromXML(*jt, coeffs);
            }
        }
    }

    treeView->expandAll();

    if (nFloors() > 0 && floor(0)->nRooms() > 0) {
        QModelIndex first = floor(0)->room(0)->index();
        treeView->setCurrentIndex(first);

        selectionChanged(first);
    }
}

void RoomTree::toXML(dbm::xml::node& xml)
{
    for (int i = 0; i < nFloors(); i++) {
        auto& item = xml.add(TREE_STR_FLOOR);
        floor(i)->toXML(item);
    }
}

void RoomTree::testPrint()
{
    dbm::xml::node xml("xml");
    for (int i = 0; i < nFloors(); i++) {
        auto& item = xml.add((TREE_STR_FLOOR));
        floor(i)->toXML(item);
    }

    std::string str = xml.to_string("", 4);
    qDebug() << str.c_str();
}
