#include "notepainterwidget.h"
#include "ui_notepainterwidget.h"
#include <QPainter>
#include <QColor>
#include <QMouseEvent>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>

#include "sendnotedialog.h"

#if !defined(Q_WS_MAC)
#include <qmessageservice.h>
#include <QContactManager>
#include <QSystemInfo>
#include <QContact>
#include <QContactDetail>
#endif

#define DOGEARZONE_X 300
#define DOGEARZONE_Y 580
#define UPPER_DOGEARZONE_Y 60
#define MAX_PAGES 3
#define PEN_WIDTH 6
#define NIRVANA -PEN_WIDTH / 2

#if !defined(Q_WS_MAC)
QTM_USE_NAMESPACE
#endif

/**
 * @brief Constructor of NotePainterWidget.
 *
 * Sets initial values of attributes and initialises the
 * the painter used to draw.
 */
NotePainterWidget::NotePainterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotePainterWidget)
{
    ui->setupUi(this);

#if defined(Q_WS_MAC)
    _loadDir = "/Users/jan/Desktop/notes/";
    _dir = "/";

    // handle the "." and ".." folders
    _startI = 2;
#else
    // check for flash media
    if (QDir("e:\\").exists()) {
        _loadDir = "e:\\Development\\notes\\";
    } else {
        _loadDir = "c:\\Development\\notes\\";
    }
    _dir = "\\";
    _startI = 0;
#endif

    _currentPage            = 0;
    _drawMode               = true;
    _dogEarClicked          = false;
    _upperDogEarClicked     = false;
    _fileName               = "";
    _rubber                 = false;
    _paintWidgetDisabled    = false;

    ui->penButton->setDisabled(true);

    for (int i = 0; i <= MAX_PAGES; i++)
    {
        _modified[i] = false;
        _erase[i] = false;
    }

    // hide menus
    showBackgroundMenu(false);

    _pen.setWidth(PEN_WIDTH);
    _pen.setCapStyle(Qt::RoundCap);
    _pen.setJoinStyle(Qt::RoundJoin);
    _pen.setStyle(Qt::SolidLine);

    initDrawTools();
}

/**
 * @brief Destructor of NotePainterWidget.
 */
NotePainterWidget::~NotePainterWidget()
{
    delete ui;
}

/**
 * @brief Initialises everything that is needed to draw properly.
 *
 * Sets an empty QPixmap with transparent background to the current page
 * of the note if there wasn't one before.
 * Also sets the 2 points used to handle the drawing to the reset values.
 */
void NotePainterWidget::initDrawTools()
{
    if (_pixmap[_currentPage].isNull())
    {
        _pixmap[_currentPage] = QPixmap(360, 640);
        _pixmap[_currentPage].fill(Qt::transparent);
    }

    _p1 = QPoint(NIRVANA, NIRVANA);
    _p2 = QPoint(NIRVANA, NIRVANA);
}

/**
 * @brief Draws the QPixmap of the current page to the Screen.
 */
void NotePainterWidget::paintEvent(QPaintEvent *)
{
    // workaround to display backgrounds on custom widgets
    QStyleOption styleOption;
    styleOption.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);

    // draw pixmap
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(_pen);
    painter.drawPixmap(QPoint(0, 0), _pixmap[_currentPage]);
}

/**
 * @brief Decides what mode to activate.
 *
 * Possible modes are _drawMode, _dogEarClicked or _upperDogEarClicked.
 *
 * @param event
 *      The event that will be handled.
 */
void NotePainterWidget::mousePressEvent(QMouseEvent * event)
{
    if (event->pos().x() > DOGEARZONE_X && event->pos().y() > DOGEARZONE_Y)
    {
        _dogEarClicked      = true;
        _drawMode = false;
    } else if (event->pos().y() < UPPER_DOGEARZONE_Y && event->pos().x() > DOGEARZONE_X) {
        _upperDogEarClicked = true;
        _drawMode = false;
    } else {
        _drawMode           = true;

        // set first position and reset 2nd point
        _p1 = event->pos();
        _p2 = QPoint(NIRVANA, NIRVANA);
    }
}

/**
 * @brief Handles drawing of lines.
 *
 * If _drawMode is true we capture points while moving the "mouse" and
 * draw a lines between this points on a QPixmap, by calling paintToPixmap().
 *
 * @param event
 *      The event that will be handled.
 */
void NotePainterWidget::mouseMoveEvent(QMouseEvent * event)
{
    if (_drawMode)
    {
        // mouse is still pressed, so we have to draw lines
        if (_p2.x() != NIRVANA)
        {
            _p1 = _p2;
        }
        _p2 = event->pos();
        paintToPixmap();
    }
}

