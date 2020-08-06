#include "CoeffWidget.h"
#include <QDebug>
#include <QMenu>

unsigned CoeffWidget::counter = 0;

dbm::xml::model kElement::xmlModel()
{
    dbm::xml::model model;
    model.add_binding("uniqid", m_uniqid);
    model.add_binding("name", m_name);
    model.add_binding("k", m_k);
    model.add_binding("note", m_note);
    return model;
}

CoeffWidget::CoeffWidget(QWidget* parent)
    : QWidget(parent)
{
    tmpElement.setAutoUniqueID();
    dummy_element.setAutoUniqueID();

    mainLayout = new QVBoxLayout;

    table = new QTableWidget(0, nColumns, this);
    buttonAddRow = new QPushButton(tr("Add row"));
    mainLayout->addWidget(table);
    mainLayout->addWidget(buttonAddRow);
    setLayout(mainLayout);

    QStringList colNames;
    colNames.append(tr("Element"));
    colNames.append("k (W/m2K)");
    colNames.append(tr("Comment"));

    table->setColumnWidth(2, 400);
    table->setHorizontalHeaderLabels(colNames);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(buttonAddRow, &QPushButton::clicked, this, &CoeffWidget::addRow);
    connect(table, static_cast<void (QWidget::*)(const QPoint&)>(&QWidget::customContextMenuRequested),
            this, &CoeffWidget::menuRequest);
    connect(table, static_cast<void (QTableWidget::*)(int, int)>(&QTableWidget::cellChanged),
            this, &CoeffWidget::row2kelement);
}

kElement* CoeffWidget::k(unsigned uniqid, int& pos)
{
    pos = 0;
    for (auto& it : klist) {
        ++pos;
        if (uniqid == it.uniqueID()) {
            return &it;
        }
    }

    pos = 0;
    return &dummy_element;
}

unsigned CoeffWidget::getUniqueID()
{
    counter++;
    return counter;
}

kElement* CoeffWidget::addElement(int pos)
{
    if (pos < 0 || (unsigned) pos > nElements()) {
        return nullptr;
    }
    auto it = klist.begin();
    std::advance(it, pos);
    klist.insert(it, kElement());
    --it;
    (*it).setAutoUniqueID();
    return &(*it);
}

void kElement::setAutoUniqueID()
{
    m_uniqid = CoeffWidget::getUniqueID();
}

void CoeffWidget::row2kelement(int row, int col)
{
    qDebug() << "Element changed " << row << col;
    kElement* ke = kByRawID(row);

    if (!ke) {
        qWarning("k is null pointer %s", __FUNCTION__);
        return;
    }

    if (table->item(row, 0)) {
        ke->setName(table->item(row, 0)->text().toStdString().c_str());
    }
    if (table->item(row, 1)) {
        ke->setkValue(table->item(row, 1)->text().toDouble());
    }
    if (table->item(row, 2)) {
        ke->setNote(table->item(row, 2)->text().toStdString().c_str());
    }

    emit dataChanged();
}

void CoeffWidget::addRow(bool autoSetID)
{
    table->insertRow(table->rowCount());
    klist.emplace_back();
    if (autoSetID) {
        klist.back().setAutoUniqueID();
    }

    int r = table->rowCount() - 1;
    for (int i = 0; i < table->columnCount(); ++i) {
        table->setItem(r, i, new QTableWidgetItem());
    }

    emit dataChanged();
}

void CoeffWidget::removeRow()
{
    QModelIndex index = table->currentIndex();

    if (!index.isValid()) {
        qWarning("Index not valid");
        return;
    }

    table->removeRow(index.row());
    removeElement(index.row());

    emit dataChanged();
}

void CoeffWidget::insertRow()
{
    QModelIndex index = table->currentIndex();

    if (!index.isValid()) {
        qWarning("Index not valid - appending row");
        addRow();
        return;
    }

    addElement(index.row());

    table->insertRow(index.row());
    int r = index.row();
    for (int i = 0; i < table->columnCount(); ++i) {
        table->setItem(r, i, new QTableWidgetItem());
    }

    emit dataChanged();
}

