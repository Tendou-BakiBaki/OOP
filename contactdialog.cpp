#include "contactdialog.h"
#include <regex>

using namespace std;

static regex number_mask(R"(^(\+7|8)(\(\d{3}\)\d{3}(\-\d{2}\-\d{2}|\d{7})|\d{10})$)");
static regex fio_mask(R"(^[a-zA-Zа-яА-ЯёЁ][a-zA-Zа-яА-ЯёЁ\d -]*[a-zA-Zа-яА-ЯёЁ\d]$)");
static regex data_mask(R"(^\d{2}\.\d{2}\.\d{4}$)");
static regex email_mask(R"(^[a-zA-Z0-9]+@[a-zA-Z0-9]+\.[a-zA-Z0-9]+$)");

static bool validateField(const string &text, int code) {
    switch (code) {
        case 1: return regex_match(text, fio_mask);
        case 2: return regex_match(text, number_mask);
        case 3: {
            bool con1 = regex_match(text, data_mask);
            int day = stoi(text.substr(0, 2));
            int month = stoi(text.substr(3, 2));
            int year = stoi(text.substr(6, 4));

            if (year < 1900 || year > 2025) return false;
            if (month < 1 || month > 12) return false;
            if (day < 1 || day > 31) return false;

            if (month == 2) {
                bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
                if (day > (isLeap ? 29 : 28)) return false;
            }
            else if (month == 4 || month == 6 || month == 9 || month == 11) {
                if (day > 30) return false;
            }
            return con1;
        }
        case 4: return regex_match(text, email_mask);
        default: return false;
    }
}

ContactDialog::ContactDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Добавить контакт");
    setModal(true);

    QFormLayout *formLayout = new QFormLayout(this);

    firstNameEdit = new QLineEdit(this);
    lastNameEdit = new QLineEdit(this);
    middleNameEdit = new QLineEdit(this);
    emailEdit = new QLineEdit(this);
    phone1Edit = new QLineEdit(this);
    phone2Edit = new QLineEdit(this);
    phone3Edit = new QLineEdit(this);
    dateEdit = new QLineEdit(this);
    addressEdit = new QLineEdit(this);

    dateEdit->setPlaceholderText("дд.мм.гггг");
    phone1Edit->setPlaceholderText("+7(XXX)XXX-XX-XX или 8XXXXXXXXXX");
    emailEdit->setPlaceholderText("example@mail.com");

    formLayout->addRow("Имя*:", firstNameEdit);
    formLayout->addRow("Фамилия*:", lastNameEdit);
    formLayout->addRow("Отчество:", middleNameEdit);
    formLayout->addRow("Email*:", emailEdit);
    formLayout->addRow("Телефон*:", phone1Edit);
    formLayout->addRow("Доп. телефон 1:", phone2Edit);
    formLayout->addRow("Доп. телефон 2:", phone3Edit);
    formLayout->addRow("Дата рождения:", dateEdit);
    formLayout->addRow("Адрес:", addressEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &ContactDialog::onAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ContactDialog::ContactDialog(const Contact &contact, QWidget *parent)
    : ContactDialog(parent)
{
    setWindowTitle("Редактировать контакт");

    firstNameEdit->setText(QString::fromStdString(contact.getFirstName()));
    lastNameEdit->setText(QString::fromStdString(contact.getLastName()));
    middleNameEdit->setText(QString::fromStdString(contact.getMiddleName()));
    emailEdit->setText(QString::fromStdString(contact.getEmail()));
    phone1Edit->setText(QString::fromStdString(contact.getPhone1()));
    phone2Edit->setText(QString::fromStdString(contact.getPhone2()));
    phone3Edit->setText(QString::fromStdString(contact.getPhone3()));
    dateEdit->setText(QString::fromStdString(contact.getDate()));
    addressEdit->setText(QString::fromStdString(contact.getAddress()));
}

bool ContactDialog::validateForm() {
    if (firstNameEdit->text().isEmpty() ||
        lastNameEdit->text().isEmpty() ||
        emailEdit->text().isEmpty() ||
        phone1Edit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля (*)");
        return false;
    }

    if (!validateField(firstNameEdit->text().toStdString(), 1)) {
        QMessageBox::warning(this, "Ошибка", "Некорректное имя");
        return false;
    }

    if (!validateField(lastNameEdit->text().toStdString(), 1)) {
        QMessageBox::warning(this, "Ошибка", "Некорректная фамилия");
        return false;
    }

    if (!validateField(emailEdit->text().toStdString(), 4)) {
        QMessageBox::warning(this, "Ошибка", "Некорректный email");
        return false;
    }

    if (!validateField(phone1Edit->text().toStdString(), 2)) {
        QMessageBox::warning(this, "Ошибка", "Некорректный номер телефона");
        return false;
    }

    if (!phone2Edit->text().isEmpty() && !validateField(phone2Edit->text().toStdString(), 2)) {
        QMessageBox::warning(this, "Ошибка", "Некорректный дополнительный телефон 1");
        return false;
    }

    if (!phone3Edit->text().isEmpty() && !validateField(phone3Edit->text().toStdString(), 2)) {
        QMessageBox::warning(this, "Ошибка", "Некорректный дополнительный телефон 2");
        return false;
    }

    if (!dateEdit->text().isEmpty() && !validateField(dateEdit->text().toStdString(), 3)) {
        QMessageBox::warning(this, "Ошибка", "Некорректная дата рождения");
        return false;
    }

    if (!middleNameEdit->text().isEmpty() && !validateField(middleNameEdit->text().toStdString(), 1)) {
        QMessageBox::warning(this, "Ошибка", "Некорректное отчество");
        return false;
    }

    return true;
}

void ContactDialog::onAccept() {
    if (validateForm()) {
        accept();
    }
}

Contact ContactDialog::getContact() const {
    return Contact(
        -1, // ID будет установлен позже
        firstNameEdit->text().toStdString(),
        lastNameEdit->text().toStdString(),
        emailEdit->text().toStdString(),
        phone1Edit->text().toStdString(),
        middleNameEdit->text().toStdString(),
        dateEdit->text().toStdString(),
        addressEdit->text().toStdString(),
        phone2Edit->text().toStdString(),
        phone3Edit->text().toStdString()
    );
}
