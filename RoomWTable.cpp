#include "RoomWTable.h"
#include "CoeffWidget.h"
#include "Room.h"
#include "RoomWidget.h"
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QMenu>

RoomWTable::RoomWTable(QWidget* parent, RoomWidget* roomWidget)
    : QWidget(parent)
    , roomWidget(roomWidget)
{
    insertingNewRow = false;
    tmpElement = new ElementData(roomWidget->building(), dummyVec);

    initTable();
    table->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableMenuRequest(QPoint)));
    connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(updateTable(int, int)));
}

RoomWTable::~RoomWTable()
{
    delete tmpElement;
}

void RoomWTable::initTable()
{

    QStringList colNames;
    const int nColumns = 14;

    QVBoxLayout* lay = new QVBoxLayout;

    table = new QTableWidget(0, nColumns, this);

    colNames.append("-");
    colNames.append("Or.");
    colNames.append("Elem.");
    colNames.append("n");
    colNames.append("A/L");
    colNames.append("W/H");
    colNames.append("Anetto");
    colNames.append("Meji");
    colNames.append("Tza");
    colNames.append("U");
    colNames.append("dU");
    colNames.append("Ueq");
    colNames.append("Qt");
    colNames.append("Notes");

    table->setHorizontalHeaderLabels(colNames);

    table->setColumnWidth(0, 25);
    table->setColumnWidth(1, 50);
    table->setColumnWidth(2, 80);
    table->setColumnWidth(3, 40);
    table->setColumnWidth(4, 60);
    table->setColumnWidth(5, 60);
    table->setColumnWidth(6, 60);
    table->setColumnWidth(7, 40);
    table->setColumnWidth(8, 60);
    table->setColumnWidth(9, 60);
    table->setColumnWidth(10, 60);
    table->setColumnWidth(11, 60);
    table->setColumnWidth(12, 60);

    lay->addWidget(table);
    this->setLayout(lay);
}

void RoomWTable::room2table()
{
    for (unsigned i = 0; i < roomWidget->currentRoom()->nElements(); i++) {
        table->insertRow(table->rowCount());
        initTableRow(table->rowCount() - 1);
    }

    updateTable();
}

void RoomWTable::initTableRow(int row)
{
    if (row < 0 || row >= table->rowCount()) {
        return;
    }

    insertingNewRow = true;

    ElementData* el = roomWidget->currentRoom()->element(row);

    for (int i = 0; i < table->columnCount(); i++) {

        if (i == 0) {
            QCheckBox* box;

            if (!table->item(row, i)) {
                box = new QCheckBox;
                box->setMaximumWidth(40);
                table->setCellWidget(row, i, box);
                box->setChecked(el->isMinus());
                connect(box, SIGNAL(stateChanged(int)), this, SLOT(updateTable(int)));
            }
            else {
                box = dynamic_cast<QCheckBox*>(table->cellWidget(row, i));
            }
            box->setChecked(el->isMinus());
        }
        else if (i == 1) {
            QComboBox* combo;

            if (!table->item(row, i)) {
                combo = new QComboBox();
                table->setCellWidget(row, i, combo);
                for (unsigned n = 0; n < ElementData::nOrient; n++) {
                    combo->addItem(ElementData::orientStr[n]);
                }
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTable(int)));
            }
            else {
                combo = dynamic_cast<QComboBox*>(table->cellWidget(row, i));
            }

            combo->setCurrentIndex(static_cast<int>(el->orientation()));
        }
        else if (i == 2) {
            QComboBox* combo;

            if (!table->item(row, i)) {
                combo = new QComboBox();
                combo->addItem("?");
                for (unsigned k = 0; k < roomWidget->coeffWidget->nElements(); k++) {
                    kElement* kel = roomWidget->coeffWidget->kByRawID(k);
                    combo->addItem(kel->name().c_str());
                }
                table->setCellWidget(row, i, combo);
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTable(int)));
            }
            else {
                combo = dynamic_cast<QComboBox*>(table->cellWidget(row, i));
            }

            int rawIdx;
            roomWidget->coeffWidget->k(el->kElementID(), rawIdx);
            combo->setCurrentIndex(rawIdx);
        }
        else if (i == 7) {
            QComboBox* combo;

            if (!table->item(row, i)) {
                combo = new QComboBox();

                for (unsigned n = 0; n < ElementData::nBehind; n++) {
                    combo->addItem(ElementData::behindStr[n]);
                }

                table->setCellWidget(row, i, combo);
                connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTable(int)));
            }
            else {
                combo = dynamic_cast<QComboBox*>(table->cellWidget(row, i));
            }

            combo->setCurrentIndex(static_cast<int>(el->surfaceBehind()));
        }
        else {

            QTableWidgetItem* item = table->item(row, i);

            if (!item) {
                item = new QTableWidgetItem();
                table->setItem(row, i, item);
            }
#define NON_EDITABLE_CELL item->flags() & 0b1111001
            if (i == 3) {
                if (el->count() > 0) {
                    item->setText(QString::number(el->count()));
                }
                else {
                    item->setText("");
                }
            }
            else if (i == 4) {
                if (el->length() > 0) {
                    item->setText(QString::number(el->length()));
                }
                else {
                    item->setText("");
                }
            }
            else if (i == 5) {
                if (el->width() > 0) {
                    item->setText(QString::number(el->width()));
                }
                else {
                    item->setText("");
                }
            }
            else if (i == 6) {
                //				item->setFlags(NON_EDITABLE_CELL);
            }
            else if (i == 8) {
                if (el->surfaceBehind() == ElementData::behind_t::ExternAir) {
                    table->item(row, i)->setText("");
                    //                    table->item(row,i)->setFlags(NON_EDITABLE_CELL);
                }
                else {
                    table->item(row, i)->setText(QString::number(el->surfaceTempBehind()));
                }
            }
            else if (i == 9) {
                //				item->setFlags(NON_EDITABLE_CELL);
                item->setText(QString::number(el->UValue()));
            }
            else if (i == 10) {
                table->item(row, i)->setText(QString::number(el->dUValue()));
            }
            else if (i == 11) {
                //				item->setFlags(NON_EDITABLE_CELL);
                item->setText(QString::number(el->UeqValue()));
            }
            else if (i == 12) {
                //				item->setFlags(NON_EDITABLE_CELL);
            }
            else if (i == 13) {
                table->item(row, i)->setText(el->note().c_str());
            }
        }
    }

    if (!table->currentIndex().isValid()) {
        table->setCurrentCell(row, 0);
    }

    insertingNewRow = false;
}

