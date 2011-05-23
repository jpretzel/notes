#ifndef NOTEPAINTERWIDGET_H
#define NOTEPAINTERWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QPen>

namespace Ui {
    class NotePainterWidget;
}

class NotePainterWidget : public QWidget
{
    Q_OBJECT

public:
    // Constructors:
    explicit NotePainterWidget(QWidget *parent = 0);

    // Destructor:
    ~NotePainterWidget();

    // Methods relevant for painting:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    // Methods:
    void setFileName(QString newFileName);
    void loadNotes();
    void setIcon(QPixmap pixmap);
    void sendNote();
    void showExpanded();
    QIcon getIcon();
    QPixmap getPageOne();
    QString getFilename();

private:
    Ui::NotePainterWidget *ui;

    // Gesture recording data:
    QPoint _lastPoint;

    // GUI control flags:
    bool _drawMode;
    bool _lineDrawMode;
    bool _dogEarClicked;
    bool _upperDogEarClicked;
    bool _paintWidgetDisabled;

    // Data container:
    QIcon _icon;
    QPixmap _pixmap[4];

    // Filesaving and loading data:
    bool _modified[4];
    bool _erase[4];
    bool _rubber;
    QString _fileName;
    QString _loadDir;
    QString _dir;
    int _startI;

    // Drawing data:
    QPen _pen;
    QPoint _p1;
    QPoint _p2;
    int _currentPage;

    // Methods:
    void showBackgroundMenu(bool);
    void saveState();
    void createIcon();
    void paintToPixmap();
    void initDrawTools();
    void saveImages();

private slots:
    void on_clearButton_clicked();
    void on_redButton_clicked();
    void on_blackButton_clicked();
    void on_yellowButton_clicked();
    void on_greenButton_clicked();
    void on_menuButton_clicked();
    void on_backButton_clicked();
    void on_quitButton_clicked();
    void on_penButton_clicked();
    void on_rubberButton_clicked();
    void on_closeButton_clicked();
    void on_sendButton_clicked();

    void on_delButton_clicked();

signals:
    void closeSignal();
    void deleteSignal();
};

#endif // NOTEPAINTERWIDGET_H
