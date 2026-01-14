#include "telephone.h"
#include "ui_telephone.h"
#include "contactdialog.h"
#include <QInputDialog>
#include <QRegularExpression>
#include <QTextStream>
#include <QMessageBox>
#include <QTimer>
#include <QHeaderView>
#include <QFile>
#include <sstream>
#include <iostream>

using namespace std;

telephone::telephone(QWidget *parent):QMainWindow(parent), ui(new Ui::telephone)
{
    ui->setupUi(this);
    setWindowTitle("Телефонный справочник");

    setupTable();

    fileLoader = new FileData("contact.txt");
    dbLoader = new DBData();
    currentLoader = fileLoader;

    cout << "Созданы хранилища" << endl;

    connect(ui->action_11, &QAction::triggered, this, &telephone::useFileStorage);
    connect(ui->action_12, &QAction::triggered, this, &telephone::useDBStorage);

    contextMenu = new QMenu(this);
    addAction = new QAction("Добавить контакт", this);
    editAction = new QAction("Редактировать контакт", this);
    deleteAction = new QAction("Удалить контакт", this);

    contextMenu->addAction(addAction);
    contextMenu->addAction(editAction);
    contextMenu->addAction(deleteAction);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &telephone::right_click);
    connect(addAction, &QAction::triggered, this, &telephone::addContact);
    connect(editAction, &QAction::triggered, this, &telephone::editContact);
    connect(deleteAction, &QAction::triggered, this, &telephone::deleteContact);
    connect(ui->action_3, &QAction::triggered, this, &telephone::save_click);
    connect(ui->action_4, &QAction::triggered, this, &telephone::out_click);
    connect(ui->action_5, &QAction::triggered, this, &telephone::show_click);
    connect(ui->pushButton, &QPushButton::clicked, this, &telephone::search_click);

    cout << "Начинаю загрузку данных..." << endl;
    contacts = currentLoader->load();
    cout << "Загружено контактов: " << contacts.size() << endl;

    refreshTable();

    ui->statusbar->showMessage(QString("Загружено контактов: %1").arg(contacts.size()));
    ui->textEdit->setPlaceholderText("Введите текст для поиска...");

    updateStorageMenu();
}

telephone::~telephone()
{
    delete fileLoader;
    delete dbLoader;
    delete ui;
}