void RoomWTable::removeRow()
{
    QModelIndex index = table->currentIndex();
    if (!index.isValid()) {
        return;
    }

    int i = index.row();
    roomWidget->currentRoom()->removeElement(i);
    table->removeRow(i);
}

void RoomWTable::insertRow()
{
    QModelIndex index = table->currentIndex();
    if (!index.isValid()) {
        return;
    }

    int i = index.row();

    ElementData* el = roomWidget->currentRoom()->insertElement(i);
    el->setURef(roomWidget->coeffWidget->dummyElement());
    el->setTemperatuerBehind(roomWidget->buildingWidget->outdoorTemperature());
    el->setdUValue(roomWidget->buildingWidget->thermalBridgesAdd());

    table->insertRow(i);
    initTableRow(i);
}

void RoomWTable::addRow()
{
    if (!roomWidget->currentRoom()) {
        return;
    }

    ElementData* el = roomWidget->currentRoom()->addElement();
    el->setURef(roomWidget->coeffWidget->dummyElement());
    // suggestions
    el->setTemperatuerBehind(roomWidget->buildingWidget->outdoorTemperature());
    el->setdUValue(roomWidget->buildingWidget->thermalBridgesAdd());

    table->insertRow(table->rowCount());
    initTableRow(table->rowCount() - 1);
}

void RoomWTable::updateTable(int r, int c)
{
    int rowCount = table->rowCount();
    if (rowCount == 0) {
        return;
    }

    for (int i = 0; i < table->rowCount(); i++) {
        rowDataChanged(i, 0);
    }

    roomWidget->updateResult();
}

void RoomWTable::rowDataChanged(int r, int c)
{

    if (insertingNewRow || table->rowCount() == 0 || !roomWidget->currentRoom())
        return;

    qDebug() << "Row changed" << r << "cell" << c;
    ElementData* el = roomWidget->currentRoom()->element((unsigned) r);
    int vald;
    unsigned valu;
    double valf;

    QCheckBox* check = dynamic_cast<QCheckBox*>(table->cellWidget(r, 0));
    el->setMinus(check->isChecked());

    QComboBox* combo = dynamic_cast<QComboBox*>(table->cellWidget(r, 1));
    vald = combo->currentIndex();
    el->setOrientation(static_cast<ElementData::orientation_t>(vald));

    combo = dynamic_cast<QComboBox*>(table->cellWidget(r, 2));
    vald = combo->currentIndex();
    if (vald <= 0) {
        el->setkValueID(roomWidget->coeffWidget->dummyElement()->uniqueID());
    }
    else {
        el->setkValueID(roomWidget->coeffWidget->kByRawID(vald - 1)->uniqueID());
    }

    valu = table->item(r, 3)->text().toUInt();
    el->setCount(valu);

    valf = table->item(r, 4)->text().toDouble();
    el->setLength(valf);

    valf = table->item(r, 5)->text().toDouble();
    el->setWidth(valf);

    combo = dynamic_cast<QComboBox*>(table->cellWidget(r, 7));
    vald = combo->currentIndex();
    el->setSerfaceBehind(static_cast<ElementData::behind_t>(vald));

    valf = table->item(r, 8)->text().toDouble();
    if (el->surfaceBehind() == ElementData::behind_t::ExternAir) {
        table->item(r, 8)->setText("");
    }
    else {
        el->setTemperatuerBehind(valf);
    }

    combo = dynamic_cast<QComboBox*>(table->cellWidget(r, 2));
    vald = combo->currentIndex();
    kElement* kel;
    if (vald <= 0) {
        kel = roomWidget->coeffWidget->dummyElement();
    }
    else {
        kel = roomWidget->coeffWidget->kByRawID(vald - 1);
    }
    el->setURef(kel);
    //	if (vald == 0) {
    //		el->setURef(roomWidget->coeffWidget->dummyElement());
    //	} else {
    //		el->setURef(roomWidget->coeffWidget->k(vald - 1));
    //	}
    table->item(r, 9)->setText(QString::number(el->UValue()));

    valf = table->item(r, 10)->text().toDouble();
    el->setdUValue(valf);

    //	valf = table->item(r, 11)->text().toDouble();
    //	el->setUeqValue(valf);
    table->item(r, 11)->setText(QString::number(el->UeqValue()));

    el->setNote(table->item(r, 13)->text().toStdString());

    /* Results */
    double Anetto = el->Anetto();
    table->item(r, 6)->setText(QString::number(Anetto));

    double Q = el->Q(roomWidget->currentRoom()->temperature());
    table->item(r, 12)->setText(QString::number(Q));
}