/**
 * @brief Detects if one of the dogears was clicked/draged or if we need to draw.
 *
 * If one of the dogears was clicked, either a page turn is done,
 * or the menu will be opned. If no dogear was clicked drawing will be
 * performed.
 *
 * @param event
 *      The event that will be handled.
 */
void NotePainterWidget::mouseReleaseEvent(QMouseEvent * event)
{
    // (lower) dogear clicked
    if (_dogEarClicked)
    {
        // check where the release event took place
        if (event->pos().y() > DOGEARZONE_Y)
        {
            // menu was activated
            showBackgroundMenu(true);

            // next page
        } else if (_currentPage < MAX_PAGES) {
            _currentPage++;
            initDrawTools();
        }
        _dogEarClicked  = false;

        // upper dogear clicked
    } else if (_upperDogEarClicked) {
        // page back
        if (event->pos().y() > UPPER_DOGEARZONE_Y)
        {
            if (_currentPage > 0)
            {
                _currentPage--;
                initDrawTools();
            }
        }
        _upperDogEarClicked = false;

        // nothing special clikced => draw
    } else {
        paintToPixmap();
    }

    // reset points and repaint
    _p1 = QPoint(NIRVANA, NIRVANA);
    _p2 = QPoint(NIRVANA, NIRVANA);
    repaint();
}

/**
 * @brief Draws the painting events onto a QPixmap.
 *
 * The painting events are drawn to a QPixmap. This QPixmap
 * is drawn to the actual widget in paintEvent(QPaintEvent *).
 */
void NotePainterWidget::paintToPixmap()
{
    // if menu is open, don't draw
    if (_paintWidgetDisabled)
        return;

    QPainter painter;
    painter.begin(&_pixmap[_currentPage]);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(_pen);

    // if erase mode is active, switch CompositionMode
    if(_rubber)
    {
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
    }

    // _p2 is not set draw a point, else we need to draw a line
    if (_p2.x() == NIRVANA)
        painter.drawPoint(_p1);
    else if (_p1.x() != NIRVANA && _p2.x() != NIRVANA) {
        painter.drawLine(_p1, _p2);
    }

    painter.end();

    // tell the saveImages() method to save the current page
    _modified[_currentPage] = true;
    update();
}

/**
 * @brief Loads all images of a note, so they can be shown while the note is open.
 *
 * Loads all images belonging to the concerned note, and stores them in _pixmap[].
 */
void NotePainterWidget::loadNotes()
{
    // do nothing when the note is not realy defined
    if (_fileName != "")
    {
        QString loadDir = _loadDir + _fileName + _dir;
        QStringList notePages = QDir(loadDir).entryList();
        if (notePages.size() > _startI)
        {
            for (int i = _startI; i < notePages.size(); i++)
            {
                // load the image as QPixmap and store it to the right page
                QPixmap pixmap(loadDir + notePages.at(i));
                _pixmap[notePages.at(i).mid(13, 1).toInt()] = pixmap;
            }
        }
    }
}

/**
 * @brief Returns the first page of the note.
 *
 * @return
 *      The first page of the note, beeing saved in _pixmap.
 */
QPixmap NotePainterWidget::getPageOne()
{
    if (_pixmap[0].isNull())
    {
        _pixmap[0] = QPixmap(360, 640);
        _pixmap[0].fill(Qt::transparent);
    }

    return _pixmap[0];
}

/**
 * @brief Returns the file name of the note.
 *
 * @return
 *      The file name of the note.
 */
QString NotePainterWidget::getFilename()
{
    return _fileName;
}

/**
 * @brief Saves the pages of the note as images in the filesystem.
 *
 * If there have been any changes on any page of the note or one or more
 * page(s) of the note have to be deleted this will be done here. If the
 * note is new, _fileName will be set. Also directorys will be created
 * and deleted here if needed.
 */
void NotePainterWidget::saveImages()
{
    // no changes => do nothing
    if (_modified[0] || _modified[1] || _modified[2] || _modified[3]
            || _erase[0] || _erase[1] || _erase[2] || _erase[3])
    {
        if (!QDir(_loadDir).exists())
        {
            QDir().mkdir(_loadDir);
        }

        // set fileName if it is a new note
        if (_fileName == "")
        {
            _fileName = QDateTime::currentDateTime().toString("yyMMddhhmmss");
        }

        // create sub_directory
        QString loadDir = _loadDir + _fileName + _dir;

        // if needed? mk_dir() returns false if not successfully
        if (!QDir(loadDir).exists())
        {
            QDir().mkdir(loadDir);
        }

        for(int page = 0; page <= MAX_PAGES; page++)
        {
            QFile file(loadDir + _fileName + "_" + QString::number(page) + ".png");

            if (_modified[page])
            {
                _pixmap[page].save(file.fileName());
            } else if (_erase[page] && file.exists()) {
                file.remove();
            }
        }

        // delete folder if note is empty
        QStringList checkForDel = QDir(loadDir).entryList();
#if defined(Q_WS_MAC)
        // handle the folders "." and ".."
        checkForDel.removeFirst();
        checkForDel.removeFirst();
#endif

        if (checkForDel.isEmpty())
        {
            QDir().rmdir(loadDir);
        }
    }
}

