#ifndef NOTEBUTTON_H
#define NOTEBUTTON_H

#include <QPushButton>
#include "notepainterwidget.h"

/**
 * NoteButton represents the connection between a NotePainterWidget and the
 * MainWindow. It inherits QPushButton and always shows the first page of the
 * corresponding note, partly presented by NotePainterWidget, as its icon.
 *
 * @author Jan Pretzel (jan.pretzel@deepsource.de)
 * @author Sebastian Ullrich (sebastian.ullrich@deepsource.de)
 */
class NoteButton : public QPushButton
{
    Q_OBJECT
public:
    // Constructors:
    NoteButton();
    NoteButton(QPixmap pixmap, QString fileName);

    // Methods:
    void updateIcon();
    NotePainterWidget * getNotePainterWidget();
    void setNotePainterWidget(NotePainterWidget * p);
    QString getFileName();

    // Methods to handle scrolling and opening of buttons:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    QPoint _p1;
    QPoint _p2;
    QString _fileName;
    NotePainterWidget *painterWidget;

    // Methods:
    void doConnect();
    void doDisconnect();
    void openNote();

public slots:
    void NotePainterWidgetClosed();
    void NotePainterWidgetDeleteTriggered();

signals:
    void updateMe(NoteButton *);
    void deleteMe(NoteButton *);
    void releaseEvent();
};

#endif // NOTEBUTTON_H
