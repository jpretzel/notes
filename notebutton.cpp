#include "notebutton.h"
#include <QPainter>
#include <QMouseEvent>

/**
 * @brief Constructor of NoteButton for adding a new note.
 *
 * This Constructor is used to if a new note is added to the application
 * and the filesystem.
 */
NoteButton::NoteButton()
{
    setIconSize(QSize(111, 197));
    setStyleSheet("NoteButton:checked{background-image: url(:/backgrounds/images/highlightframe.png);}");
    setFlat(true);
    setCheckable(true);

    // we need a NotePainterWidget here because we open the note
    // right away
    painterWidget = new NotePainterWidget();
    doConnect();
    painterWidget->showExpanded();
}

/**
 * @brief Constructor of NoteButton for adding existing notes.
 *
 * This Constructor is used if notes that exists in the
 * filesystem need to be loaded to the application.
 *
 * @param pixmap
 *      The icon (first page of the note) that is used for the button.
 *
 * @param fileName
 *      The name of the note in the filesystem.
 */
NoteButton::NoteButton(QPixmap pixmap, QString fileName)
{
    // so we can check if it's loaded or not
    painterWidget = NULL;
    setIconSize(QSize(111, 197));
    setIcon(pixmap);
    this->_fileName = fileName;
    setStyleSheet("NoteButton:checked{background-image: url(:/backgrounds/images/highlightframe.png);}");
    setFlat(true);
    setCheckable(true);
}

/**
 * @brief Updates the icon of the NoteButton.
 *
 * This is used to update the icon if the note changed.
 */
void NoteButton::updateIcon()
{
    if (painterWidget != NULL)
    {
        QPixmap bg(360, 640);
        bg.fill(Qt::white);
        QPainter painter(&bg);
        painter.drawPixmap(QPoint(0, 0), painterWidget->getPageOne());
        setIcon(bg);
    }
}

/**
 * @brief Saves the position, where the touchscreen was pressed.
 *
 * To properly handle scrolling and pressing of the buttons at the
 * same time, we need to know where the button was pressed. The position
 * is saved to _p1. Further handling of the described is done in
 * mouseReleaseEvent(QMouseEvent *event).
 *
 * @param event
 *      The QMouseEvent that is handled.
 */
void NoteButton::mousePressEvent(QMouseEvent *event)
{
    _p1 = event->pos();
}

/**
 * @brief Decides whether to open the note or not.
 *
 * The position where the touchscreen is realeased is saved to _p2.
 * If the release took place over 20px away from the press event
 * or the NoteButton isn't checked the note is not beeing opened.
 *
 * The button is always set to be checked and the signal releaseEvent
 * is emitted which will be catched by MainWindow::resetLastPresspoint.
 * This is done, because MainWindow won't notice the mouseReleaseEvent
 * by itself, but it needs to know to handle scrolling properly.
 *
 * @param event
 *      The QMouseEvent that is handled.
 */
void NoteButton::mouseReleaseEvent(QMouseEvent *event)
{
    _p2 = event->pos();
    if (abs(_p1.x() - _p2.x()) < 20 && abs(_p1.y() - _p2.y()) < 20 && isChecked())
        openNote();

    setChecked(true);

    // tell MainWindow to reset _lastPresspoint
    // because it won't notice the mouseReleaseEvent of
    // the button itself
    emit(releaseEvent());
}

/**
 * @brief Opens the note of the button.
 *
 * If the note presented by a NotePainterWidget is loaded
 * it is opened, if not a new NotePainterWidget is created
 * and its notes will be loaded.
 */
void NoteButton::openNote()
{
    if (painterWidget == NULL)
    {
        painterWidget = new NotePainterWidget();
        if (!_fileName.isEmpty())
        {
            painterWidget->setFileName(this->_fileName);
            painterWidget->loadNotes();
        }
    }

    doConnect();
    painterWidget->showExpanded();
}

/**
 * @brief Returns the file name of the note.
 *
 * @return
 *      The file name of the note in the filesystem.
 */
QString NoteButton::getFileName()
{
    return _fileName;
}

/**
 * @brief Handles the connecting of the slots to the signals of the buttons NotePainterWidget.
 */
void NoteButton::doConnect()
{
    connect(painterWidget, SIGNAL(closeSignal()), this, SLOT(NotePainterWidgetClosed()));
    connect(painterWidget, SIGNAL(deleteSignal()), this, SLOT(NotePainterWidgetDeleteTriggered()));
}

/**
 * @brief Handles the disconnecting of the slots if the buttons NotePainterWidget is deleted for example.
 */
void NoteButton::doDisconnect()
{
    disconnect(painterWidget, SIGNAL(closeSignal()), this, SLOT(NotePainterWidgetClosed()));
    disconnect(painterWidget, SIGNAL(deleteSignal()), this, SLOT(NotePainterWidgetDeleteTriggered()));
}

/**
 * @brief Slot to delete the buttons note.
 *
 * When the button for deleting the note was triggered in NotePainterWidget
 * NotePainterWidget is getting closed, all signals and slots will be disconnected
 * and the MainWindow will be toled to delte this NoteButton and the note in the
 * filesystem.
 */
void NoteButton::NotePainterWidgetDeleteTriggered()
{
    painterWidget->close();
    doDisconnect();

    // tell MainWindow to delete the button and
    // the images in the filesystem for this note
    emit(deleteMe(this));
}

/**
 * @brief Slot to close the buttons NotePainterWidget.
 *
 * When the NotePainterWidget is closed, this slot will catch its signal.
 * Slots then get disconnected, the file name of the note is saved, a signal
 * for MainWindow emitted to update the button and for performence reasons
 * the NotePainterWidget is getting deleted.
 */
void NoteButton::NotePainterWidgetClosed()
{
    doDisconnect();
    _fileName = painterWidget->getFilename();

    // tell MainWindow to update this button
    emit(updateMe(this));
    painterWidget->deleteLater();

    // so we know it's not set
    painterWidget = NULL;
}

/**
 * @brief Set the buttons NotePainterWidget.
 *
 * @param p
 *      The NotePainterWidget to be set.
 */
void NoteButton::setNotePainterWidget(NotePainterWidget * p)
{
    this->painterWidget = p;
}

/**
 * @brief Retunrs the buttons NotePainterWidget.
 *
 * If a NotePainterWidget is not set, a new one is created first.
 *
 * @return
 *      The buttons NotePainterWidget.
 */
NotePainterWidget * NoteButton::getNotePainterWidget()
{
    if (painterWidget == NULL)
    {
        painterWidget = new NotePainterWidget();
        if(!_fileName.isEmpty()){
            painterWidget->setFileName(this->_fileName);
        }
    }

    return painterWidget;
}