void RoomWTable::tableMenuRequest(QPoint point)
{
    QMenu* menu = new QMenu(this);
    QModelIndex index = table->indexAt(point);

    QAction* actionAddRow = new QAction(tr("Add row"), this);
    QAction* actionInsertRow = nullptr;
    QAction* actionRemoveRow = nullptr;
    QAction* actionPasteRow = nullptr;
    QAction* actionCopyRow = nullptr;
    //	QAction *actionPrint = new QAction("printme", this);

    menu->addAction(actionAddRow);

    if (index.isValid()) {
        actionInsertRow = new QAction(tr("Insert row"), this);
        menu->addAction(actionInsertRow);

        actionRemoveRow = new QAction(tr("Remove row"), this);
        menu->addAction(actionRemoveRow);

        actionCopyRow = new QAction(tr("Copy row"), this);
        menu->addAction(actionCopyRow);

        actionPasteRow = new QAction(tr("Paste row"), this);
        menu->addAction(actionPasteRow);
    }

    QPoint ptShow = table->mapToGlobal(point);
    ptShow.setX(ptShow.x() + 20);
    menu->popup(ptShow);

    if (actionAddRow) {
        connect(actionAddRow, &QAction::triggered, this, &RoomWTable::addRow);
    }
    if (actionInsertRow) {
        connect(actionInsertRow, &QAction::triggered, this, &RoomWTable::insertRow);
    }
    if (actionRemoveRow) {
        connect(actionRemoveRow, &QAction::triggered, this, &RoomWTable::removeRow);
    }
    if (actionCopyRow) {
        connect(actionCopyRow, &QAction::triggered, this, &RoomWTable::copyRow);
    }
    if (actionPasteRow) {
        connect(actionPasteRow, &QAction::triggered, this, &RoomWTable::pasteRow);
    }
}

void RoomWTable::copyRow()
{
    QModelIndex index = table->currentIndex();
    if (!index.isValid()) {
        return;
    }

    int i = index.row();

    ElementData* src = roomWidget->currentRoom()->element(i);
    tmpElement->setNote(src->note());
    tmpElement->setCount(src->count());
    tmpElement->setdUValue(src->dUValue());
    tmpElement->setkValueID(src->kElementID());
    tmpElement->setLength(src->length());
    tmpElement->setMinus(src->isMinus());
    tmpElement->setOrientation(src->orientation());
    tmpElement->setSerfaceBehind(src->surfaceBehind());
    tmpElement->setTemperatuerBehind(src->surfaceTempBehind());
    tmpElement->setURef(src->URef());
    tmpElement->setWidth(src->width());
    //	roomWidget->currentRoom()->element2Temp(i);
}

void RoomWTable::pasteRow()
{
    QModelIndex index = table->currentIndex();
    if (!index.isValid()) {
        return;
    }

    int i = index.row();

    ElementData* dest = roomWidget->currentRoom()->element(i);
    dest->setNote(tmpElement->note());
    dest->setCount(tmpElement->count());
    dest->setdUValue(tmpElement->dUValue());
    dest->setkValueID(tmpElement->kElementID());
    dest->setLength(tmpElement->length());
    dest->setMinus(tmpElement->isMinus());
    dest->setOrientation(tmpElement->orientation());
    dest->setSerfaceBehind(tmpElement->surfaceBehind());
    dest->setTemperatuerBehind(tmpElement->surfaceTempBehind());
    dest->setURef(tmpElement->URef());
    dest->setWidth(tmpElement->width());

    //	roomWidget->currentRoom()->tmp2Element(i);
    initTableRow(i);
    updateTable();
}
