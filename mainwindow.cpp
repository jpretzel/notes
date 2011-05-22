// checksum 0xa193 version 0x30001
/*
  This file was generated by the Mobile Qt Application wizard of Qt Creator.
  MainWindow is a convenience class containing mobile device specific code
  such as screen orientation handling.
  It is recommended not to modify this file, since newer versions of Qt Creator
  may offer an updated version of it.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QMouseEvent>
#include <QPair>
#include <QPixmap>
#include <QMessageBox>
#include <QString>
#include <QList>


#include <QPainter>
#include <QScrollBar>

#define NOTES_PER_PAGE 4
#define NOTES_PER_ROW  2
#define DYNAMIC_HEIGHT 289

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if defined(Q_WS_MAC)
    _loadDir = "/Users/sebastian/Desktop/notes/";
    // startI = 2 um . und .. auszuschlieﬂen
    _startI = 2;
    _dir = "/";
#else
    // überprüfen, ob eine speicherkarte im Gerät ist
    if (QDir("e:\\").exists()) {
        _loadDir = "e:\\Development\\notes\\";
    } else {
        _loadDir = "c:\\Development\\notes\\";
    }
    _startI = 0;
    _dir = "\\";
#endif

    _rowCounter     = 0;
    _columnCounter  = 0;
    _pageCounter    = 1;
    _itemCounter    = 0;
    _currentPage    = 0;
    _p1             = QPoint(-1, -1);
    _p2             = QPoint(-1, -1);

    _gridLayout = (QGridLayout*)ui->noteWidget->layout();

    ui->menuWidget->setVisible(false);
    ui->menuWidget->setEnabled(false);

    // to recieve QMouseEvents through the scrollArea and its viewport
    ui->scrollArea->viewport()->setMouseTracking(true);

    loadNotes();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // _p1 takes _p2's old value
    if (_p2.y() != -1){
        _p1 = _p2;
    }

    _p2 = event->pos();

    // if both points got a value manipultae the position of the scrollBar
    if(_p1.y() != -1){
        int newPosition = ui->scrollArea->verticalScrollBar()->value() + (_p1.y() - _p2.y());
        ui->scrollArea->verticalScrollBar()->setValue(newPosition);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    // ivalidate points for new scrolling
    _p1 = QPoint(-1, -1);
    _p2 = QPoint(-1, -1);
}

void MainWindow::on_addButton_clicked()
{
    qDebug() << "[QUICK ADD NEW NOTE]";

    NoteButton * nb = new NoteButton();
    addNoteToGrid(nb);
}

void MainWindow::addNoteToGrid(NoteButton *b){
    qDebug() << "[ADDING NOTE TO GRID]" << b;

    _noteButtonList.append(b);

    qDebug() << "[connecting NoteButton]" << b;
    connect(b, SIGNAL(updateMe(NoteButton *)), this, SLOT(updateNoteButtonIcon(NoteButton *)));
    connect(b, SIGNAL(deleteMe(NoteButton*)), this, SLOT(deleteNoteButton(NoteButton*)));

    int row     = 0;
    int column  = 0;
    for(int i = 0; i < _noteButtonList.size(); i++){
        if(column > 1){
            row++;
            column = 0;
        }
        _gridLayout->addWidget(_noteButtonList.at(i), row, column++, Qt::AlignTop);
        _noteButtonGroup.addButton(_noteButtonList.at(i), i);
    }

    updateMinimumHeight();
    update();
}

void MainWindow::updateMinimumHeight(){
    qDebug() << "[UPDATING MINIMUM HEIGHT] " << _noteButtonList.size();

    if(_noteButtonList.size() % 2 == 0){
        ui->scrollAreaWidget->setMinimumHeight(DYNAMIC_HEIGHT * (_noteButtonList.size()/2));
        ui->noteWidget->setMinimumHeight(DYNAMIC_HEIGHT * (_noteButtonList.size()/2));
    }else{
        ui->scrollAreaWidget->setMinimumHeight(DYNAMIC_HEIGHT * (_noteButtonList.size()/2) + DYNAMIC_HEIGHT);
        ui->noteWidget->setMinimumHeight(DYNAMIC_HEIGHT * (_noteButtonList.size()/2) + DYNAMIC_HEIGHT);
    }
}

void MainWindow::clearPage(){
    // Delete
    /*while(_noteList.size() > 0){
        delete(_noteList.takeFirst());
    }*/
}

void MainWindow::updateNoteButtonIcon(NoteButton * b){
    qDebug() << "[SLOT: updating NoteButton]" << b;
    b->updateIcon();
}

