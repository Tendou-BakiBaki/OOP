#ifndef TELEPHONE_H
#define TELEPHONE_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QMessageBox>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include "contact.h"
#include "contactdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class telephone;
}
QT_END_NAMESPACE

class telephone : public QMainWindow
{
    Q_OBJECT

public:
    telephone(QWidget *parent = nullptr);
    ~telephone();

private slots:
    void on_pushButton_clicked();              // Поиск
    void on_action_2_triggered();              // Сохранить
    void on_action_4_triggered();              // Выйти
    void on_action_5_triggered();              // Описание приложения

    void on_tableWidget_customContextMenuRequested(const QPoint &pos);
    void addContact();                         // Добавить контакт
    void editContact();                        // Редактировать контакт
    void deleteContact();                      // Удалить контакт

private:
    Ui::telephone *ui;
    std::vector<Contact> contacts;             // Вектор контактов


    void setupTable();                         // Настройка таблицы
    void refreshTable();                       // Обновление таблицы
    void loadContacts();                       // Загрузка контактов из файла
    void saveContacts();                       // Сохранение контактов в файл
    void displaySearchResults(const std::vector<int>& results);

    // нужна для поиска
    std::string formatPhone(const std::string &number);

    // Контекстное меню
    QMenu *contextMenu;
    QAction *addAction;
    QAction *editAction;
    QAction *deleteAction;
};

#endif // TELEPHONE_H
