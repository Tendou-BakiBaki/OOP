#ifndef FILEDATA_H
#define FILEDATA_H

#include "dataload.h"
#include <QString>

using namespace std;

class FileData : public DataLoad {
public:
    FileData(const QString& file = "contact.txt");

    vector<Contact> load() override;
    void save(const Contact& contact) override;
    void remove(int id) override;
    vector<Contact> find(const string& text) override;

private:
    QString filename;
    vector<Contact> contacts;

    void writeFile();
};

#endif
