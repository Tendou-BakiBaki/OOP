#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include "contact.h"

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    ContactDialog(QWidget *parent = nullptr);
    ContactDialog(const Contact &contact, QWidget *parent = nullptr);
    Contact getContact() const;

private:
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;
    QLineEdit *middleNameEdit;
    QLineEdit *emailEdit;
    QLineEdit *phone1Edit;
    QLineEdit *phone2Edit;
    QLineEdit *phone3Edit;
    QLineEdit *dateEdit;
    QLineEdit *addressEdit;

    bool validateForm();

private slots:
    void onAccept();
};

#endif
