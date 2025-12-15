#include "telephone.h"
#include "ui_telephone.h"
#include "contactdialog.h"
#include <QInputDialog>
#include <QRegularExpression>
#include <sstream>
#include <iostream>

// Конструктор, здесь настраивается интерфейс при запуске
telephone::telephone(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::telephone)
{
    ui->setupUi(this);

    setWindowTitle("Телефонный справочник");

    setupTable();

    // Контекстное меню для таблицы
    contextMenu = new QMenu(this);
    addAction = new QAction("Добавить контакт", this);
    editAction = new QAction("Редактировать контакт", this);
    deleteAction = new QAction("Удалить контакт", this);

    contextMenu->addAction(addAction);
    contextMenu->addAction(editAction);
    contextMenu->addAction(deleteAction);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested,
            this, &telephone::on_tableWidget_customContextMenuRequested);
    connect(addAction, &QAction::triggered, this, &telephone::addContact);
    connect(editAction, &QAction::triggered, this, &telephone::editContact);
    connect(deleteAction, &QAction::triggered, this, &telephone::deleteContact);

    loadContacts();

    ui->statusbar->showMessage(QString("Загружено контактов: %1").arg(contacts.size()));
    ui->textEdit->setPlaceholderText("Введите текст для поиска...");
}

telephone::~telephone()
{
    delete ui;
}

// Настройка таблицы - поведение, сортировка
void telephone::setupTable()
{
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

// Обновление таблицы данными из вектора contacts
void telephone::refreshTable()
{
    ui->tableWidget->setRowCount(0);

    for (size_t i = 0; i < contacts.size(); i++) {
        const Contact& c = contacts[i];
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row, 0,
            new QTableWidgetItem(QString::fromStdString(c.getLastName())));
        ui->tableWidget->setItem(row, 1,
            new QTableWidgetItem(QString::fromStdString(c.getFirstName())));
        ui->tableWidget->setItem(row, 2,
            new QTableWidgetItem(QString::fromStdString(c.getMiddleName())));
        ui->tableWidget->setItem(row, 3,
            new QTableWidgetItem(QString::fromStdString(c.getEmail())));

        std::string phone = c.getPhone1();
        if (!phone.empty()) {
            phone = formatPhone(phone);
        }
        ui->tableWidget->setItem(row, 4,
            new QTableWidgetItem(QString::fromStdString(phone)));

        ui->tableWidget->setItem(row, 5,
            new QTableWidgetItem(QString::fromStdString(c.getDate())));
        ui->tableWidget->setItem(row, 6,
            new QTableWidgetItem(QString::fromStdString(c.getAddress())));
        ui->tableWidget->setItem(row, 7,
            new QTableWidgetItem(QString::fromStdString(c.getPhone2())));
        ui->tableWidget->setItem(row, 8,
            new QTableWidgetItem(QString::fromStdString(c.getPhone3())));
    }

    ui->tableWidget->resizeColumnsToContents();
    ui->statusbar->showMessage(QString("Всего контактов: %1").arg(contacts.size()));
}

// Загрузка контактов из файла cont.txt
void telephone::loadContacts()
{
    std::ifstream file("cont.txt");
    if (!file.is_open()) {
        std::cout << "Файл cont.txt не найден. Будет создан новый." << std::endl;
        return;
    }

    std::string line;
    int count = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string fn, ln, email, phone1, mn, date, adrs, phone2, phone3;

        std::getline(ss, fn, ',');
        std::getline(ss, ln, ',');
        std::getline(ss, email, ',');
        std::getline(ss, phone1, ',');
        std::getline(ss, mn, ',');
        std::getline(ss, date, ',');
        std::getline(ss, adrs, ',');
        std::getline(ss, phone2, ',');
        std::getline(ss, phone3, ',');

        contacts.push_back(Contact(fn, ln, email, phone1, mn, date, adrs, phone2, phone3));
        count++;
    }

    file.close();
    refreshTable();
    std::cout << "Загружено контактов: " << count << std::endl;
}

// Сохранение контактов в файл
void telephone::saveContacts()
{
    std::ofstream file("cont.txt", std::ios::trunc);

    for (const Contact& c : contacts) {
        file << c.getFirstName() << ","
             << c.getLastName() << ","
             << c.getEmail() << ","
             << c.getPhone1() << ","
             << c.getMiddleName() << ","
             << c.getDate() << ","
             << c.getAddress() << ","
             << c.getPhone2() << ","
             << c.getPhone3() << "\n";
    }

    file.close();
    ui->statusbar->showMessage("Данные сохранены в cont.txt", 2000);
}

