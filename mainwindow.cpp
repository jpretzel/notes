#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QDir>
#include <QMouseEvent>
#include <QPixmap>
#include <QString>
#include <QList>

#include <QPainter>
#include <QScrollBar>

#include "helpwidget.h"

#define NOTES_PER_PAGE 4
#define NOTES_PER_ROW  2
#define DYNAMIC_HEIGHT 260

/**
 * @brief Constructor of MainWindow.
 *
 * Sets initial values of attributes and loads the notes saved
 * in the filesystem by calling loadNotes().
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if defined(Q_WS_MAC)
    _loadDir = "/Users/jan/Desktop/notes/";
    // handle the "." and ".." folders
    _startI = 2;
    _dir = "/";
#else
    // check for flash media
    if (QDir("e:\\").exists()) {
        _loadDir = "e:\\Development\\notes\\";
    } else {
        _loadDir = "c:\\Development\\notes\\";
    }
    _startI = 0;
    _dir = "\\";
#endif

    _lastPresspoint = -1;

    _gridLayout = (QGridLayout*)ui->noteWidget->layout();
    _gridLayout->setAlignment(Qt::AlignTop);
    _gridLayout->setVerticalSpacing(46);

    ui->menuWidget->setVisible(false);
    ui->menuWidget->setEnabled(false);

    // to recieve QMouseEvents through the scrollArea and its viewport
    ui->scrollArea->viewport()->setMouseTracking(true);

    loadNotes();
}

/**
 * @brief Destructor of MainWindow.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Handles the selfimplemented kinetic scrolling for scrollArea.
 *
 * Because we have NoteButtons on top of the scrollArea we need to fake
 * a mousePressEvent. To do so we use _lastPresspoint where we save the first
 * captured position while moving the "mouse". If we realease the "mouse" we
 * invalidate _lastPresspoint by setting it to "-1".
 *
 * @param event
 *      The event that will be handled.
 */
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // fake a mousePressEvent because the real one
    // could be caught by a button while moveEvents go through
    if (_lastPresspoint == -1)
    {
        _lastPresspoint = event->globalPos().y();
        _lastScrollBarPos = ui->scrollArea->verticalScrollBar()->value();
    } else {
        int scrollPos = _lastScrollBarPos - (event->globalPos().y() - _lastPresspoint);
        const int min = ui->scrollArea->verticalScrollBar()->minimum();
        const int max = ui->scrollArea->verticalScrollBar()->maximum();

        // consider the limits
        if (scrollPos > max)
        {
            scrollPos = max;
        } else if (scrollPos < min) {
            scrollPos = min;
        }

        ui->scrollArea->verticalScrollBar()->setValue(scrollPos);
    }
}

/**
 * @brief Invalidates _lastPresspoint.
 *
 * _lastPressPoint needs to be invalidated on a mouseReleaseEvent,
 * so we can fake a mousePressEvent while we have a mouseMoveEvent.
 * But we only want to do this once while moing the mouse, so we
 * invalidate _lastPresspoint at the end of such an action.
 */
void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    resetLastPresspoint();
}

/**
 * @brief Invalidates _lastPresspoint.
 *
 * This slots cathes the mouseReleaseEvent of a NoteButton.
 * It is needed because MainWindow won't know itself when this happens,
 * but still needs to invalidate _lastPresspoint to handle scrolling
 * properly.
 */
void MainWindow::resetLastPresspoint()
{
    _lastPresspoint = -1;
}

/**
 * @brief Connect the given NoteButton to MainWindow.
 *
 * @param b
 *      The NoteButton that will be connected to MainWindow.
 */
void MainWindow::doConnect(NoteButton *b)
{
    connect(b, SIGNAL(updateMe(NoteButton *)), this, SLOT(updateNoteButtonIcon(NoteButton *)));
    connect(b, SIGNAL(deleteMe(NoteButton *)), this, SLOT(deleteNoteButton(NoteButton*)));
    connect(b, SIGNAL(releaseEvent()), this, SLOT(resetLastPresspoint()));
}

