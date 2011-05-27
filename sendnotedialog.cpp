#include "sendnotedialog.h"
#include "ui_sendnotedialog.h"

/**
 * @brief Constructor of SendNoteDialog.
 */
SendNoteDialog::SendNoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendNoteDialog)
{
    ui->setupUi(this);
}

/**
 * @brief Destructor of SendNoteDialog.
 */
SendNoteDialog::~SendNoteDialog()
{
    delete ui;
}

/**
 * @brief Opens a SendNoteDialog and returns a QString.
 *
 * Opens a SendNoteDialog and retunrs the input of the emailInput field
 * if the dialog was accepted or a NULL QString.
 *
 * @param parent
 *      The QWdiget that is parent of the dialog.
 *
 *  @return
 *      Either the input of the email field or a NULL QString.
 */
QString SendNoteDialog::getEmailAddress(QWidget *parent)
{
    SendNoteDialog dialog(parent);

    // Fullscreen for Symbian
    #ifdef Q_OS_SYMBIAN
    dialog.setWindowState(dialog.windowState() | Qt::WindowFullScreen);
    #endif

    if (dialog.exec() == QDialog::Accepted)
    {
        return dialog.ui->emailInput->text();
    }

    return QString();
}

/**
 * @brief Set the state of the dialog to accepted.
 */
void SendNoteDialog::on_okButton_clicked()
{
    accept();
}

/**
 * @brief Set the state of the dialog to rejected.
 */
void SendNoteDialog::on_cancelButton_clicked()
{
    reject();
}
