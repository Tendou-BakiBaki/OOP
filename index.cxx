#include <iostream>
#include <regex>
#include "contact.cxx"
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>


using namespace std;

// ВЕКТОР КОНТАКТОВ. Тут крч хранятся все контакты из файлика и добавляемые нами, ехех
vector<Contact> contacts;
// АААААААААААААААА

regex number_mask(R"(^(\+7|8)(\(\d{3}\)\d{3}(\-\d{2}\-\d{2}|\d{7})|\d{10})$)"); // Масочка для номеров. Вродь правильно выглядит. ^ - начало $ - конец
regex fio_mask(R"(^[a-zA-Zа-яА-ЯёЁ][a-zA-Zа-яА-ЯёЁ\d -]*[a-zA-Zа-яА-ЯёЁ\d]$)"); //Маска для фио
regex data_mask(R"(^\d{2}\.\d{2}\.\d{4}$)"); // маска для даты вида 01.01.2001
regex email_mask(R"(^[a-zA-Z0-9]+@[a-zA-Z0-9]+\.[a-zA-Z0-9]+$)"); // маска email

// code: 1 - fio 2 - number 3 - data 4 - email 

bool chek(string text, int code){
    switch (code) {
        case 1:
            return regex_match(text, fio_mask);
        case 2:
            return regex_match(text, number_mask);
        case 3:{
            bool con1 = regex_match(text, data_mask);
            int day = stoi(text.substr(0, 2));
            int month = stoi(text.substr(3, 2));
            int year = stoi(text.substr(6, 4));
            // Проверяем диапазоны
            if (year < 1900 || year > 2025) return false;
            if (month < 1 || month > 12) return false;
            if (day < 1 || day > 31) return false;
            
            // Проверяем число дней в месяце
            if (month == 2) {
                // Февраль
                bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
                if (day > (isLeap ? 29 : 28)) return false; // ? - условие первое если истина, второе если ложь... А то через неделю забуду
            } 
            else 
                if (month == 4 || month == 6 || month == 9 || month == 11) {
                // Месяцы с 30 днями
                    if (day > 30) return false;
                }
            return con1;
            }
        case 4:
            return regex_match(text, email_mask);
        default:
            return false;
    }
}

void clscr() { // Очистка экрана
    cout << "\033[2J\033[1;1H"; // Перенос вниз чтоб красивенько терминал был, как бы очищает)))
}

// ОСНОВНОЕ МЕНЮ

void menu() {
    clscr();
    cout << "@@@ ТЕЛЕФОННЫЙ СПРАВОЧНИК @@@\n\n";
    cout << "1. Показать все контакты\n";
    cout << "2. Добавить контакт\n";
    cout << "3. Поиск контакта\n";
    cout << "4. Редактировать контакт\n";
    cout << "5. Удалить контакт\n";
    cout << "0. Выход\n\n";
}

// -------------------------------------------

// ПОКАЗ КОНТАКТОВ

void showAllContacts() {
    clscr();
    cout << "\n@@@ ВСЕ КОНТАКТЫ (" << contacts.size() << ") @@@" << endl;
    if (contacts.empty()) {
        cout << "Список контактов пуст" << endl;
    }
    else{
        for (size_t i = 0; i < contacts.size(); i++) {
            const Contact& c = contacts[i];
            cout << "\n" << i + 1 << ". ";
            cout << "ФИО:            " << c.getLastName() << " " << c.getFirstName() << " " << c.getMiddleName() << endl;
            cout << "   Email:          " << c.getEmail() << endl;
            cout << "   Номер телефона: " << c.getPhone1();
            if (!c.getPhone2().empty() || !c.getPhone3().empty()) {
                cout << "   (доп: ";
                if (!c.getPhone2().empty()) cout << c.getPhone2() << ", ";
                if (!c.getPhone3().empty()) cout << c.getPhone3();
                cout << ")";
            }
                cout << endl;
            cout << "   Дата рождения:  " << c.getDate() << endl;
            cout << "   Адрес:          " << c.getAddress() << endl;
        }
        
        cout << endl;
    }
    cout << "Нажмите Enter для возврата...\n";
    cin.get();
}

