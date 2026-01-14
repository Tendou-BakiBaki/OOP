#include "dbdata.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <iostream>
#include <algorithm>

using namespace std;

DBData::DBData() {
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("contact");
    db.setUserName("postgres");
    db.setPassword("");
}

DBData::~DBData() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DBData::open() {
    if (!db.isOpen()) {
        if (!db.open()) {
            cerr << "Ошибка подключения: " << db.lastError().text().toStdString() << endl;
            return false;
        }

        QSqlQuery query(db);
        if (!query.exec(
            "CREATE TABLE IF NOT EXISTS contacts ("
            "id SERIAL PRIMARY KEY,"
            "first_name VARCHAR(100) NOT NULL,"
            "last_name VARCHAR(100) NOT NULL,"
            "email VARCHAR(100) NOT NULL,"
            "phone1 VARCHAR(20) NOT NULL,"
            "middle_name VARCHAR(100),"
            "birth_date VARCHAR(20),"
            "address VARCHAR(200),"
            "phone2 VARCHAR(20),"
            "phone3 VARCHAR(20))"
        )) {
            cerr << "Ошибка создания таблицы: " << query.lastError().text().toStdString() << endl;
            return false;
        }
    }
    return true;
}

bool DBData::isOpen() const {
    return db.isOpen();
}

vector<Contact> DBData::load() {
    vector<Contact> loadedContacts;

    if (!db.isOpen()) {
        cerr << "База данных не открыта" << endl;
        return loadedContacts;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT * FROM contacts ORDER BY last_name, first_name")) {
        cerr << "Ошибка загрузки: " << query.lastError().text().toStdString() << endl;
        return loadedContacts;
    }

    while (query.next()) {
        int id = query.value("id").toInt();
        string firstName = query.value("first_name").toString().toStdString();
        string lastName = query.value("last_name").toString().toStdString();
        string email = query.value("email").toString().toStdString();
        string phone1 = query.value("phone1").toString().toStdString();
        string middleName = query.value("middle_name").toString().toStdString();
        string date = query.value("birth_date").toString().toStdString();
        string address = query.value("address").toString().toStdString();
        string phone2 = query.value("phone2").toString().toStdString();
        string phone3 = query.value("phone3").toString().toStdString();

        loadedContacts.push_back(Contact(id, firstName, lastName, email, phone1,
                                         middleName, date, address, phone2, phone3));
    }

    cout << "Загружено контактов из БД: " << loadedContacts.size() << endl;
    return loadedContacts;
}

void DBData::save(const Contact& contact) {
    if (!db.isOpen()) {
        cerr << "База данных не открыта" << endl;
        return;
    }

    if (contact.getId() > 0) {
        QSqlQuery query(db);
        query.prepare(
            "UPDATE contacts SET "
            "first_name = ?, last_name = ?, email = ?, phone1 = ?, "
            "middle_name = ?, birth_date = ?, address = ?, phone2 = ?, phone3 = ? "
            "WHERE id = ?"
        );

        query.addBindValue(QVariant(QString::fromStdString(contact.getFirstName())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getLastName())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getEmail())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getPhone1())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getMiddleName())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getDate())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getAddress())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getPhone2())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getPhone3())));
        query.addBindValue(QVariant(contact.getId()));

        if (!query.exec()) {
            cerr << "Ошибка обновления: " << query.lastError().text().toStdString() << endl;
        }
    } else {
        QSqlQuery query(db);
        query.prepare(
            "INSERT INTO contacts "
            "(first_name, last_name, email, phone1, middle_name, birth_date, address, phone2, phone3) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) RETURNING id"
        );

        query.addBindValue(QVariant(QString::fromStdString(contact.getFirstName())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getLastName())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getEmail())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getPhone1())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getMiddleName())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getDate())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getAddress())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getPhone2())));
        query.addBindValue(QVariant(QString::fromStdString(contact.getPhone3())));

        if (query.exec() && query.next()) {
            int newId = query.value(0).toInt();
            const_cast<Contact&>(contact).setId(newId);
        } else {
            cerr << "Ошибка добавления: " << query.lastError().text().toStdString() << endl;
        }
    }
}

void DBData::remove(int id) {
    if (!db.isOpen()) {
        cerr << "База данных не открыта" << endl;
        return;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM contacts WHERE id = ?");
    query.addBindValue(QVariant(id));

    if (!query.exec()) {
        cerr << "Ошибка удаления: " << query.lastError().text().toStdString() << endl;
    }
}

vector<Contact> DBData::find(const string& text) {
    vector<Contact> results;

    if (!db.isOpen()) {
        cerr << "База данных не открыта" << endl;
        return results;
    }

    QString searchPattern = "%" + QString::fromStdString(text) + "%";
    QVariant searchVariant = QVariant(searchPattern);

    QSqlQuery query(db);
    query.prepare(
        "SELECT * FROM contacts WHERE "
        "first_name ILIKE ? OR "
        "last_name ILIKE ? OR "
        "middle_name ILIKE ? OR "
        "email ILIKE ? OR "
        "phone1 ILIKE ? OR "
        "phone2 ILIKE ? OR "
        "phone3 ILIKE ? "
        "ORDER BY last_name, first_name"
    );

    for (int i = 0; i < 7; i++) {
        query.addBindValue(searchVariant);
    }

    if (!query.exec()) {
        cerr << "Ошибка поиска: " << query.lastError().text().toStdString() << endl;
        return results;
    }

    while (query.next()) {
        int id = query.value("id").toInt();
        string firstName = query.value("first_name").toString().toStdString();
        string lastName = query.value("last_name").toString().toStdString();
        string email = query.value("email").toString().toStdString();
        string phone1 = query.value("phone1").toString().toStdString();
        string middleName = query.value("middle_name").toString().toStdString();
        string date = query.value("birth_date").toString().toStdString();
        string address = query.value("address").toString().toStdString();
        string phone2 = query.value("phone2").toString().toStdString();
        string phone3 = query.value("phone3").toString().toStdString();

        results.push_back(Contact(id, firstName, lastName, email, phone1,
                                  middleName, date, address, phone2, phone3));
    }

    return results;
}