void telephone::setupTable()
{
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    QStringList headers;
    headers << "Фамилия" << "Имя" << "Отчество" << "Email"
            << "Телефон" << "Дата рождения" << "Адрес"
            << "Доп. тел. 1" << "Доп. тел. 2";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

void telephone::refreshTable()
{
    ui->tableWidget->setRowCount(0);

    for (size_t i = 0; i < contacts.size(); i++){
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

        string phone = c.getPhone1();
        if (!phone.empty()){
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

void telephone::search_click()
{
    QString query = ui->textEdit->toPlainText().trimmed();

    if (query.isEmpty()){
        contacts = currentLoader->load();
        refreshTable();
        ui->statusbar->showMessage("Показаны все контакты", 2000);
        return;
    }

    vector<Contact> results = currentLoader->find(query.toStdString());
    displaySearchResults(results);

    if (!results.empty()){
        ui->statusbar->showMessage(
            QString("Найдено %1 контакт(ов)").arg(results.size()),
            3000);
    } else {
        ui->statusbar->showMessage("Контакты не найдены", 2000);
    }
}

void telephone::displaySearchResults(const vector<Contact>& results)
{
    ui->tableWidget->setRowCount(0);

    if (results.empty()){
        QMessageBox::information(this, "Результаты поиска",
            "Контакты не найдены.\n\nПопробуйте изменить поисковый запрос.");
        return;
    }

    for (const Contact& c : results){
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

        string phone = c.getPhone1();
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

void telephone::save_click()
{
    if (currentLoader == fileLoader) {
        QFile file("contact.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&file);
            for (const Contact& c : contacts) {
                out << c.getId() << ","
                    << QString::fromStdString(c.getFirstName()) << ","
                    << QString::fromStdString(c.getLastName()) << ","
                    << QString::fromStdString(c.getEmail()) << ","
                    << QString::fromStdString(c.getPhone1()) << ","
                    << QString::fromStdString(c.getMiddleName()) << ","
                    << QString::fromStdString(c.getDate()) << ","
                    << QString::fromStdString(c.getAddress()) << ","
                    << QString::fromStdString(c.getPhone2()) << ","
                    << QString::fromStdString(c.getPhone3()) << "\n";
            }
            file.close();
            ui->statusbar->showMessage("Данные сохранены в файл", 2000);
        }
    } else {
        for (const Contact& c : contacts) {
            currentLoader->save(c);
        }
        ui->statusbar->showMessage("Данные сохранены в базу", 2000);
    }
}

void telephone::out_click()
{
    QApplication::quit();
}

void telephone::show_click()
{
    QMessageBox::about(this, "О программе",
                      "Телефонный справочник\n\n"
                      "Версия 1.0\n\n"
                      "Функции:\n"
                      "- Просмотр всех контактов\n"
                      "- Поиск по всем полям\n"
                      "- Добавление/редактирование/удаление контактов\n"
                      "- Сохранение данных в файл\n"
                      "- Работа с базой данных PostgreSQL\n"
                      "- Валидация вводимых данных\n\n"
                      "© 2025");
}

void telephone::right_click(const QPoint &pos)
{
    QPoint globalPos = ui->tableWidget->mapToGlobal(pos);
    contextMenu->exec(globalPos);
}

void telephone::addContact()
{
    ContactDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted){
        Contact newContact = dialog.getContact();
        newContact.setId(-1);

        currentLoader->save(newContact);
        contacts = currentLoader->load();
        refreshTable();
        ui->statusbar->showMessage("Контакт добавлен", 2000);
    }
}

void telephone::editContact()
{
    int row = ui->tableWidget->currentRow();
    if (row == -1 || row >= (int)contacts.size()){
        QMessageBox::warning(this, "Ошибка", "Выберите контакт для редактирования");
        return;
    }

    Contact contactToEdit = contacts[row];
    ContactDialog dialog(contactToEdit, this);
    if (dialog.exec() == QDialog::Accepted){
        Contact updatedContact = dialog.getContact();
        updatedContact.setId(contactToEdit.getId());

        currentLoader->save(updatedContact);
        contacts = currentLoader->load();
        refreshTable();
        ui->statusbar->showMessage("Контакт обновлен", 2000);
    }
}

void telephone::deleteContact()
{
    int row = ui->tableWidget->currentRow();
    if (row == -1 || row >= (int)contacts.size()){
        QMessageBox::warning(this, "Ошибка", "Выберите контакт для удаления");
        return;
    }

    Contact contactToDelete = contacts[row];
    QString name = QString::fromStdString(contactToDelete.getLastName() + " " +
                                         contactToDelete.getFirstName());

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение",
                                 "Вы уверены, что хотите удалить контакт:\n" + name + "?",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes){
        currentLoader->remove(contactToDelete.getId());
        contacts.erase(contacts.begin() + row);
        refreshTable();
        ui->statusbar->showMessage("Контакт удален", 2000);
    }
}

string telephone::formatPhone(const string &number)
{
    string res;
    for (char c : number){
        if (isdigit(c)){
            res += c;
        }
    }
    if (!res.empty() && res[0] == '7'){
        res[0] = '8';
    }
    return res;
}

void telephone::useFileStorage()
{
    currentLoader = fileLoader;
    contacts = currentLoader->load();
    refreshTable();
    updateStorageMenu();
    ui->statusbar->showMessage("Используется файловое хранилище", 2000);
}

void telephone::useDBStorage()
{
    if (dbLoader->open()) {
        currentLoader = dbLoader;
        contacts = currentLoader->load();
        refreshTable();
        updateStorageMenu();
        ui->statusbar->showMessage("Используется база данных", 2000);
    } else {
        QMessageBox::warning(this, "Ошибка",
            "Не удалось подключиться к базе данных.\n"
            "Проверьте что PostgreSQL запущен и база 'contact' существует.");
    }
}

void telephone::updateStorageMenu()
{
    bool isFile = (currentLoader == fileLoader);
    bool isDB = (currentLoader == dbLoader);

    ui->action_11->setChecked(isFile);
    ui->action_12->setChecked(isDB);
}