// ----------------------------

string form(string number){ // приводим номер к виду 8090... Это для удобства поиска и чтоб в одном формате отображались
    string res;
    for (char c : number) {
        if (isdigit(c)) {
            res += c;
        }
    }
    if (res[0] == '7'){
        res[0] = '8';
    }
    return res;
}

// ДОБАВЛЕНИЕ КОНТАКТА

void addContact() {
    clscr();
    cout << "@@@ ДОБАВЛЕНИЕ КОНТАКТА @@@\n\n";
    
    string fn, ln, mn, email, phone1, phone2, phone3, date, adr;
    cout << "         ОБЯЗАТЕЛЬНЫЕ ПОЛЯ" << endl << endl;
    // Ввод имени с проверкой
    do {
        cout << "Имя*: ";
        getline(cin, fn);
        if (!chek(fn, 1)) {
            cout << "Неверный формат имени!" << endl;
        }
    } while (!chek(fn, 1));
    
    // Ввод фамилии с проверкой
    do {
        cout << "Фамилия*: ";
        getline(cin, ln);
        if (!chek(ln, 1)) {
            cout << "Неверный формат фамилии!" << endl;
        }
    } while (!chek(ln, 1));
    
    // Ввод email с проверкой
    do {
        cout << "Email*: ";
        getline(cin, email);
        if (!chek(email, 4)) {
            cout << "Неверный формат email!" << endl;
        }
    } while (!chek(email, 4));
    
    // Ввод телефона с проверкой
    do {
        cout << "Телефон*: ";
        getline(cin, phone1);
        if (!chek(phone1, 2)) {
            cout << "Неверный формат телефона!" << endl;
        }
    } while (!chek(phone1, 2));
    phone1 = form(phone1);
    
    cout << endl << "         ДОПОЛНИТЕЛЬНЫЕ ПОЛЯ" << endl << endl;
    
    // Отчество необязательно, но с проверкой если заполнено
    while(1){
        cout << "Отчество: ";
        getline(cin, mn);
        if (!mn.empty() && !chek(mn, 1)) {
            cout << "Неверный формат отчества!" << endl;
        } 
        else {
            break;
        }
    };
    
    // Дата рождения с проверкой как и отчество
    while(1){
        cout << "Дата рождения (дд.мм.гггг): ";
        getline(cin, date);
        if (!date.empty() && !chek(date, 3)) {
            cout << "Неверная дата рождения!" << endl;
        } 
        else {
            break;
        }
    };
    
    // Адрес проверки не надо
    cout << "Адрес: ";
    getline(cin, adr);
    
    // Дополнительные телефоны с проверкой
    while(1){
        cout << "Доп. телефон 2: ";
        getline(cin, phone2);
        if (!phone2.empty() && !chek(phone2, 2)) {
            cout << "Неверный формат телефона!" << endl;
        } 
        else {
            break;
        }
    };
    
    while(1){
        cout << "Доп. телефон 3: ";
        getline(cin, phone3);
        if (!phone3.empty() && !chek(phone3, 2)) {
            cout << "Неверный формат телефона!" << endl;
        } 
        else {
            break;
        }
    };
    
    // Создание контакта
    Contact newContact(fn, ln, email, phone1, mn, date, adr, phone2, phone3);
    contacts.push_back(newContact);
    cout << "Контакт успешно добавлен!" << endl;
    cout << "Нажмите Enter для возврата...\n";
    cin.get();
}

// ------------------------------

