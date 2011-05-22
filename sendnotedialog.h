#ifndef SENDNOTEDIALOG_H
#define SENDNOTEDIALOG_H

#include <QDialog>

namespace Ui {
    class SendNoteDialog;
}

class SendNoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendNoteDialog(QWidget *parent = 0);
    ~SendNoteDialog();
    static QString getEmailAdress(QWidget* parent = 0);

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::SendNoteDialog *ui;
};

#endif // SENDNOTEDIALOG_H
