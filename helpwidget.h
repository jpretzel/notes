#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QWidget>

namespace Ui {
    class HelpWidget;
}

class HelpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HelpWidget(QWidget *parent = 0);
    ~HelpWidget();
    void showNotePainterWidgetHelp();
    void showExpanded();

private slots:
    void on_cancelButton_clicked();
    void on_nextButton_clicked();

private:
    Ui::HelpWidget *ui;
};

#endif // HELPWIDGET_H
