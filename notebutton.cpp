#include "notebutton.h"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>

NoteButton::NoteButton()
{
    setIconSize(QSize(111, 197));
    setStyleSheet("NoteButton:checked{background-image: url(:/backgrounds/images/highlightframe.png);}");
    setFlat(true);
    setCheckable(true);
    painterWidget = new NotePainterWidget();
    doConnect();
    painterWidget->showExpanded();
}

NoteButton::NoteButton(QPixmap pixmap, QString fileName)
{
    painterWidget = NULL;
    setIconSize(QSize(111, 197));
    setIcon(pixmap);
    this->_fileName = fileName;
    setStyleSheet("NoteButton:checked{background-image: url(:/backgrounds/images/highlightframe.png);}");
    setFlat(true);
    setCheckable(true);
}

void NoteButton::loadStyleSheet(){
    QString style;
}

void NoteButton::setHighlighted(bool hl){
    _highlight = hl;
    if(_highlight){
        setStyleSheet("");
    }else{
        setStyleSheet("");
    }
}

void NoteButton::updateIcon(){
    if(painterWidget != NULL){
        qDebug() << "[UPDATING ICON] " << this;
        QPixmap bg(360, 640);
        bg.fill(Qt::white);
        QPainter painter(&bg);
        painter.drawPixmap(QPoint(0, 0), painterWidget->getPageOne());
        setIcon(bg);
    }
}

bool NoteButton::isHighlighted(){
    return _highlight;
}

void NoteButton::mousePressEvent(QMouseEvent *event)
{
    _p1 = event->pos();
}

void NoteButton::mouseReleaseEvent(QMouseEvent *event)
{
    _p2 = event->pos();
    if(abs(_p1.x() - _p2.x()) < 10 && abs(_p1.y() - _p2.y()) < 10 && isChecked())
        openNote();

    setChecked(true);
    emit(releaseEvent());
}

void NoteButton::openNote()
{
    if(painterWidget == NULL)
    {
        painterWidget = new NotePainterWidget();
        if(!_fileName.isEmpty()){
            qDebug() << "[OPEN] openNote with filename:" << _fileName;
            qDebug() << "[OPEN] parent:" << this;
            painterWidget->setFileName(this->_fileName);
            painterWidget->loadNotes();
        }
    }

    painterWidget->showExpanded();
}

QString NoteButton::getFileName()
{
    return _fileName;
}

void NoteButton::doConnect(){
    qDebug() << "[connecting painterWidget]";
    connect(painterWidget, SIGNAL(closeSignal()), this, SLOT(NotePainterWidgetClosed()));
    connect(painterWidget, SIGNAL(deleteSignal()), this, SLOT(NotePainterWidgetDeleteTriggered()));
}

void NoteButton::doDisconnect(){
    qDebug() << "[disconnecting painterWidget]";
    disconnect(painterWidget, SIGNAL(closeSignal()), this, SLOT(NotePainterWidgetClosed()));
    disconnect(painterWidget, SIGNAL(deleteSignal()), this, SLOT(NotePainterWidgetDeleteTriggered()));
}

void NoteButton::NotePainterWidgetDeleteTriggered(){
    painterWidget->close();
    doDisconnect();
    qDebug() << "[close signal from NotePainterWidget captured --> deleting ]";
    emit(deleteMe(this));
}

void NoteButton::NotePainterWidgetClosed(){
    doDisconnect();
    qDebug() << "[close signal captured --> deleting ]";
    _fileName = painterWidget->getFilename();
    emit(updateMe(this));
    painterWidget->deleteLater();
    painterWidget = NULL;
}

void NoteButton::setNotePainterWidget(NotePainterWidget * p){
    this->painterWidget = p;
}

NotePainterWidget * NoteButton::getNotePainterWidget(){
    if (painterWidget == NULL)
    {
        painterWidget = new NotePainterWidget();
        if(!_fileName.isEmpty()){
            painterWidget->setFileName(this->_fileName);
        }
    }

    return painterWidget;
}