// Поиск контактов по введенному тексту
void telephone::on_pushButton_clicked()
{
    QString query = ui->textEdit->toPlainText().trimmed();

    if (query.isEmpty()) {
        refreshTable();
        ui->statusbar->showMessage("Показаны все контакты", 2000);
        return;
    }

    QString searchQuery = query.toLower();
    std::vector<int> searchResults;

    for (size_t i = 0; i < contacts.size(); i++) {
        const Contact& c = contacts[i];

        QString firstName = QString::fromStdString(c.getFirstName()).toLower();
        QString lastName = QString::fromStdString(c.getLastName()).toLower();
        QString middleName = QString::fromStdString(c.getMiddleName()).toLower();
        QString email = QString::fromStdString(c.getEmail()).toLower();
        QString phone1 = QString::fromStdString(formatPhone(c.getPhone1())).toLower();
        QString phone2 = QString::fromStdString(formatPhone(c.getPhone2())).toLower();
        QString phone3 = QString::fromStdString(formatPhone(c.getPhone3())).toLower();

        if (firstName.contains(searchQuery) ||
            lastName.contains(searchQuery) ||
            middleName.contains(searchQuery) ||
            email.contains(searchQuery) ||
            phone1.contains(searchQuery) ||
            phone2.contains(searchQuery) ||
            phone3.contains(searchQuery)) {
            searchResults.push_back(i);
        }
    }

    displaySearchResults(searchResults);

    if (!searchResults.empty()) {
        ui->statusbar->showMessage(
            QString("Найдено %1 контакт(ов)").arg(searchResults.size()),
            3000);
    }
}

// Показать результаты поиска в таблице
void telephone::displaySearchResults(const std::vector<int>& results)
{
    ui->tableWidget->setRowCount(0);

    if (results.empty()) {
        QMessageBox::information(this, "Результаты поиска",
            "Контакты не найдены.\n\nПопробуйте изменить поисковый запрос.");
        return;
    }

    for (int idx : results) {
        if (idx < 0 || idx >= (int)contacts.size()) {
            continue;
        }

        const Contact& c = contacts[idx];
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row, 0,
            new QTableWidgetItem(QString::fromStdString(c.getLastName())));
        ui->tableWidget->setItem(row, 1,
            new QTableWidgetItem(QString::fromStdString(c.getFirstName())));
        ui->tableWidget->setItem(row, 2,
            new QTableWidgetItem(QString::fromStdString(c.getMiddleName())));
        ui->tableWidget->setItem(row, 3,
            new QTableWidgetItem(QString::fromStdString(c.getEmail())));

        std::string phone = c.getPhone1();
        if (!phone.empty()) phone = formatPhone(phone);
        ui->tableWidget->setItem(row, 4,
            new QTableWidgetItem(QString::fromStdString(phone)));

        ui->tableWidget->setItem(row, 5,
            new QTableWidgetItem(QString::fromStdString(c.getDate())));
        ui->tableWidget->setItem(row, 6,
            new QTableWidgetItem(QString::fromStdString(c.getAddress())));
        ui->tableWidget->setItem(row, 7,
            new QTableWidgetItem(QString::fromStdString(c.getPhone2())));
        ui->tableWidget->setItem(row, 8,
            new QTableWidgetItem(QString::fromStdString(c.getPhone3())));
    }

    ui->tableWidget->resizeColumnsToContents();
}

// Сохранение в файл
void telephone::on_action_2_triggered()
{
    saveContacts();
}

// Выход из программы
void telephone::on_action_4_triggered()
{
    QApplication::quit();
}

// Показать информацию о программе
void telephone::on_action_5_triggered()
{
    QMessageBox::about(this, "О программе",
                      "Телефонный справочник\n\n"
                      "Версия 1.0\n\n"
                      "Функции:\n"
                      "- Просмотр всех контактов\n"
                      "- Поиск по всем полям\n"
                      "- Добавление/редактирование/удаление контактов\n"
                      "- Сохранение данных в файл\n"
                      "- Валидация вводимых данных\n\n"
                      "© 2025");
}

// Показать контекстное меню при правом клике на таблице
void telephone::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->tableWidget->mapToGlobal(pos);
    contextMenu->exec(globalPos);
}

// Добавить новый контакт через диалоговое окно
void telephone::addContact()
{
    ContactDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Contact newContact = dialog.getContact();
        contacts.push_back(newContact);
        saveContacts();
        refreshTable();
        ui->statusbar->showMessage("Контакт добавлен", 2000);
    }
}

// Редактировать выбранный контакт
void telephone::editContact()
{
    int row = ui->tableWidget->currentRow();
    if (row == -1 || row >= (int)contacts.size()) {
        QMessageBox::warning(this, "Ошибка", "Выберите контакт для редактирования");
        return;
    }

    ContactDialog dialog(contacts[row], this);
    if (dialog.exec() == QDialog::Accepted) {
        contacts[row] = dialog.getContact();
        saveContacts();
        refreshTable();
        ui->statusbar->showMessage("Контакт обновлен", 2000);
    }
}

// Удалить выбранный контакт
void telephone::deleteContact()
{
    int row = ui->tableWidget->currentRow();
    if (row == -1 || row >= (int)contacts.size()) {
        QMessageBox::warning(this, "Ошибка", "Выберите контакт для удаления");
        return;
    }

    QString name = QString::fromStdString(contacts[row].getLastName() + " " +
                                         contacts[row].getFirstName());

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение",
                                 "Вы уверены, что хотите удалить контакт:\n" + name + "?",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        contacts.erase(contacts.begin() + row);
        saveContacts();
        refreshTable();
        ui->statusbar->showMessage("Контакт удален", 2000);
    }
}

// Форматирование номера телефона (убираем все нецифры и меняем 7 на 8)
std::string telephone::formatPhone(const std::string &number)
{
    std::string res;
    for (char c : number) {
        if (std::isdigit(c)) {
            res += c;
        }
    }
    if (!res.empty() && res[0] == '7') {
        res[0] = '8';
    }
    return res;
}