void findd(string f, int zn){
    vector<int> result;
    for (size_t i = 0; i < contacts.size(); i++) {
        const Contact& c = contacts[i];
        switch (zn){
            case 1:{
                if (c.getFirstName() == f){
                    result.push_back(i);
                }
                break;}
            case 2:{
                if (c.getLastName() == f){
                    result.push_back(i);
                }
                break;}
            case 3:
                {
                if (c.getPhone1() == f || c.getPhone2() == f || c.getPhone3() == f){
                    result.push_back(i);
                }
                break;}
            case 4:
                {
                if (c.getEmail() == f){
                    result.push_back(i);
                }
                break;}
            default:
                break;
        }
    }
    for (size_t i = 0; i < result.size(); i++) {
        const Contact& c = contacts[result[i]];
        cout << i + 1 << ". ";
        cout << "ФИО:            " << c.getLastName() << " " << c.getFirstName() << " " << c.getMiddleName() << endl;
        cout << "   Email:          " << c.getEmail() << endl;
        cout << "   Номер телефона: " << c.getPhone1();
        if (!c.getPhone2().empty() || !c.getPhone3().empty()) {
            cout << "   (доп: ";
            if (!c.getPhone2().empty()) cout << c.getPhone2() << ", ";
            if (!c.getPhone3().empty()) cout << c.getPhone3();
            cout << ")";
        }
        cout << endl;
        cout << "   Дата рождения:  " << c.getDate() << endl;
        cout << "   Адрес:          " << c.getAddress() << endl;
    }
}

// ПОИСК КОНТАКТОВ

void searchContact() {
    string cho1, cho;
    string qst;
    while(1){
        clscr();
        cout << "@@@ ПОИСК КОНТАКТА @@@\n\n";
        cout << "1. По имени" << endl;
        cout << "2. По фамилии" << endl;
        cout << "3. По номеру телефона" << endl;
        cout << "4. По email" << endl;
        cout << "0. Назад" << endl << endl;
        int ret = 0;
        cout << "Выберите поле для поиска: ";
        cin >> cho1;
        for (char c : cho1) {
            if(!isdigit(c)){
                cout << "\nВведите число от 0 до 4!" << endl << endl;
                ret = 1;
                break;
            }
        }

        if (ret == 1){
            continue;
        }

        int cho = stoi(cho1); 
        
        if (cho > 4 || cho < 0){
            cout << "\nНеверный выбор поля!" << endl;
            continue;
        }
        if (cho == 0){
            break;
        }
        cout << "Введите поисковый запрос: ";
        cin.ignore();
        getline(cin, qst);
        cout << endl;
        findd(qst, cho);
        cout << "\nНажмите Enter для продолжения...\n";
        cin.get();
    }
    cout << "Нажмите Enter для возврата...\n";
    cin.get();
}

//------------------------------------

// РЕДАКТОР КОНТАКТА