/**
 * @brief Adds a NoteButton to the gridLayout.
 *
 * The NoteButton will be placed to the next place in
 * the gridLayout and will be added to _noteButtonList and
 * _noteButtdonGroup if it is not in there yet. At the end
 * updateMinimumHeight() is called to adapt the size of the
 * scrollArea.
 *
 * @param b
 *      The NoteButton that will be added to the gridLayout.
 */
void MainWindow::addNoteToGrid(NoteButton *b)
{
    int row = _gridLayout->count() / 2;
    int column = 0;
    if (_gridLayout->count() % 2 == 1)
    {
        column = 1;
    }

    _gridLayout->addWidget(b, row, column++);

    if (!_noteButtonList.contains(b))
    {
        _noteButtonList.append(b);
        _noteButtonGroup.addButton(b, _noteButtonList.size() - 1);
    }

    updateMinimumHeight();
    update();
}

/**
 * @brief Updates the height of the scrollArea depending on the number of NoteButtons.
 */
void MainWindow::updateMinimumHeight()
{
    if(_noteButtonList.size() % 2 == 0)
    {
        ui->scrollAreaWidget->setMinimumHeight(DYNAMIC_HEIGHT * (_noteButtonList.size()/2));
        ui->noteWidget->setMinimumHeight(DYNAMIC_HEIGHT * (_noteButtonList.size()/2));
    } else {
        ui->scrollAreaWidget->setMinimumHeight(DYNAMIC_HEIGHT * (_noteButtonList.size()/2) + DYNAMIC_HEIGHT);
        ui->noteWidget->setMinimumHeight(DYNAMIC_HEIGHT * (_noteButtonList.size()/2) + DYNAMIC_HEIGHT);
    }
}

/**
 * @brief Updates the icon of the give NoteButton by calling NoteButton::updateIcon().
 *
 * @param b
 *      The NoteButton that will be updated.
 */
void MainWindow::updateNoteButtonIcon(NoteButton * b)
{
    b->updateIcon();
}

/**
 * @brief Loads the notes saved in the filesystem.
 *
 * For every note in the filesystem a NoteButton is created
 * and added to the gridLayout by calling addNoteToGrid(NoteButton *b).
 * Also every button will be connected to MainWindow (doConnect(NoteButton *b)).
 */
void MainWindow::loadNotes()
{
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

        // get first page of the note
        // if there is no first page create an empty pixmap
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

        NoteButton * button = new NoteButton(pixmap, dirEntrys.at(i));
        addNoteToGrid(button);
        doConnect(button);
    }
}

/**
 * @brief Handles how the Application will be shown on different Operating Systems.
 *
 * At the moment only Mac OS X gets a special treatment. Just add another OS if needed.
 */
void MainWindow::showExpanded()
{
#ifdef Q_WS_MAC
    show();
#else
    showFullScreen();
#endif
}

/**
 * @brief Updates the gridLayout to show the NoteButtons properly.
 *
 * First the gridLayout is cleared, then the NoteButtons of
 * _noteButtonList are added by calling addNoteToGrid(NoteButton *b).
 * After rebuilding the gridLayout the height of the scrollArea is
 * getting recalculated by calling updateMinimumHeight().
 */
void MainWindow::updateGrid()
{
    // Clearing the gridLayout
    QLayoutItem *child;
    while ((child = _gridLayout->takeAt(0)) != 0)
    {
        _gridLayout->removeItem(_gridLayout->takeAt(0));
        delete child;
    }

    int d = _noteButtonList.size();
    for(int i = 0; i < d; i++)
    {
        addNoteToGrid(_noteButtonList.at(i));
    }

    updateMinimumHeight();
}

