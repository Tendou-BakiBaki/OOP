#ifndef DBDATA_H
#define DBDATA_H

#include "dataload.h"
#include <QSqlDatabase>

using namespace std;

class DBData : public DataLoad {
public:
    DBData();
    ~DBData();

    bool open();
    bool isOpen() const;

    vector<Contact> load() override;
    void save(const Contact& contact) override;
    void remove(int id) override;
    vector<Contact> find(const string& text) override;

private:
    QSqlDatabase db;
};

#endif
