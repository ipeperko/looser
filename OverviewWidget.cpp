#include "OverviewWidget.h"
#include <QDebug>
#include <QVBoxLayout>

struct FloorResult
{
    double Qtot;
    double Qtrans;
    double Qvent;

    double A;
    double V;

    FloorResult()
    {
        Qtot = Qtrans = Qvent = A = V = 0;
    }
};

OverviewWidget::OverviewWidget(QWidget* parent, RoomTree* tree, BuildingWidget* building)
    : QWidget(parent)
    , tree(tree)
    , building(building)
{
    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    buttCalc = new QPushButton(tr("Overview"));
    area = new QTextEdit;

    layout->addWidget(buttCalc);
    layout->addWidget(area);

    QFont font = area->font();
    font.setFamily("Monospace");
    font.setPointSize(font.pointSize() - 2);
    area->setFont(font);
    area->setText(tr("Press ..."));

    connect(buttCalc, &QPushButton::clicked, this, &OverviewWidget::calculate);
}

std::string& numFormat(std::string& str, double num, unsigned N = 10)
{
    char tmp[32] = {0};
    sprintf(tmp, "%.1f", num);
    str = tmp;

    while (str.size() < N) {
        str.insert(str.begin(), ' ');
    }

    return str;
}

std::string& appendSpace(std::string& str, unsigned N = 10)
{
    while (str.size() < N) {
        str.push_back(' ');
    }

    return str;
}

std::string& insertSpace(std::string& str, unsigned N = 10)
{
    while (str.size() < N) {
        str.insert(str.begin(), ' ');
    }

    return str;
}

#define NAME_LEN 30
#define NUM_LEN 9

