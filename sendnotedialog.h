#ifndef SENDNOTEDIALOG_H
#define SENDNOTEDIALOG_H

#include <QDialog>

namespace Ui {
    class SendNoteDialog;
}

/**
 * SendNoteDialog inherits QDialog. It is a simple dialog with
 * only one input, that is the email input and two QButtons, one to
 * accept and one to reject the dialog. It is used by calling
 * getEmailAddress(QWdiget *parent = 0). For example:
 * @code { QString email = SendNoteDialog::getEmailAddress(this) }
 *
 *
 * @author Jan Pretzel (jan.pretzel@deepsource.de)
 * @author Sebastian Ullrich (sebastian.ullrich@deepsource.de)
 */
class SendNoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendNoteDialog(QWidget *parent = 0);
    ~SendNoteDialog();
    static QString getEmailAddress(QWidget *parent = 0);

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::SendNoteDialog *ui;
};

#endif // SENDNOTEDIALOG_H
