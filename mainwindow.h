// checksum 0x9a77 version 0x30001
/*
  This file was generated by the Mobile Qt Application wizard of Qt Creator.
  MainWindow is a convenience class containing mobile device specific code
  such as screen orientation handling.
  It is recommended not to modify this file, since newer versions of Qt Creator
  may offer an updated version of it.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "notebutton.h"
#include "helpwidget.h"
#include <QGridLayout>
#include <QButtonGroup>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    void showExpanded();

    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    Ui::MainWindow *ui;
    int _rowCounter;
    int _columnCounter;
    int _pageCounter;
    int _itemCounter;
    int _currentPage;
    int _startI;
    QString _loadDir;
    QString _dir;
    int _lastPresspoint;
    int _lastScrollBarPos;
    void clearPage();
    QGridLayout * _gridLayout;
    NotePainterWidget *_notepainterwidget;
    QList<NoteButton *> _noteButtonList;
    QButtonGroup _noteButtonGroup;
    void addNoteToGrid(NoteButton *b);
    void loadNotes();
    void sendNoteViaEmail();
    void updateGrid();
    void updateMinimumHeight();

public slots:
    void paintEvent(QPaintEvent *);
    void updateNoteIcons();
    void deleteNoteButton(NoteButton *);
    void resetLastPresspoint();

private slots:
    void on_quitButton_menu_clicked();
    void on_menuButton_clicked();
    void on_addButton_clicked();
    void on_menuCloseButton_clicked();
    void on_sendButton_menu_clicked();
    void on_helpButton_menu_clicked();
    void updateNoteButtonIcon(NoteButton *);
    void on_delButton_menu_clicked();
    void on_addButton_menu_clicked();
};

#endif // MAINWINDOW_H
