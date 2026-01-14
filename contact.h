#ifndef CONTACT_H
#define CONTACT_H

#include <string>

using namespace std;

class Contact
{
public:
    Contact();
    Contact(int id, const string& firstName, const string& lastName,
            const string& email, const string& phone,
            const string& middleName, const string& date,
            const string& address, const string& phone2,
            const string& phone3);

    // Сеттеры
    void setId(int id);
    void setFirstName(const string& firstName);
    void setLastName(const string& lastName);
    void setEmail(const string& email);
    void setPhone1(const string& phone);
    void setMiddleName(const string& middleName);
    void setDate(const string& date);
    void setAddress(const string& address);
    void setPhone2(const string& phone);
    void setPhone3(const string& phone);

    // Геттеры
    int getId() const;
    string getFirstName() const;
    string getLastName() const;
    string getEmail() const;
    string getPhone1() const;
    string getMiddleName() const;
    string getDate() const;
    string getAddress() const;
    string getPhone2() const;
    string getPhone3() const;

    ~Contact();

private:
    int id_;
    string firstName_;
    string lastName_;
    string email_;
    string phone1_;
    string middleName_;
    string date_;
    string address_;
    string phone2_;
    string phone3_;
};

#endif
