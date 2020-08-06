#ifndef ROOMWTABLE_H
#define ROOMWTABLE_H

#include <QTableWidget>
#include <QWidget>

class RoomWidget;
class ElementData;

class RoomWTable : public QWidget
{
    friend class RoomWidget;

    Q_OBJECT
public:
    explicit RoomWTable(QWidget* parent = 0, RoomWidget* roomWidget = nullptr);
    ~RoomWTable();

    void initTable();

private:
    void initTableRow(int row);

    RoomWidget* roomWidget {nullptr};
    QTableWidget* table {nullptr};

    bool insertingNewRow;

    ElementData* tmpElement;
    std::vector<ElementData> dummyVec;

public slots:
    void removeRow();
    void insertRow();
    void addRow();
    void copyRow();
    void pasteRow();

    void rowDataChanged(int r, int c);
    void tableMenuRequest(QPoint point);
    void updateTable(int r = 0, int c = 0);

    void room2table();
};

#endif // ROOMWTABLE_H
