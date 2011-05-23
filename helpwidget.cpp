#include "helpwidget.h"
#include "ui_helpwidget.h"

HelpWidget::HelpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpWidget)
{
    ui->setupUi(this);
    ui->secondWidget->setVisible(false);
}

HelpWidget::~HelpWidget()
{
    delete ui;
}

void HelpWidget::on_cancelButton_clicked()
{
    this->close();
}

void HelpWidget::showNotePainterWidgetHelp(){
    ui->secondWidget->setVisible(true);
}

void HelpWidget::on_cancelButton_2_clicked()
{
    ui->secondWidget->setVisible(!ui->secondWidget->isVisible());
}
