#ifndef NOTEBUTTON_H
#define NOTEBUTTON_H

#include <QPushButton>
#include "notepainterwidget.h"

class NoteButton : public QPushButton
{
    Q_OBJECT
public:
    NoteButton();
    NoteButton(QPixmap pixmap, QString fileName);
    bool isHighlighted();
    void setHighlighted(bool hl);
    void updateIcon();
    NotePainterWidget * getNotePainterWidget();
    void setNotePainterWidget(NotePainterWidget * p);
    QString getFileName();

private:
    void loadStyleSheet();
    bool _highlight;
    QString _fileName;
    NotePainterWidget *painterWidget;
    void doConnect();
    void doDisconnect();
    void openNote();

public slots:
    void NotePainterWidgetClosed();

signals:
    void updateMe(NoteButton *);

protected:
    void mousePressEvent( QMouseEvent * );

};

#endif // NOTEBUTTON_H
