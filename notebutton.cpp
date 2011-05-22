#include "notebutton.h"
#include <QDebug>
#include <QPainter>

NoteButton::NoteButton()
{
    setIconSize(QSize(111, 197));
    setStyleSheet("NoteButton{height:197px;width:111px;}");
    setFlat(true);
    setCheckable(true);
    painterWidget = new NotePainterWidget();
    doConnect();
    #if defined(Q_WS_MAC)
            painterWidget->show();
    #else
            painterWidget->showFullScreen();
    #endif
}

NoteButton::NoteButton(QPixmap pixmap, QString fileName)
{
    setIconSize(QSize(111, 197));
    setIcon(pixmap);
    this->_fileName = fileName;
    setStyleSheet("NoteButton:checked{border-style:outset;border-radius:21px;border-width:6px;border-color:#b9aead;}");
    setFlat(true);
    setCheckable(true);
}

void NoteButton::loadStyleSheet(){
    QString style;
}

void NoteButton::setHighlighted(bool hl){
    _highlight = hl;
    if(_highlight){
        setStyleSheet("NoteButton{height:197px;width:111px;border-style:outset;border-radius:21px;border-width:6px;border-color:#b9aead;}");
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

void NoteButton::mousePressEvent(QMouseEvent *){
    if(!isChecked())
    {
        setChecked(true);
    } else {
        openNote();
    }
}

void NoteButton::openNote()
{
    painterWidget = new NotePainterWidget();
    if(!_fileName.isEmpty()){
        qDebug() << "[OPEN] openNote with filename:" << _fileName;
        qDebug() << "[OPEN] parent:" << this;
        painterWidget->setFileName(this->_fileName);
        painterWidget->loadNotes();
    }

    #if defined(Q_WS_MAC)
        painterWidget->show();
    #else
        painterWidget->showFullScreen();
    #endif
    doConnect();
}

QString NoteButton::getFileName()
{
    return _fileName;
}

void NoteButton::doConnect(){
    qDebug() << "[connecting painterWidget]";
    connect(painterWidget, SIGNAL(closeSignal()), this, SLOT(NotePainterWidgetClosed()));
}

void NoteButton::doDisconnect(){
    qDebug() << "[disconnecting painterWidget]";
    disconnect(painterWidget, SIGNAL(closeSignal()), this, SLOT(NotePainterWidgetClosed()));
}

void NoteButton::NotePainterWidgetClosed(){
    doDisconnect();
    qDebug() << "[close signal captured --> deleting ]";
    emit(updateMe(this));
    painterWidget->deleteLater();
}

void NoteButton::setNotePainterWidget(NotePainterWidget * p){
    this->painterWidget = p;
}

NotePainterWidget * NoteButton::getNotePainterWidget(){
    return painterWidget;
}

