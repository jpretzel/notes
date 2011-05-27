#include "helpwidget.h"
#include "ui_helpwidget.h"

/**
 * @brief Constructor of HelpWidget.
 */
HelpWidget::HelpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpWidget)
{
    ui->setupUi(this);
    ui->secondWidget->setVisible(false);
}

/**
 * @brief Destructor of HelpWidget.
 */
HelpWidget::~HelpWidget()
{
    delete ui;
}

/**
 * @brief Closes the HelpWidget.
 */
void HelpWidget::on_cancelButton_clicked()
{
    this->close();
}

/**
 * @brief Switches between the two help pages.
 */
void HelpWidget::on_nextButton_clicked()
{
    ui->secondWidget->setVisible(!ui->secondWidget->isVisible());
}

/**
 * @brief Handles how the Application will be shown on different Operating Systems.
 *
 * At the moment only Mac OS X gets a special treatment. Just add another OS if needed.
 */
void HelpWidget::showExpanded()
{
#ifdef Q_WS_MAC
    show();
#else
    showFullScreen();
#endif
}
