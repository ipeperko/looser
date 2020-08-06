#include "MainWindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDockWidget>
#include <QMenuBar>
#include <QSettings>
#include <sstream>

#define ss_last_opened_dir "last_opened_dir"
#define ss_mainwindow_pos "mainwindow_pos"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;
    this->setCentralWidget(new QWidget);
    this->centralWidget()->setLayout(layout);

    buildingWidget = new BuildingWidget;
    coeffWidget = new CoeffWidget;
    roomWidget = new RoomWidget(this, coeffWidget, buildingWidget);
    roomTree = new RoomTree(nullptr, coeffWidget, roomWidget);
    overviewWidget = new OverviewWidget(nullptr, roomTree, buildingWidget);

    tabs = new QTabWidget;
    tabs->addTab(buildingWidget, tr("Building"));
    tabs->addTab(coeffWidget, tr("Materials"));
    tabs->addTab(roomWidget, tr("Room"));
    tabs->addTab(overviewWidget, tr("Overview"));
    tabs->setCurrentIndex(2);

    QDockWidget* dock = new QDockWidget;
    dock->setFloating(true);
    dock->setWidget(roomTree);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    layout->addWidget(tabs);

    // Menu File
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QAction* newAct = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);

    QAction* openAct = new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    QAction* saveAct = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);

    QAction* saveAsAct = new QAction(QIcon::fromTheme("document-save-as"), tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    connect(coeffWidget, &CoeffWidget::dataChanged, roomTree, (void (RoomTree::*)())(&RoomTree::selectionChanged));

    roomTree->setGeometry(0, 0, 200, roomTree->height());

    QSettings settings;
    setGeometry(settings.value(ss_mainwindow_pos, QRect(0, 0, 1000, 600)).toRect());
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue(ss_mainwindow_pos, geometry());
}

void MainWindow::openProject(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), "Cannot open file " + fileName + " for writing");
        return;
    }

    QTextStream in(&file);
    QString data;
    while (!in.atEnd()) {
        data += in.readLine();
    }

    try {
        dbm::xml::node xml = dbm::xml::node::parse(data.toStdString());
        //qDebug() << xml.toString("", 4).c_str();

        auto building = xml.find("building");
        looser_assert(building != xml.end(), "xml not valid (tag 'buidling' not found)");
        auto kvalues = xml.find("kvalues");
        looser_assert(kvalues != xml.end(), "xml not valid (tag 'kvalues' not found)");
        auto tree = xml.find("tree");
        looser_assert(tree != xml.end(), "xml not valid (tag 'tree' not found)");

        buildingWidget->fromXML(*building);
        coeffWidget->fromXML(*kvalues);
        roomTree->fromXML(*tree);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Error"), e.what());
        return;
    }
}

void MainWindow::newFile()
{
    QMessageBox::critical(this, tr("Error"), "TODO: new file ");
}

void MainWindow::saveProject(const QString& fileName)
{
    //	qDebug() << "Saving project";

    dbm::xml::node xml("xml");
    auto& bldg = xml.add("building");
    auto& kvalues = xml.add("kvalues");
    auto& tree = xml.add("tree");

    buildingWidget->toXML(bldg);
    coeffWidget->toXML(kvalues);
    roomTree->toXML(tree);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), "Cannot open file " + fileName + " for writing");
        return;
    }

    QTextStream out(&file);
    out << xml.to_string("", 4).c_str();
}

void MainWindow::save()
{
    QSettings settings;

    if (currentFileName.size()) {
        try {
            saveProject(currentFileName);
            settings.setValue(ss_last_opened_dir, currentFileName);
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, tr("Error"), e.what());
        }
    }
    else {
        saveAs();
    }
}

void MainWindow::saveAs()
{
    QSettings settings;
    QString dir = settings.value(ss_last_opened_dir).toString();
    currentFileName = QFileDialog::getSaveFileName(this, "Open simmulation file", dir);

    if (currentFileName.size()) {
        save();
    }
    else {
        QMessageBox::critical(this, tr("Error"), "Cannot save file");
    }
}

void MainWindow::open()
{
    QSettings settings;
    QString dir = settings.value(ss_last_opened_dir).toString();
    currentFileName = QFileDialog::getOpenFileName(this, "Open simmulation file", dir);
    qDebug() << "Dialog: " << currentFileName;

    if (currentFileName.size()) {
        try {
            openProject(currentFileName);
            settings.setValue(ss_last_opened_dir, currentFileName);
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, tr("Error"), e.what());
        }
    }
}