void EDIT(int i){
    Contact& contact = contacts[i - 1]; 
    clscr();
    while (true) {
        
        int ret = 0;
        cout << "@@@ РЕДАКТИРОВАНИЕ КОНТАКТА @@@" << endl << endl;
        cout << "1. Имя: " << contact.getFirstName() << endl;
        cout << "2. Фамилия: " << contact.getLastName() << endl;
        cout << "3. Отчество: " << contact.getMiddleName() << endl;
        cout << "4. Email: " << contact.getEmail() << endl;
        cout << "5. Телефон 1: " << contact.getPhone1() << endl;
        cout << "6. Телефон 2: " << contact.getPhone2() << endl;
        cout << "7. Телефон 3: " << contact.getPhone3() << endl;
        cout << "8. Дата рождения: " << contact.getDate() << endl;
        cout << "9. Адрес: " << contact.getAddress() << endl;
        cout << "0. Завершить редактирование" << endl;
        
        string cho1;
        cout << "\nВыберите поле для редактирования: ";
        cin >> cho1;
        for (char c : cho1) {
            if(!isdigit(c)){
                cout << "Введите число от 0 до 9" << endl;
                ret = 1;
                break;
            }
        }

        if (ret == 1){
            cout << "\nНеверный выбор поля!" << endl;
            continue;
        }

        int cho = stoi(cho1); 
        
        if (cho > 9 || cho < 0){
            cout << "\nНеверный выбор поля!" << endl;
            continue;
        }

        if (cho == 0) {
            cout << "\nРедактирование завершено" << endl << endl;
            return;
        }
        
        string nv;
        cout << "Введите новое значение: ";
        cin >> nv;
        cout << endl;
        switch (cho) {
            case 1:
                if (chek(nv, 1)){
                    contact.setFirstName(nv);
                    cout << "Имя изменено" << endl;
                }
                else{
                    cout << "Неверный формат поля!" << endl;
                }
                break;
            case 2:
                if (chek(nv, 1)){
                    contact.setLastName(nv);
                    cout << "Фамилия изменена" << endl;
                }
                else{
                    cout << "Неверный формат поля!" << endl;
                }
                break;
            case 3:
                if (chek(nv, 1)){
                    contact.setMiddleName(nv);
                    cout << "Отчество изменено" << endl;
                }
                else{
                    cout << "Неверный формат поля!" << endl;
                }
                break;
            case 4:
                if (chek(nv, 4)){
                    contact.setEmail(nv);
                    cout << "Email изменен" << endl;
                }
                else{
                    cout << "Неверный формат поля!" << endl;
                }
                break;
            case 5:
                if (chek(nv, 2)){
                    contact.setPhone1(nv);
                    cout << "Телефон 1 изменен" << endl;
                }
                else{
                    cout << "Неверный формат поля!" << endl;
                }
                break;
            case 6:
                if (chek(nv, 2)){
                    contact.setPhone2(nv);
                    cout << "Телефон 2 изменен" << endl;
                }
                else{
                    cout << "Неверный формат поля!" << endl;
                }
                break;
            case 7:
                if (chek(nv, 2)){
                    contact.setPhone3(nv);
                    cout << "Телефон 3 изменен" << endl;
                }
                else{
                    cout << "Неверный формат поля!" << endl;
                }
                break;
            case 8:
                if (chek(nv, 3)){
                    contact.setDate(nv);
                    cout << "Дата рождения изменена" << endl;
                }
                else{
                    cout << "Неверный формат поля!" << endl;
                }
                break;
            case 9:
                contact.setAddress(nv);
                cout << "Адрес изменен" << endl;
                break;
            default:
                break;
        }
    }
}

void editContact() {
    clscr();
    cout << "@@@ РЕДАКТИРОВАНИЕ КОНТАКТА @@@\n\n";
    if (contacts.empty()) {
        cout << "Список контактов пуст" << endl;
    }
    else{
        while(1){
            clscr();
            cout << "@@@ РЕДАКТИРОВАНИЕ КОНТАКТА @@@\n";
            int le = 0;
            for (size_t i = 0; i < contacts.size(); i++) {
                const Contact& c = contacts[i];
                cout << "\n" << i + 1 << ". ";
                cout << "ФИО:            " << c.getLastName() << " " << c.getFirstName() << " " << c.getMiddleName();
                cout << endl;
                cout << "   Номер телефона: " << c.getPhone1();
                le++;
            }
            cout << endl << endl;
            string choice;
            int cont;
            int ret = 0;
            ret = 0;
            cout << "Выберите контакт для редактирования (для выхода введите 0): ";
            cin >> choice;
            if (choice == "0"){
                return;
            }
            for (char c : choice) {
                if(!isdigit(c)){
                    cout << "Введите число от 1 до " << le << endl;
                    ret = 1;
                    break;
                }
            }
            if (ret == 1){
                continue;
            }
            cont = stoi(choice);
            if (cont < 1 || cont > le){
                cout << "Введите число от 1 до " << le << endl;
                continue;
            }
            EDIT(cont);
        };
    }
    cout << "Нажмите Enter для возврата...\n";
    cin.get();
}

// ----------------------------------------

// УДАЛЕНИЕ КОНТАКТА

