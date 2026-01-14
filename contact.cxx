#include "contact.h"

using namespace std;

Contact::Contact()
    : id_(-1)
    , firstName_("")
    , lastName_("")
    , email_("")
    , phone1_("")
    , middleName_("")
    , date_("")
    , address_("")
    , phone2_("")
    , phone3_("")
{}

Contact::Contact(int id, const string& firstName, const string& lastName,
                const string& email, const string& phone,
                const string& middleName, const string& date,
                const string& address, const string& phone2,
                const string& phone3)
    : id_(id)
    , firstName_(firstName)
    , lastName_(lastName)
    , email_(email)
    , phone1_(phone)
    , middleName_(middleName)
    , date_(date)
    , address_(address)
    , phone2_(phone2)
    , phone3_(phone3)
{}

Contact::~Contact(){}

// Сеттеры
void Contact::setId(int id) { id_ = id; }
void Contact::setFirstName(const string& firstName) { firstName_ = firstName; }
void Contact::setLastName(const string& lastName) { lastName_ = lastName; }
void Contact::setEmail(const string& email) { email_ = email; }
void Contact::setPhone1(const string& phone) { phone1_ = phone; }
void Contact::setMiddleName(const string& middleName) { middleName_ = middleName; }
void Contact::setDate(const string& date) { date_ = date; }
void Contact::setAddress(const string& address) { address_ = address; }
void Contact::setPhone2(const string& phone) { phone2_ = phone; }
void Contact::setPhone3(const string& phone) { phone3_ = phone; }

// Геттеры
int Contact::getId() const { return id_; }
string Contact::getFirstName() const { return firstName_; }
string Contact::getLastName() const { return lastName_; }
string Contact::getEmail() const { return email_; }
string Contact::getPhone1() const { return phone1_; }
string Contact::getMiddleName() const { return middleName_; }
string Contact::getDate() const { return date_; }
string Contact::getAddress() const { return address_; }
string Contact::getPhone2() const { return phone2_; }
string Contact::getPhone3() const { return phone3_; }
