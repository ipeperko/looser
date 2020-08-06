#ifndef COEFFWIDGET_H
#define COEFFWIDGET_H

#include <QLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QWidget>
#include <list>
#include "looserdefs.h"

class kElement
{
    friend class CoeffWidget;

public:
    kElement()
        : m_uniqid(0)
    {
        m_k = 0.0f;
    }
    kElement(const char* name, double k, const char* note)
        : m_uniqid(0)
        , m_name(name)
        , m_k(k)
        , m_note(note)
    {}
    kElement(const kElement& other)
        : m_uniqid(0)
    {
        *this = other;
        //		setUniqueID();
    }

    kElement& operator=(const kElement& other)
    {
        this->m_name = other.m_name;
        this->m_k = other.m_k;
        this->m_note = other.m_note;
        return *this;
    }

    ~kElement() {}

    void setName(const char* idStr) { m_name = idStr; }
    void setkValue(double val) { m_k = val; }
    void setNote(const char* str) { m_note = str; }

    double kValue() const { return m_k; }
    const std::string& name() const { return m_name; }
    const std::string& note() const { return m_note; }
    unsigned uniqueID() const { return m_uniqid; }

    dbm::xml::model xmlModel();

private:
    unsigned m_uniqid;
    std::string m_name;
    double m_k;
    std::string m_note;

    void setAutoUniqueID();
    void setUniqueID(unsigned val) { m_uniqid = val; }
};

class CoeffWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CoeffWidget(QWidget* parent = 0);
    ~CoeffWidget() {}

    unsigned nElements() const { return klist.size(); }

    kElement* dummyElement() { return &dummy_element; }

    kElement* kByRawID(int pos)
    {
        if (pos < 0 || (unsigned) pos > nElements()) {
            return &dummy_element;
        }
        std::list<kElement>::iterator it = klist.begin();
        std::advance(it, pos);
        return &(*it);
    }

    kElement* k(unsigned uniqid, int& pos);

    void toXML(dbm::xml::node& xml);
    void fromXML(const dbm::xml::node& xml);

    static unsigned getUniqueID();

private:
    QVBoxLayout* mainLayout;
    QTableWidget* table;
    QPushButton* buttonAddRow;

    static const int nColumns = 3;

    std::list<kElement> klist;
    kElement tmpElement;    // copy/paste
    kElement dummy_element; // k=0

    static unsigned counter;

    void removeElement(int pos)
    {
        if (pos < 0 || (unsigned) pos > nElements()) {
            return;
        }

        std::list<kElement>::iterator it = klist.begin();
        std::advance(it, pos);
        klist.erase(it);
    }

    void removeAll()
    {
        while (table->rowCount()) {
            table->removeRow(0);
        }
        while (nElements()) {
            removeElement(0);
        }
    }

    kElement* addElement(int pos);

signals:

    void dataChanged();

private slots:
    void addRow(bool autoSetID = true);
    void insertRow();
    void removeRow();
    void copyRow();
    void pasteRow();
    void menuRequest(QPoint point);
    void row2kelement(int, int);
    void tmpElement2row(int row);
    void printList();
};

#endif // COEFFWIDGET_H
