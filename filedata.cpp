#include "filedata.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

FileData::FileData(const QString& file) : filename(file) {
    cout << "FileData создан для файла: " << filename.toStdString() << endl;
    load();
}

vector<Contact> FileData::load() {
    cout << "FileData::load() из файла: " << filename.toStdString() << endl;

    vector<Contact> loadedContacts;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cout << "Файл не найден, будет создан новый" << endl;
        return loadedContacts;
    }

    QTextStream in(&file);
    int lineNum = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNum++;

        QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty()) continue;

        QStringList parts = trimmedLine.split(',');

        while (parts.size() < 10) {
            parts.append("");
        }

        int id = parts[0].toInt();
        string fn = parts[1].toStdString();
        string ln = parts[2].toStdString();
        string email = parts[3].toStdString();
        string phone1 = parts[4].toStdString();
        string mn = parts[5].toStdString();
        string date = parts[6].toStdString();
        string adrs = parts[7].toStdString();
        string phone2 = parts[8].toStdString();
        string phone3 = parts[9].toStdString();

        loadedContacts.push_back(Contact(id, fn, ln, email, phone1, mn, date, adrs, phone2, phone3));
    }

    file.close();
    cout << "Загружено контактов из файла: " << loadedContacts.size() << endl;

    contacts = loadedContacts;
    return loadedContacts;
}

void FileData::save(const Contact& contact) {
    cout << "FileData::save() для контакта ID: " << contact.getId() << endl;

    if (contact.getId() > 0) {
        for (auto& c : contacts) {
            if (c.getId() == contact.getId()) {
                c = contact;
                writeFile();
                return;
            }
        }
    }

    int maxId = 0;
    for (const auto& c : contacts) {
        if (c.getId() > maxId) {
            maxId = c.getId();
        }
    }

    Contact newContact = contact;
    newContact.setId(maxId + 1);

    contacts.push_back(newContact);
    writeFile();
}

void FileData::remove(int id) {
    cout << "FileData::remove() ID: " << id << endl;

    for (auto it = contacts.begin(); it != contacts.end(); ++it) {
        if (it->getId() == id) {
            contacts.erase(it);
            writeFile();
            return;
        }
    }
}

vector<Contact> FileData::find(const string& text) {
    if (text.empty()) {
        return contacts;
    }

    vector<Contact> results;
    string searchText = text;
    transform(searchText.begin(), searchText.end(), searchText.begin(), ::tolower);

    for (const auto& c : contacts) {
        string firstName = c.getFirstName();
        string lastName = c.getLastName();
        string middleName = c.getMiddleName();
        string email = c.getEmail();
        string phone1 = c.getPhone1();
        string phone2 = c.getPhone2();
        string phone3 = c.getPhone3();

        auto format = [](const string& num) -> string {
            string res;
            for (char ch : num) if (isdigit(ch)) res += ch;
            if (!res.empty() && res[0] == '7') res[0] = '8';
            return res;
        };

        phone1 = format(phone1);
        phone2 = format(phone2);
        phone3 = format(phone3);

        transform(firstName.begin(), firstName.end(), firstName.begin(), ::tolower);
        transform(lastName.begin(), lastName.end(), lastName.begin(), ::tolower);
        transform(middleName.begin(), middleName.end(), middleName.begin(), ::tolower);
        transform(email.begin(), email.end(), email.begin(), ::tolower);

        if (firstName.find(searchText) != string::npos ||
            lastName.find(searchText) != string::npos ||
            middleName.find(searchText) != string::npos ||
            email.find(searchText) != string::npos ||
            phone1.find(searchText) != string::npos ||
            phone2.find(searchText) != string::npos ||
            phone3.find(searchText) != string::npos) {
            results.push_back(c);
        }
    }

    return results;
}

void FileData::writeFile() {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        cerr << "Ошибка записи в файл" << endl;
        return;
    }

    QTextStream out(&file);
    for (const auto& c : contacts) {
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
}