void OverviewWidget::calculate()
{
    area->clear();

    unsigned nFloors = tree->nFloors();
    std::vector<FloorResult> floorVec;
    double tOutdoor = building->outdoorTemperature();

    char headString[256];
    std::string headName = "Room";
    appendSpace(headName, NAME_LEN);
    std::string headA = "A (m2)";
    insertSpace(headA, NUM_LEN);
    std::string headV = "V (m3)";
    insertSpace(headV, NUM_LEN);
    std::string headQtrans = "Qt (W)";
    insertSpace(headQtrans, NUM_LEN);
    std::string headQvent = "Qv (W)";
    insertSpace(headQvent, NUM_LEN);
    std::string headQtotal = "Q (W)";
    insertSpace(headQtotal, NUM_LEN);
    std::string headQ_m2 = "(W/m2)";
    insertSpace(headQ_m2, NUM_LEN);
    std::string headQ_m3 = "(W/m3)";
    insertSpace(headQ_m3, NUM_LEN);

    sprintf(headString, "%s%s%s%s%s%s%s%s\n",
            headName.c_str(), headA.c_str(), headV.c_str(), headQtrans.c_str(), headQvent.c_str(), headQtotal.c_str(), headQ_m2.c_str(), headQ_m3.c_str());

    std::string output;
    output.clear();

    for (unsigned f = 0; f < nFloors; f++) {

        floorVec.push_back(FloorResult());

        Floor* floor = tree->floor(f);
        unsigned nRooms = floor->nRooms();
        FloorResult* floorRes = &floorVec.back();

        output.append(headString);

        for (unsigned x = 0; x < NAME_LEN + 7 * NUM_LEN; x++) {
            output.push_back('-');
        }
        output.push_back('\n');

        for (unsigned i = 0; i < nRooms; i++) {

            Room* room = floor->room(i);

            double Qvent = room->ventLosses(tOutdoor);
            double Qtrans = room->transLosses();
            double Qtot = Qtrans + Qvent;
            double A = room->area();
            double V = room->volume();

            floorRes->Qtot += Qtot;
            floorRes->Qtrans += Qtrans;
            floorRes->Qvent += Qvent;
            floorRes->A += A;
            floorRes->V += V;

            std::string NameStr, QtotStr, QtransStr, QventStr, AStr, VStr, Qm2Str, Qm3Str;

            NameStr = room->text().toStdString();
            appendSpace(NameStr, NAME_LEN);
            numFormat(QtotStr, Qtot, NUM_LEN);
            numFormat(QtransStr, Qtrans, NUM_LEN);
            numFormat(QventStr, Qvent, NUM_LEN);
            numFormat(AStr, A, NUM_LEN);
            numFormat(VStr, V, NUM_LEN);
            numFormat(Qm2Str, Qtot / A, NUM_LEN);
            numFormat(Qm3Str, Qtot / V, NUM_LEN);

            char tmpStr[512] = {0};
            sprintf(tmpStr, "%s%s%s%s%s%s%s%s\n",
                    NameStr.c_str(), AStr.c_str(), VStr.c_str(), QtransStr.c_str(), QventStr.c_str(), QtotStr.c_str(), Qm2Str.c_str(), Qm3Str.c_str());
            output.append(tmpStr);
        }

        for (unsigned x = 0; x < NAME_LEN + 7 * NUM_LEN; x++) {
            output.push_back('-');
        }
        output.push_back('\n');

        std::string NameStr, QtotStr, QtransStr, QventStr, AStr, VStr, Qm2Str, Qm3Str;

        NameStr = floor->text().toStdString();
        appendSpace(NameStr, NAME_LEN);
        numFormat(QtotStr, floorRes->Qtot, NUM_LEN);
        numFormat(QtransStr, floorRes->Qtrans, NUM_LEN);
        numFormat(QventStr, floorRes->Qvent, NUM_LEN);
        numFormat(AStr, floorRes->A, NUM_LEN);
        numFormat(VStr, floorRes->V, NUM_LEN);
        numFormat(Qm2Str, floorRes->Qtot / floorRes->A, NUM_LEN);
        numFormat(Qm3Str, floorRes->Qtot / floorRes->V, NUM_LEN);

        char tmpStr[512] = {0};
        sprintf(tmpStr, "%s%s%s%s%s%s%s%s\n",
                NameStr.c_str(), AStr.c_str(), VStr.c_str(), QtransStr.c_str(), QventStr.c_str(), QtotStr.c_str(), Qm2Str.c_str(), Qm3Str.c_str());
        output.append(tmpStr);

        output.push_back('\n');
    }

    double Qvent = 0;
    double Qtrans = 0;
    double Qtot = 0;
    double A = 0;
    double V = 0;

    for (auto& it : floorVec) {
        Qtot += it.Qtot;
        Qtrans += it.Qtrans;
        Qvent += it.Qvent;
        A += it.A;
        V += it.V;
    }

    std::string NameStr, QtotStr, QtransStr, QventStr, AStr, VStr, Qm2Str, Qm3Str;

    NameStr = "TOTAL";
    appendSpace(NameStr, NAME_LEN);
    numFormat(QtotStr, Qtot, NUM_LEN);
    numFormat(QtransStr, Qtrans, NUM_LEN);
    numFormat(QventStr, Qvent, NUM_LEN);
    numFormat(AStr, A, NUM_LEN);
    numFormat(VStr, V, NUM_LEN);
    numFormat(Qm2Str, Qtot / A, NUM_LEN);
    numFormat(Qm3Str, Qtot / V, NUM_LEN);

    for (unsigned x = 0; x < NAME_LEN + 7 * NUM_LEN; x++) {
        output.push_back('-');
    }
    output.push_back('\n');

    char tmpStr[512] = {0};
    sprintf(tmpStr, "%s%s%s%s%s%s%s%s\n",
            NameStr.c_str(), AStr.c_str(), VStr.c_str(), QtransStr.c_str(), QventStr.c_str(), QtotStr.c_str(), Qm2Str.c_str(), Qm3Str.c_str());
    output.append(tmpStr);

    for (unsigned x = 0; x < NAME_LEN + 7 * NUM_LEN; x++) {
        output.push_back('-');
    }
    output.push_back('\n');

    area->setText(output.c_str());
    //	qDebug() << output.c_str();
}
