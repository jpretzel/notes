#include "notebutton.h"
#include <QDebug>

NoteButton::NoteButton()
{
    setIconSize(QSize(111, 197));
    setStyleSheet("QPushButton#noteButton{height: 197px; width: 111px;}");
    setFlat(true);
    update();
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
    this->fileName = fileName;
    setStyleSheet("QPushButton#noteButton{height: 197px; width: 111px;}");
    setFlat(true);
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
    //this->setIcon();
}

bool NoteButton::isHighlighted(){
    return _highlight;
}

void NoteButton::mousePressEvent(QMouseEvent *){
    if(_highlight){
        setHighlighted(false);
    }else{
        setHighlighted(true);
    }
}

void NoteButton::mouseDoubleClickEvent(QMouseEvent *){
    qDebug() << "[creating new NotePainterWidget]";
    painterWidget = new NotePainterWidget();

    if(!fileName.isEmpty()){
        painterWidget->setFileName(this->fileName);
        painterWidget->loadNotes();
    }

    #if defined(Q_WS_MAC)
        painterWidget->show();
    #else
        painterWidget->showFullScreen();
    #endif
    doConnect();
}

void NoteButton::doConnect(){
    qDebug() << "[connecting painterWidget]";
    connect(painterWidget, SIGNAL(closeSignal()), this, SLOT(NotePainterWidgetClosed()));
}

void NoteButton::NotePainterWidgetClosed(){
    updateIcon();
    qDebug() << "[disconnecting painterWidget]";
    painterWidget->disconnect(painterWidget, SIGNAL(closeSignal()), this, SLOT(NotePainterWidgetClosed()));
    qDebug() << "[close signal captured --> deleting ]";
    painterWidget->deleteLater();
}

void NoteButton::setNotePainterWidget(NotePainterWidget * p){
    this->painterWidget = p;
}

NotePainterWidget * NoteButton::getNotePainterWidget(){
    return painterWidget;
}

