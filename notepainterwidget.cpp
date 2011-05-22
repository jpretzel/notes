#include "notepainterwidget.h"
#include "ui_notepainterwidget.h"
#include <QPainter>
#include <QColor>
#include <QMatrix>
#include <QDebug>
#include <QMouseEvent>
#include <QDir>
#include <QDateTime>

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

NotePainterWidget::NotePainterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotePainterWidget)
{
    ui->setupUi(this);

#if defined(Q_WS_MAC)
    _loadDir = "/Users/sebastian/Desktop/notes/";
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

NotePainterWidget::~NotePainterWidget()
{
    delete ui;
}

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
        _p1 = event->pos();
        _p2 = QPoint(NIRVANA, NIRVANA);
    }
}

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
        } else if (_currentPage < MAX_PAGES) {
            _currentPage++;
            initDrawTools();
        }
        _dogEarClicked  = false;

        // upper dogear clicked
    } else if (_upperDogEarClicked) {
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
 * is drawn to the actual widget in @see { paintEvent(QPaintEvent *) }.
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
 * Loads all images belonging to the concerned note, and stores them in @see { _pixmap[] }
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
                qDebug() << loadDir + notePages.at(i);
                _pixmap[notePages.at(i).mid(13, 1).toInt()] = pixmap;
            }
        }
    }
}

QPixmap NotePainterWidget::getPageOne()
{
    if (_pixmap[0].isNull())
    {
        _pixmap[0] = QPixmap(360, 640);
        _pixmap[0].fill(Qt::transparent);
    }

    return _pixmap[0];
}

QString NotePainterWidget::getFilename()
{
    return _fileName;
}

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

        if (_fileName == "")
        {
            _fileName = QDateTime::currentDateTime().toString("yymmddhhmmss");
        }

        // create sub_directory
        QString loadDir = _loadDir + _fileName + _dir;

        // if needed? mk_dir() returns false if not successfully
        if (!QDir(loadDir).exists())
        {
            QDir().mkdir(loadDir);
        }

        for(int page = 0; page <= MAX_PAGES; page++){
            QFile file(loadDir + _fileName + "_" + QString::number(page) + ".png");

            if (_modified[page])
            {
                QPixmap pixmap( 360, 640 );

                // images must be saved with transparent background,
                // so they can be loaded properly
                pixmap.fill(Qt::transparent);
                QPainter painter(&pixmap);
                painter.setPen(_pen);

                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.drawPixmap(QPoint(0, 0), _pixmap[page]);

                pixmap.save(file.fileName());
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

void NotePainterWidget::setFileName(QString newFileName)
{
    _fileName = newFileName;
}

void NotePainterWidget::setIcon(QPixmap pixmap)
{
    _pixmap[0] = pixmap;
    createIcon();
}

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

QIcon NotePainterWidget::getIcon()
{
    return this->_icon;
}

void NotePainterWidget::closeEvent(QCloseEvent *)
{
    saveImages();
    emit(closeSignal());
    createIcon();
}

void NotePainterWidget::sendNote()
{
    QString emailAdress = SendNoteDialog::getEmailAdress(this);
#if !defined(Q_WS_MAC)
    qDebug() << "[EMAIL]";

    /*QStringList availableManagers = QContactManager::availableManagers();

        for(int managerIdx = 0; managerIdx < availableManagers.count(); managerIdx++) {
            QContactManager * manager = new QContactManager(availableManagers.at(managerIdx));

            if(manager) {
                QList<QContact> contacts = manager->contacts();
                for(int i = 0; i < contacts.count(); i++){
                    qDebug() << contacts.at(i);
                }
                delete manager;
            }
        }*/

    QMessageService * service = new QMessageService;
    QMessageAddress sendTo(QMessageAddress::Email, emailAdress);

    QMessage msg;
    msg.setType(QMessage::Email);

    // Setting the stored EmailAdress as sender.
    msg.setParentAccountId(QMessageAccount::defaultAccount(QMessage::Email));

    msg.setTo(sendTo);
    msg.setSubject("notes for you :)");

    QStringList notePages = QDir(_loadDir + _fileName).entryList();
    QStringList absPath;

    for(int i = 0; i < notePages.size(); i++)
    {
        absPath.append(_loadDir + _fileName + _dir + notePages.at(i));
    }

    msg.appendAttachments(absPath);

    if(service->send(msg))
    {
        qDebug() << "[EMAIL] Email sent successfully!";
    } else qDebug() << "[EMAIL] Unable to send Email!";
#endif

}

/* =========================================================================
 * BackgroundMenu functionality
 * ========================================================================= */

/**
 * @brief Shows or hides the BackgroundMenu.
 *
 * @param show Defines whether the BackgroundMenu should be shown or hidden.
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
 * Closes the concerned NotePainterWidget and saves the Note to the
 * filesystem by calling @see{ saveImages() }.
 *
 */
void NotePainterWidget::on_backButton_clicked()
{
    // so it is closed when you reopen the Note
    showBackgroundMenu(false);
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
 * @brief Closes the BackgroundMenu by calling @see{ showBackgroundMenu(bool show) }.
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

void NotePainterWidget::on_sendButton_clicked()
{
    saveImages();
    sendNote();
}

void NotePainterWidget::on_delButton_clicked()
{
    emit(deleteSignal());
}