/**
 * @brief Deletes the NoteButton and the note in the filesystem.
 *
 * For deleting the note from the filesystem, all images are deleted before
 * the folder can be deleted. Also _noteButtonGroup needs to be rebuild,
 * because the indices must be the same as in _noteButtonList.
 *
 * @param b
 *      The NoteButton and the attatched note that will be deleted.
 */
void MainWindow::deleteNoteButton(NoteButton * b)
{
    QString deleteFolder = _loadDir + b->getFileName();
    QStringList deleteFiles = QDir(deleteFolder).entryList();

    // delete files (only empty folders can be deleted)
    for(int i = _startI; i < deleteFiles.size(); i++)
    {
        QDir().remove(deleteFolder + _dir + deleteFiles.at(i));
    }

    // after all files are deleted the folder can be removed
    QDir().rmdir(deleteFolder);

    // remove all buttons from the group, because we need to build
    // it new to keep indices the same as in _noteButtonList
    for(int i = 0; i < _noteButtonList.size(); i++)
    {
        _noteButtonGroup.removeButton(_noteButtonList.at(i));
    }

    _noteButtonList.removeOne(b);

    // build the new group
    for(int i = 0; i < _noteButtonList.size(); i++)
    {
        _noteButtonGroup.addButton(_noteButtonList.at(i), i);
    }

    updateGrid();
    b->deleteLater();
}

/* =========================================================================
 * Menu functionality
 * ========================================================================= */

/**
 * @brief Openes the menu.
 */
void MainWindow::on_menuButton_clicked()
{
    // only show send and delete button if a note is selected
    if (_noteButtonGroup.checkedId() == -1)
    {
        ui->sendButton_menu->setDisabled(true);
        ui->delButton_menu->setDisabled(true);
    } else {
        ui->sendButton_menu->setEnabled(true);
        ui->delButton_menu->setEnabled(true);
    }

    ui->menuWidget->setVisible(true);
    ui->menuWidget->setEnabled(true);
}

/**
 * @brief Closes the menu.
 */
void MainWindow::on_menuCloseButton_clicked()
{
    ui->menuWidget->setVisible(false);
    ui->menuWidget->setEnabled(false);
}

/**
 * @brief Closes the Application.
 */
void MainWindow::on_quitButton_menu_clicked()
{
    qApp->quit();
}

/**
 * @brief Initiates sending of a note.
 *
 * To send a note NotePainterWidget::sendNote() is called.
 */
void MainWindow::on_sendButton_menu_clicked()
{
    _noteButtonList.at(_noteButtonGroup.checkedId())->getNotePainterWidget()->sendNote();

    // set to null because it's not opened anyway
    _noteButtonList.at(_noteButtonGroup.checkedId())->setNotePainterWidget(NULL);

    on_menuCloseButton_clicked();
}

/**
 * @brief Shows a HelpWidget.
 */
void MainWindow::on_helpButton_menu_clicked()
{
    HelpWidget * hw = new HelpWidget();
    hw->showExpanded();
    on_menuCloseButton_clicked();
}

/**
 * @brief Initiates deleting of a note by calling deleteNoteButton.
 */
void MainWindow::on_delButton_menu_clicked()
{
    deleteNoteButton(_noteButtonList.at(_noteButtonGroup.checkedId()));
    on_menuCloseButton_clicked();
}

/**
 * @brief Initiates adding of a new note.
 *
 * New NoteButton is created, added to the gridLayout and
 * connected to MainWindow.
 */
void MainWindow::on_addButton_clicked()
{
    NoteButton * nb = new NoteButton();
    addNoteToGrid(nb);

    // connect button with MainWindow
    doConnect(nb);
}

/**
 * @brief Initiates adding of a new note by calling on_addButton_clicked().
 */
void MainWindow::on_addButton_menu_clicked()
{
    on_addButton_clicked();

    // close menu so it won't show if we leave the note
    on_menuCloseButton_clicked();
}
