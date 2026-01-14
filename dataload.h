#ifndef DATALOAD_H
#define DATALOAD_H

#include <vector>
#include <string>
#include "contact.h"

using namespace std;

class DataLoad {
public:
    virtual ~DataLoad() {}

    virtual vector<Contact> load() = 0;
    virtual void save(const Contact& contact) = 0;
    virtual void remove(int id) = 0;
    virtual vector<Contact> find(const string& text) = 0;
};

#endif