/**
 * @brief Sets the name of the note to handle loading, saving and so on.
 *
 * @param newFileName
 *      The new file name.
 */
void NotePainterWidget::setFileName(QString newFileName)
{
    _fileName = newFileName;
}

/**
 * @brief Sets the icon of the NotePainterWidget.
 *
 * A QPixmap is saved as the first page of the note, after that
 * createIcon() is called to create the icon and save it to _icon.
 *
 * @param pixmap
 *      The QPixmap that will be set as icon.
 */
void NotePainterWidget::setIcon(QPixmap pixmap)
{
    _pixmap[0] = pixmap;
    createIcon();
}

/**
 * @brief Creates the icon of the NotePainterWidget.
 *
 * Creates the icon of the NotePainterWidget, by filling a QPixmap
 * with a white background and drawing the first page of the note
 * on top of it. The QIcon will be saved in _icon.
 */
void NotePainterWidget::createIcon()
{
    QPixmap pixmap( 360, 640 );
    pixmap.fill(Qt::white);
    QPainter painter( &pixmap );

    painter.setPen(_pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(QPoint(0, 0), _pixmap[0]);

    this->_icon = pixmap;
}

/**
 * @brief Returns the icon of the NotePainterWidget.
 *
 * @return
 *      The QIcon of the NotePainterWidget.
 */
QIcon NotePainterWidget::getIcon()
{
    return this->_icon;
}

/**
 * @brief Sends the note using QMobility APIs.
 *
 * Opens a SendNoteDialog to get an email address. If the Dialog
 * is left in the accepted mode the note will be send to the entered address,
 * if not empty and passed the pre validation.
 * The Symbian intern email API will handle everything else.
 */
void NotePainterWidget::sendNote()
{
    QString emailAdress = SendNoteDialog::getEmailAddress(this);

    // OK Button was pressed and input field was not empty
    if (!emailAdress.isNull() && emailAdress != "")
    {
        // Validate Email
        QRegExp rx("^[A-Z0-9._%+-]+@[A-Z0-9.-]+.[A-Z]{2,4}$", Qt::CaseInsensitive);
        if (!rx.exactMatch(emailAdress))
        {
            QMessageBox::warning(this, "Email Adresse", emailAdress
                                 + QString::fromUtf8(" scheint keine gültige Email Adresse zu sein."));
        } else {
#if !defined(Q_WS_MAC)
            QMessageService * service = new QMessageService;
            QMessageAddress sendTo(QMessageAddress::Email, emailAdress);

            QMessage msg;
            msg.setType(QMessage::Email);
            msg.setBody("Hi there!\nPlease have a look at the attachements.\n\n---\n(notes) made by Jan Pretzel & Sebastian Ullrich");

            // Setting the stored EmailAdress as sender.
            msg.setParentAccountId(QMessageAccount::defaultAccount(QMessage::Email));

            msg.setTo(sendTo);
            msg.setSubject("notes for you :)");

            // load pages of the note and attach them to the email.
            QStringList notePages = QDir(_loadDir + _fileName).entryList();
            QStringList absPath;

            for(int i = 0; i < notePages.size(); i++)
            {
                absPath.append(_loadDir + _fileName + _dir + notePages.at(i));
            }

            msg.appendAttachments(absPath);

            if(service->send(msg))
            {
                // do nothing atm. Symbian email should handle error messages.
            }
#endif
        }
    }
}

/**
 * @brief Handles how the Application will be shown on different Operating Systems.
 *
 * At the moment only Mac OS X gets a special treatment. Just add another OS if needed.
 */
void NotePainterWidget::showExpanded()
{
#ifdef Q_WS_MAC
    show();
#else
    showFullScreen();
#endif
}

/* =========================================================================
 * BackgroundMenu functionality
 * ========================================================================= */

/**
 * @brief Shows or hides the BackgroundMenu.
 *
 * @param show
 *      Defines whether the BackgroundMenu should be shown or hidden.
 */
void NotePainterWidget::showBackgroundMenu(bool show)
{
    if (show)
    {
        ui->BackgroundMenu->setVisible(true);
        ui->BackgroundMenu->setEnabled(true);
        _paintWidgetDisabled = true;
    } else {
        ui->miniMenuWidget->setVisible(false);
        ui->BackgroundMenu->setVisible(false);
        ui->BackgroundMenu->setEnabled(false);
        _paintWidgetDisabled = false;
    }
}

/**
 * @brief Toggles the miniMenu of the BackgroundMenu.
 */
void NotePainterWidget::on_menuButton_clicked()
{
    if (ui->miniMenuWidget->isHidden())
    {
        ui->menuButton->setIcon(QIcon(":/menu/images/closeMenu"));
        ui->miniMenuWidget->setVisible(true);
    } else {
        ui->menuButton->setIcon(QIcon(":/menu/images/openMenu"));
        ui->miniMenuWidget->setVisible(false);
    }
}

/**
 * @brief Clears the current page of the NotePainterWidget.
 *
 * After clearing the current page the page will be marked as not modified
 * but to be deleted, so it can be handled correctly in saveImages().
 * To do so _modified and _erase for this page are set.
 *
 */
void NotePainterWidget::on_clearButton_clicked()
{
    _pixmap[_currentPage].fill(Qt::transparent);

    // current page is no longer modified
    // but to be erased if not modified again
    _modified[_currentPage] = false;
    _erase[_currentPage] = true;
    update();
}

/**
 * @brief Changes the pen color to black.
 */
void NotePainterWidget::on_blackButton_clicked()
{
    showBackgroundMenu(false);
    _pen.setColor(QColor("#2c2626"));
    on_penButton_clicked();
}

/**
 * @brief Changes the pen color to green.
 */
void NotePainterWidget::on_greenButton_clicked()
{
    showBackgroundMenu(false);
    _pen.setColor(QColor("#9ac51b"));
    on_penButton_clicked();
}

/**
 * @brief Changes the pen color to red.
 */
void NotePainterWidget::on_redButton_clicked()
{
    showBackgroundMenu(false);
    _pen.setColor(QColor("#d03f3b"));
    on_penButton_clicked();
}

/**
 * @brief Changes the pen color to yellow.
 */
void NotePainterWidget::on_yellowButton_clicked()
{
    showBackgroundMenu(false);
    _pen.setColor(QColor("#ffbd31"));
    on_penButton_clicked();
}

/**
 * @brief Closes the NotePainterWidget to return to the NoteMainWindow.
 *
 * Closes the concerned NotePainterWidget and saves the note to the
 * filesystem by calling saveImages(). Also the signal closeSignal()
 * is emitted and will be catched by NoteButton::NotePainterWidgetClosed().
 *
 */
void NotePainterWidget::on_backButton_clicked()
{
    // so it is closed when you reopen the Note
    showBackgroundMenu(false);

    saveImages();
    createIcon();
    emit(closeSignal());
    this->close();
}

/**
 * @brief Closes the hole application.
 */
void NotePainterWidget::on_quitButton_clicked()
{
    qApp->quit();
}

/**
 * @brief Closes the BackgroundMenu by calling showBackgroundMenu(bool show).
 *
 * @note The Button lies on top of part of the BackgroundMenu and is transparent.
 */
void NotePainterWidget::on_closeButton_clicked()
{
    showBackgroundMenu(false);
}

/**
 * @brief Activates the pen, so drawing mode is active.
 */
void NotePainterWidget::on_penButton_clicked()
{
    showBackgroundMenu(false);
    _rubber = false;

    // set normal pen width
    _pen.setWidth(PEN_WIDTH);

    // adapt ui
    ui->penButton->setChecked(true);
    ui->penButton->setDisabled(true);
    ui->rubberButton->setEnabled(true);
    ui->rubberButton->setChecked(false);
}

/**
 * @brief Activates the rubber, so erasing mode is active.
 */
void NotePainterWidget::on_rubberButton_clicked()
{
    showBackgroundMenu(false);
    _rubber = true;

    // make erasing easier by adding some size to the pen width
    _pen.setWidth(PEN_WIDTH + 4);

    // adapt ui
    ui->penButton->setChecked(true);
    ui->rubberButton->setDisabled(true);
    ui->penButton->setEnabled(true);
    ui->penButton->setChecked(false);
}

/**
 * @brief Initiates sending of the note.
 *
 * Saves the pages of the note as images before initiating sending if this note.
 * The pages need to be saved first so that the latest changes will be send.
 */
void NotePainterWidget::on_sendButton_clicked()
{
    saveImages();
    sendNote();
}

/**
 * @brief Initiates deleting of the note.
 *
 * To initiate deleting it emits the signal deleteSignal(), which will be
 * catched by NoteButton::NotePainterWidgetDeleteTriggered() where further
 * handling of the deletion process is done.
 */
void NotePainterWidget::on_delButton_clicked()
{
    emit(deleteSignal());
}
