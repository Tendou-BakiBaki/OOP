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
    // Конструктор для добавления (пустая форма)
    ContactDialog(QWidget *parent = nullptr);

    // Конструктор для редактирования (заполненная форма)
    ContactDialog(const Contact &contact, QWidget *parent = nullptr);

    // Получить данные из формы
    Contact getContact() const;

private:
    // Поля формы
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;
    QLineEdit *middleNameEdit;
    QLineEdit *emailEdit;
    QLineEdit *phone1Edit;
    QLineEdit *phone2Edit;
    QLineEdit *phone3Edit;
    QLineEdit *dateEdit;
    QLineEdit *addressEdit;

    // Валидация
    bool validateForm();

private slots:
    void onAccept();
};

#endif // CONTACTDIALOG_H
