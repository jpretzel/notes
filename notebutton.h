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

private:
    void loadStyleSheet();
    bool _highlight;
    QString fileName;
    NotePainterWidget *painterWidget;
    void doConnect();

public slots:
    void NotePainterWidgetClosed();

protected:
    void mousePressEvent( QMouseEvent * );
    void mouseDoubleClickEvent( QMouseEvent * );

};

#endif // NOTEBUTTON_H