void deleteContact() {
    clscr();
    cout << "@@@ УДАЛЕНИЕ КОНТАКТА @@@\n\n";
    if (contacts.empty()) {
        cout << "Список контактов пуст" << endl;
    }
    else{
        cout << "Выберите контакт для удаления (для выхода введите 0)" << endl;
        int le = 0;
        for (size_t i = 0; i < contacts.size(); i++) {
            const Contact& c = contacts[i];
            cout << "\n" << i + 1 << ". ";
            cout << "ФИО:            " << c.getLastName() << " " << c.getFirstName() << " " << c.getMiddleName();
            cout << endl;
            cout << "   Номер телефона: " << c.getPhone1();
            le++;
        }
        cout << endl << endl;
        string choice;
        int cont;
        int ret = 0;
        while(1){
            ret = 0;
            cin >> choice;
            if (choice == "0"){
                return;
            }
            for (char c : choice) {
                if(!isdigit(c)){
                    cout << "Введите число от 1 до " << le << endl;
                    ret = 1;
                    break;
                }
            }
            if (ret == 1){
                continue;
            }
            cont = stoi(choice);
            if (cont < 1 || cont > le){
                cout << "Введите число от 1 до " << le << endl;
                continue;
            }
            cout << "Вы уверены, что хотите удалить контакт: " 
            << contacts[cont - 1].getLastName() << " " << contacts[cont - 1].getFirstName() << " " << contacts[cont - 1].getMiddleName()
            << "?" << endl;
            cout << "Если да - введите y, если нет - n" << endl;
            string ch; 
            while(1){
                cin >> ch;
                if (ch != "n" && ch != "y") continue;
                if (ch == "y"){
                    contacts.erase(contacts.begin() + cont);
                    return;
                }
                if (ch == "n"){
                    cout << "Удаление отменено, сделайте выбор снова или введите 0" << endl;
                    break;
                }
            }
        };
    }
    cout << "Нажмите Enter для возврата...\n";
    cin.get();
}

// ---------------------------------------

int getMenuChoice() {
    int choice;
    cout << "Выберите действие: ";
    while (!(cin >> choice)) {
        cout << "Ошибка! Введите число: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Иначе вечно зацикливается
    }
    return choice;
}

// В самом начале считываем сохраненый файл. Поля идут так: Имя, Фамилия, email, номер, отчество,дата рождения, адрес, номер2, номер3
// Если в пользователь не вводил необязательные поля то там просто пустота и идет чет тип такого: блабла,,блабла 

void read(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()){ // проверяем все ли ок с файликом
        cout << "Файл " << filename << " не найден" << endl;
        return;
    }
    
    string line;
    int nol = 0;
    
    while (getline(file, line)){ // построчно читаем файл 
        stringstream ss(line);
        string fn, ln, email, phone1, mn, date, adrs, phone2, phone3;
        // Читаем все поля по порядку
        getline(ss, fn, ',');
        getline(ss, ln, ',');
        getline(ss, email, ',');
        getline(ss, phone1, ',');
        getline(ss, mn, ',');
        getline(ss, date, ',');
        getline(ss, adrs, ',');
        getline(ss, phone2, ',');
        getline(ss, phone3, ',');
        contacts.push_back(Contact(fn, ln, email, phone1, mn, date, adrs, phone2, phone3)); // Добавляем контакт в вектор
    }
    
    file.close();
}




int main() {
    int choice;
    read("cont.txt");
    do {
        menu();
        choice = getMenuChoice();
        cin.ignore(); // При вводе у нас остается \n потому мы его игнорируем чтобы cin.get() его не схавал в функции
        switch (choice) {
            case 1:
                showAllContacts();
                break;
            case 2:
                addContact();
                break;
            case 3:
                searchContact();
                break;
            case 4:
                editContact();
                break;
            case 5:
                deleteContact();
                break;
            case 0:
                clscr();
                cout << "До свидания!\n";
                break;
            default:
                cout << "Введите верное значение.\n";
                cout << "Нажмите Enter для продолжения...\n";
                cin.get(); // такие штуки чтоб символ только считывать в нашем случае энтер. Хз зачем это пишу ахах
                break;
        }
        
    } while (choice != 0);
    
    ofstream file("cont.txt", ios::trunc); // ios::trunc - очистить файл
    
    // Записываем все контакты из вектора в файл
    for (const Contact& c : contacts) {
        file << c.getFirstName() << ","
             << c.getLastName() << ","
             << c.getEmail() << ","
             << c.getPhone1() << ","
             << c.getMiddleName() << ","
             << c.getDate() << ","
             << c.getAddress() << ","
             << c.getPhone2() << ","
             << c.getPhone3() << "\n";
    }
    
    file.close();

    return 0;
}


