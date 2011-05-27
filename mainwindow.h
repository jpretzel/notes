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

/**
 * MainWindow is the starting window of the application.
 * Core functionality here is to access adding, deleting and sending of
 * a note. It is also possible to open a help dialog (HelpWidget)
 * and close the application from here.
 *
 * @author Jan Pretzel (jan.pretzel@deepsource.de)
 * @author Sebastian Ullrich (sebastian.ullrich@deepsource.de)
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    // Constructors:
    explicit MainWindow(QWidget *parent = 0);

    // Destructor:
    virtual ~MainWindow();

    // Methods:
    void showExpanded();

    // Methods to hanlde scrolling:
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    Ui::MainWindow *ui;

    // Attributes
    int _startI;
    QString _loadDir;
    QString _dir;
    int _lastPresspoint;
    int _lastScrollBarPos;
    QGridLayout * _gridLayout;
    QList<NoteButton *> _noteButtonList;
    QButtonGroup _noteButtonGroup;

    // Methods
    void addNoteToGrid(NoteButton *b);
    void loadNotes();
    void updateGrid();
    void updateMinimumHeight();
    void doConnect(NoteButton *);

public slots:
    void deleteNoteButton(NoteButton *);

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
    void resetLastPresspoint();
};

#endif // MAINWINDOW_H