void MainWindow::on_menuButton_clicked()
{
    //ui->addButton->setVisible(false);

    if(_noteButtonGroup.checkedId() == -1)
    {
        ui->sendButton_menu->setDisabled(true);
    } else {
        ui->sendButton_menu->setEnabled(true);
    }

    ui->menuWidget->setVisible(true);
    ui->menuWidget->setEnabled(true);
}

void MainWindow::on_menuCloseButton_clicked()
{
    //ui->addButton->setVisible(true);
    ui->menuWidget->setVisible(false);
    ui->menuWidget->setEnabled(false);
}

void MainWindow::paintEvent(QPaintEvent *){
    /*for(int i=0; i < _noteButtonList.size(); i++){
        _noteButtonList.at(i)->updateIcon();
    }*/
}

void MainWindow::on_quitButton_menu_clicked()
{
    exit(0);
}

void MainWindow::updateNoteIcons(){
}

void MainWindow::loadNotes()
{
    qDebug() << "[INFO] loading notes";
    if (!QDir(_loadDir).exists())
    {
        QDir().mkdir(_loadDir);
    }

    QStringList dirEntrys = QDir(_loadDir).entryList();
    QString tempDir;
    QPixmap pixmap(360, 640);

    for(int i = _startI; i < dirEntrys.size(); i++)
    {
        tempDir = dirEntrys.at(i);

        // die erste Seite der Note holen
        // sollte diese nicht existieren weil es sich um eine leere Seite handelt
        // muss eine leere Pixmap erstellt und ¸bergeben werden
        QFile firstPage(_loadDir + tempDir + _dir + tempDir + "_0.png");
        if (firstPage.exists())
        {
            pixmap.fill(Qt::white);
            QPainter painter(&pixmap);
            QPixmap temp = QPixmap(_loadDir + tempDir + _dir + tempDir + "_0.png");
            painter.drawPixmap(QPoint(0, 0), temp);
        } else {
            pixmap.fill(Qt::transparent);
        }
        addNoteToGrid(new NoteButton(pixmap, dirEntrys.at(i)));
    }
}
void MainWindow::on_sendButton_menu_clicked()
{
    _noteButtonList.at(_noteButtonGroup.checkedId())->getNotePainterWidget()->sendNote();

    on_menuCloseButton_clicked();
}

void MainWindow::showExpanded()
{
#ifdef Q_WS_MAC
    show();
#else
    showFullScreen();
#endif
}

void MainWindow::on_helpButton_menu_clicked()
{
}

void MainWindow::updateGrid(){

    // Clearing the gridLayout
    QLayoutItem *child;
     while ((child = _gridLayout->takeAt(0)) != 0) {
         _gridLayout->removeItem(_gridLayout->takeAt(0));
         delete child;
     }

    // rebuild
     int row     = 0;
     int column  = 0;
     for(int i = 0; i < _noteButtonList.size(); i++){
         if(column > 1){
             row++;
             column = 0;
         }
         _gridLayout->addWidget(_noteButtonList.at(i), row, column++, Qt::AlignTop);
         _noteButtonGroup.addButton(_noteButtonList.at(i), i);
     }
    updateMinimumHeight();
}

void MainWindow::deleteNoteButton(NoteButton * b){
    qDebug() << "[DELETE SIGNAL]" << b;
    QString deleteFolder = _loadDir + b->getFileName();
    QStringList deleteFiles = QDir(deleteFolder).entryList();
    for(int i = _startI; i < deleteFiles.size(); i++)
    {
        QDir().remove(deleteFolder + _dir + deleteFiles.at(i));
    }
    QDir().rmdir(deleteFolder);
    _noteButtonList.removeOne(b);
    _noteButtonGroup.removeButton(b);
    updateGrid();
    b->deleteLater();
}

void MainWindow::on_delButton_menu_clicked()
{
    qDebug() << "[DELETE]" << _noteButtonGroup.checkedButton();

    QString deleteFolder = _loadDir + _noteButtonList.at(_noteButtonGroup.checkedId())->getFileName();
    QStringList deleteFiles = QDir(deleteFolder).entryList();
    for(int i = _startI; i < deleteFiles.size(); i++)
    {
        QDir().remove(deleteFolder + _dir + deleteFiles.at(i));
    }
    QDir().rmdir(deleteFolder);

    _noteButtonList.removeAt(_noteButtonGroup.checkedId());
    _noteButtonGroup.checkedButton()->deleteLater();
    updateGrid();
    on_menuCloseButton_clicked();
}

void MainWindow::on_addButton_menu_clicked()
{
    on_addButton_clicked();
}