void CoeffWidget::copyRow()
{
    QModelIndex index = table->currentIndex();
    if (!index.isValid()) {
        qWarning("Index not valid - cannot copy row");
        return;
    }

    kElement* ke = kByRawID(index.row());
    if (!ke) {
        qWarning("Null pointer - cannot copy row");
        return;
    }

    tmpElement = *ke;
}

void CoeffWidget::pasteRow()
{
    QModelIndex index = table->currentIndex();
    if (!index.isValid()) {
        qWarning("Index not valid - cannot paste row");
        return;
    }

    kElement* ke = kByRawID(index.row());
    if (!ke) {
        qWarning("Null pointer - cannot paste row");
        return;
    }
    *ke = tmpElement;
    tmpElement2row(index.row());

    emit dataChanged();
}

void CoeffWidget::tmpElement2row(int row)
{
    if (row < 0 || row >= table->rowCount()) {
        return;
    }

    table->item(row, 0)->setText(tmpElement.name().c_str());
    table->item(row, 1)->setText(QString::number(tmpElement.kValue()));
    table->item(row, 2)->setText(tmpElement.note().c_str());
}

void CoeffWidget::menuRequest(QPoint point)
{
    QModelIndex index = table->indexAt(point);
    qDebug() << "Menu request" << index;
    auto* menu = new QMenu(this);

    auto* actionCopy = new QAction(tr("Copy element"), this);
    auto* actionPaste = new QAction(tr("Paste element"), this);
    auto* actionAddRow = new QAction(tr("Add row"), this);
    auto* actionRemoveRow = new QAction(tr("Remove row"), this);
    auto* actionInsertRow = new QAction(tr("Insert row"), this);
    auto* actionPrintList = new QAction(tr("Print list"), this);

    menu->addAction(actionCopy);
    menu->addAction(actionPaste);
    menu->addAction(actionInsertRow);
    menu->addAction(actionAddRow);
    menu->addAction(actionRemoveRow);
    menu->addAction(actionPrintList);

    menu->popup(table->viewport()->mapToGlobal(point));

    connect(actionAddRow, &QAction::triggered, this, &CoeffWidget::addRow);
    connect(actionRemoveRow, &QAction::triggered, this, &CoeffWidget::removeRow);
    connect(actionInsertRow, &QAction::triggered, this, &CoeffWidget::insertRow);
    connect(actionCopy, &QAction::triggered, this, &CoeffWidget::copyRow);
    connect(actionPaste, &QAction::triggered, this, &CoeffWidget::pasteRow);
    connect(actionPrintList, &QAction::triggered, this, &CoeffWidget::printList);
}

void CoeffWidget::printList()
{
    for (const auto& it : klist) {
        qDebug() << it.uniqueID() << it.name().c_str() << it.kValue() << it.note().c_str();
    }

    dbm::xml::node xml("kvalues_example");
    toXML(xml);

    std::string str = xml.to_string("", 4);
    qDebug("\nXML string:\n%s", str.c_str());
}

/* <kvalue> ... </kvalue><kvalue> ... </kvalue> ... */
void CoeffWidget::toXML(dbm::xml::node& xml)
{
    for (unsigned i = 0; i < nElements(); i++) {
        kElement* el = kByRawID(i);
        auto model = el->xmlModel();
        auto& item = xml.add("kvalue");
        model.to_xml(item);
    }
}

/* @xml: example string: [ <kvalue> ... </kvalue><kvalue> ... </kvalue> ... ] */
void CoeffWidget::fromXML(const dbm::xml::node& xml)
{
    removeAll();
    counter = 0;

    for (auto& it : xml) {

        addRow(false);
        int r = table->rowCount() - 1;

        kElement& kItem_ = klist.back();
        auto model = kItem_.xmlModel();
        model.from_xml(it);

        if (kItem_.uniqueID() > counter)
            counter = kItem_.uniqueID();

        kElement kItem = kItem_;
        table->item(r, 0)->setText(kItem.name().c_str());
        table->item(r, 1)->setText(QString::number(kItem.kValue()));
        table->item(r, 2)->setText(kItem.note().c_str());
    }
}
