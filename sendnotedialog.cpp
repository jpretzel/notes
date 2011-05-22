#include "sendnotedialog.h"
#include "ui_sendnotedialog.h"
#include <QDebug>

SendNoteDialog::SendNoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendNoteDialog)
{
    ui->setupUi(this);
}

SendNoteDialog::~SendNoteDialog()
{
    delete ui;
}

QString SendNoteDialog::getEmailAdress(QWidget *parent)
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

void SendNoteDialog::on_okButton_clicked()
{
    accept();
}

void SendNoteDialog::on_cancelButton_clicked()
{
    reject();
}
