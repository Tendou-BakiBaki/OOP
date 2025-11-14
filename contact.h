#ifndef CONTACT_H // Это штука чтобы проверить что они не вставляются несколько раз
#define CONTACT_H

#include <string>
#include <vector>

using namespace std;

class Contact
{
public:
    // Конструктор по умолчанию
    Contact();
    
    // Полный конструктор
    Contact(const string& firstName, const string& lastName, 
            const string& email, const string& phone,
            const string& middleName, const string& date,
            const string& address, const string& phone2,
            const string& phone3);
    // Сеттеры
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
    string firstName_;     // Имя
    string lastName_;      // Фамилия  
    string email_;         // Email
    string phone1_;        // Основной номер телефона
    string middleName_;    // Отчество
    string date_;     // Дата рождения
    string address_;       // Адрес
    string phone2_;        // Номер 2
    string phone3_;        // Номер 3


};

#endif // CONTACT_H