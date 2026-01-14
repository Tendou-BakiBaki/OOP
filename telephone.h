#ifndef TELEPHONE_H
#define TELEPHONE_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <vector>
#include <string>
#include "contact.h"
#include "contactdialog.h"
#include "dataload.h"
#include "filedata.h"
#include "dbdata.h"

using namespace std;

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
    void search_click();
    void save_click();
    void out_click();
    void show_click();

    void right_click(const QPoint &pos);
    void addContact();
    void editContact();
    void deleteContact();

    void useFileStorage();
    void useDBStorage();

private:
    Ui::telephone *ui;

    DataLoad* currentLoader;
    FileData* fileLoader;
    DBData* dbLoader;

    vector<Contact> contacts;

    void setupTable();
    void refreshTable();
    void displaySearchResults(const vector<Contact>& results);

    string formatPhone(const string &number);
    void updateStorageMenu();

    QMenu *contextMenu;
    QAction *addAction;
    QAction *editAction;
    QAction *deleteAction;

    QMenu *storageMenu;
    QAction *fileStorageAction;
    QAction *dbStorageAction;
};

#endif
