#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <string>
#include <regex>

using namespace std;
#define PATH "data.dat"

struct Client {
    char city[20];
    char fullName[30];
    char phoneNumber[15];
    float loanBody;
    float totalAmount;
    float monthlyPayment;
    char loanDate[11];
    bool isPaid;
};

// Перевірка існування файлу
bool FileExists(const string& path) {
    ifstream file(path);
    return file.good();
}
// Створення файлу, якщо файл відсутній
void EnsureFileExists(const string& path) {
    if (!FileExists(path)) {
        ofstream file(path, ios::binary);
        file.close();
    }
}
// Запис клієнта у файл
void WriteClient(const string& path, const Client& client) {
    ofstream file(path, ios::binary | ios::app);
    if (!file) { // Перевірка, чи файл відкрився успішно
        cerr << "Error: Could not open file for writing!" << endl;
        return;
    }
    file.write((const char*)(&client), sizeof(Client));
    file.close();
}
// Перевірка наявності записів у файлі
bool HasRecords(const string& path) {
    ifstream file(path, ios::binary);
    file.seekg(0, ios::end);
    return file.tellg() > 0;
}
// Перевірка на унікальність номера телефону
bool IsUniquePhoneNumber(const string& path, const char* phoneNumber) {
    ifstream file(path, ios::binary);
    Client current;
    while (file.read((char*)(&current), sizeof(Client))) {
        if (strcmp(current.phoneNumber, phoneNumber) == 0) {
            return false;
        }
    }
    return true;
}
// Введення суми кредиту з перевіркою (діапазон від 1,000 до 1,000,000)
void InputLoanBody(float& loanBody) {
    regex validAmountPattern("^\\d+(\\.\\d+)?$"); // Регулярний вираз для числа (ціле або з плаваючою комою)
    string input;

    while (true) {
        cout << "Enter the \"Sum of credit\" (from 1,000 to 1,000,000) -  ";
        getline(cin, input);

        // Перевірка на коректність введеного числа
        if (!regex_match(input, validAmountPattern)) {
            cout << "Error: You must enter a valid number!" << endl;
            continue;
        }

        // Конвертуємо введений рядок в число
        loanBody = stof(input);

        // Перевірка діапазону
        if (loanBody >= 1000 && loanBody <= 1000000) {
            break; // Якщо число в межах діапазону, виходимо з циклу
        }
        else {
            cout << "Error: Amount is not in the acceptable range!" << endl;
        }
    }
}


// Перевірка на правильність вводу дати
bool IsValidDate(const char date[]) {
    bool isLeap;

    // Перевірка довжини (формат DD.MM.YYYY повинен мати рівно 10 символів)
    if (strlen(date) != 10) {
        return false;
    }

    // Перевірка на правильні роздільники (дата повинна мати '.' на 3-й і 6-й позиціях)
    if (date[2] != '.' || date[5] != '.') {
        return false;
    }

    // Отримання дня, місяця та року
    int day = atoi(string(date, 2).c_str());
    int month = atoi(string(date + 3, 2).c_str());
    int year = atoi(string(date + 6, 4).c_str());

    // Перевірка діапазонів для дня, місяця та року
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900) {
        return false;
    }

    // Перевірка днів у кожному місяці
    switch (month) {
    case 4: case 6: case 9: case 11:
        if (day > 30) return false;
        break;
    case 2:
        // Визначення, чи є рік високосним
        isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeap && day > 29) return false;
        if (!isLeap && day > 28) return false;
        break;
    default:
        if (day > 31) return false;
    }

    return true;
}
// Функція для перевірки довжини рядка
bool isValidLength(const string& str, size_t maxLength) {
    return str.length() <= maxLength;
}
// Функція форматування назви міста
string formatCityName() {
    const size_t maxCityLength = 16; // Максимальна довжина назви міста
    string result;

    while (true) {
        getline(cin, result);

        // Перевірка довжини
        if (!isValidLength(result, maxCityLength)) {
            cout << "Error: The city name cannot exceed "
                << maxCityLength << " characters. Try again: ";
            continue; // Повертаємось до повторного вводу
        }

        // Форматування: кожне слово починається з великої літери
        bool capitalize = true;
        bool isValid = true; // Змінна для відслідковування валідності вводу
        for (size_t i = 0; i < result.length(); ++i) {
            if (isalpha(result[i])) {
                result[i] = capitalize ? toupper(result[i]) : tolower(result[i]);
                capitalize = false;
            }
            else if (result[i] == ' ' || result[i] == '-') {
                capitalize = true;
            }
            else
            {
                cout << "Error: The city name cannot have numbers or special symbols. Try again: ";
                isValid = false; // Встановлюємо прапорець невдалого вводу
                break; // Виходимо з for, щоб перевірити isValid
            }
        }
        // Якщо ввід некоректний, повторюємо введення
        if (!isValid) {
            continue;
        }

        break; // Вихід із циклу, якщо всі перевірки пройдені
    }

    return result;
}
void SityInput(Client& client)
{
    string input_str;
    cout << endl;
    cout << "Enter the \"City\" (max 16 characters): ";

    // Копіюємо результат у client.city
    strcpy(client.city, formatCityName().c_str());
}
// Функція для перевірки імені за допомогою regex
void ValidateFullName(char fullName[30]) {
    // Регулярний вираз для формату "Surname I. P." (англійські літери)
    regex fullNamePattern("([A-Z]{1})([a-z]{1,20})([ ]{1})([A-Z]{1})([\\.]{1})([ ]{1})([A-Z]{1})([\\.]{1})");
    string name;

    do {
        cout << "Enter Full Name (format: 'Surname I. P.'): ";
        getline(cin, name);

        // Видалення зайвих пробілів на початку та в кінці
        name = regex_replace(name, regex("^ +| +$|( ) +"), "$1");

        // Перевірка введеного значення через регулярний вираз
        if (!regex_match(name, fullNamePattern)) {
            cout << "Error: Full name must be entered in the format 'Surname I. P.' (Surname max: 20)\n";
        }
    } while (!regex_match(name, fullNamePattern));

    // Копіювання інформації у поле структури
    strncpy(fullName, name.c_str(), 30);
}
// Введення Is Payed 
void InputIsPaid(Client& client) {
    regex inputPattern("^[+-]$"); // Регулярний вираз для одного символу '+' або '-'
    string input;

    while (true) {
        cout << "Enter, whether the fine \"Is paid?\" (+/-): ";
        getline(cin, input);

        // Перевірка введення за шаблоном
        if (regex_match(input, inputPattern)) {
            if (input == "+") {
                client.isPaid = true;
            }
            else if (input == "-") {
                client.isPaid = false;
            }
            break; // Коректне введення
        }
        else {
            cout << "Error: Input must be '+' or '-' only. Try again." << endl;
        }
    }
}
// Введення суми кредиту
void InputLoanDate(Client& client) {
    do {
        while (true) {
            cout << "Please enter the date in DD.MM.YYYY format: ";

            string input;  // Використовуємо std::string для зберігання вводу
            getline(cin, input);  // Читання всього рядка

            // Перевірка на довжину введеного рядка
            if (input.length() > 10) {
                cout << "Error: Input exceeds maximum allowed length (10 characters). Try again.\n";
                continue; // Повторний запит на введення
            }

            // Копіювання в масив char для збереження у loanDate
            strncpy(client.loanDate, input.c_str(), 10);
            client.loanDate[10] = '\0';  // Забезпечуємо коректний термінатор

            break; // Вихід з циклу, якщо ввід правильний
        }
        if (IsValidDate(client.loanDate)) {
            return;
        }

    } while (true);

}


// Функція для введення та перевірки номера телефону
void ValidatePhoneNumber(Client& client)
{

    regex phonePattern("^[0-9]{10}$"); // Шаблон: рівно 10 цифр
    string inputPhone;

    // Цикл для введення номера телефону
    while (true) {
        cout << "Enter the \"Phone number\" (10 digits): +38";
        getline(cin, inputPhone);

        // Перевірка введеного номеру за шаблоном
        if (!regex_match(inputPhone, phonePattern)) {
            cout << "Error: Phone number must consist of exactly 10 digits." << endl;
            continue;
        }

        // Запис номеру в client.phoneNumber
        strncpy(client.phoneNumber, inputPhone.c_str(), sizeof(client.phoneNumber) - 1);
        client.phoneNumber[sizeof(client.phoneNumber) - 1] = '\0'; // Гарантуємо, що рядок завершений
        break;
    }
}
// Введення даних клієнта з перевірками
void InputClient(Client& client, const string& path) {

    SityInput(client);
    ValidateFullName(client.fullName);
    ValidatePhoneNumber(client);

    InputLoanBody(client.loanBody);
    client.totalAmount = client.loanBody * 1.1;
    client.monthlyPayment = client.totalAmount / 12;

    InputLoanDate(client);
    InputIsPaid(client);
}
// Виведення списку клієнтів
void ViewClients(const string& path) {
    if (!HasRecords(path)) {
        cout << "File is empty." << endl;
        return;
    }

    ifstream file(path, ios::binary);
    if (!file) {
        cout << "Error in file's opening" << endl;
        return;
    }

    cout << "x-------------------------------------------------------------------------------------------------------------------------X" << endl;
    cout << "|     |                  |                 |                |   Sum     |    Total   |  Monthly  |   First   |            |" << endl;
    cout << "|  №  |      City        |     FullName    |  Phone Number  |    of     |   payable  |  Payment  |    pay    |  loanDate  |" << endl;
    cout << "|     |                  |                 |                |  credit   |            |           |           |            |" << endl;
    cout << "x-------------------------------------------------------------------------------------------------------------------------X" << endl;

    Client current;
    size_t i = 0;

    while (file.read((char*)(&current), sizeof(Client))) {

        cout << "| " << setw(3) << left << ++i << " | "
            << setw(17) << left << current.city << "| "
            << setw(15) << left << current.fullName << " | +38"
            << setw(11) << left << current.phoneNumber << " | "
            << setw(9) << right << fixed << setprecision(2) << current.loanBody << " | "
            << setw(10) << right << fixed << setprecision(2) << current.totalAmount << " | "
            << setw(9) << right << fixed << setprecision(2) << current.monthlyPayment << " |   "
            << setw(3) << (current.isPaid ? "+" : "-") << "     |"
            << setw(11) << right << fixed << setprecision(2) << current.loanDate << " |   "
            << endl;
    }

    cout << "x-------------------------------------------------------------------------------------------------------------------------X" << endl;
    file.close();
}
// Редагування запису цілком
void EditClientRecord(Client& client) {
    cout << "Edit a customer record." << endl;
    cout << "Enter new data for the client: " << endl;
    InputClient(client, PATH);
}
// Редагування окремих полів запису
void EditClientField(Client& client) {
    string choiceInput;
    cout << "Select a field to edit: " << endl;
    cout << "1. City" << endl;
    cout << "2. Full Name" << endl;
    cout << "3. Phone Number" << endl;
    cout << "4. Sum of credit" << endl;
    cout << "5. Loan Date" << endl;
    cout << "6. First pay" << endl;
    cout << "Select an option: ";

    regex validChoicePattern("^[1-6]$");  // Регулярний вираз для вибору числа від 1 до 6

    while (true) {
        getline(cin, choiceInput);  // Отримуємо введення як рядок

        // Перевірка, чи введення відповідає патерну
        if (regex_match(choiceInput, validChoicePattern)) {
            int choice = stoi(choiceInput);  // Перетворюємо введене значення в ціле число

            switch (choice) {
            case 1:
                SityInput(client);
                return;
            case 2:
                ValidateFullName(client.fullName);
                return;
            case 3:
                ValidatePhoneNumber(client);
                return;
            case 4:
                InputLoanBody(client.loanBody);
                client.totalAmount = client.loanBody * 1.1;
                client.monthlyPayment = client.totalAmount / 12;
                return;
            case 5:
                InputLoanDate(client);
                return;
            case 6:
                InputIsPaid(client);
                return;
            default:
                cout << "Invalid choice. Try again: "; // По факту default не потрібен, але залишаю його для можливих подальших мдифікацій програми.
            }
        }
        else {
            cout << "Invalid choice. Try again (1-6): ";
        }
    }
}
void EditClient(const string& path) {
    if (!HasRecords(path)) {
        cout << "File is empty. No records to edit." << endl;
        return;
    }

    ViewClients(path); // Displaying the list of clients before editing

    string input;
    cout << "Are you sure? (Y/any other input): ";
    getline(cin, input);

    if (input != "Y" && input != "y") {
        cout << "Your choice was canceled :)" << endl;
        return;
    }

    string recordNumberInput;
    int recordNumber;

    fstream file(path, ios::binary | ios::in | ios::out);
    if (!file) {
        cout << "Error: Cannot open file." << endl;
        return;
    }

    Client client;

    while (true) {
        try {
            cout << "Enter the record number to edit: ";
            getline(cin, recordNumberInput);

            recordNumber = stoi(recordNumberInput);

            file.seekg(0, ios::end);
            size_t numRecords = file.tellg() / sizeof(Client);
            if (recordNumber < 1 || recordNumber > static_cast<int>(numRecords)) {
                cout << "Error: Invalid record number." << endl;
                continue;
            }
            break;
        }
        catch (const invalid_argument&) {
            cout << "Error: Invalid input. Please enter a valid number." << endl;
        }
        catch (const out_of_range&) {
            cout << "Error: Number is out of range." << endl;
        }
    }

    // Read the specified record
    file.seekg((recordNumber - 1) * sizeof(Client), ios::beg);
    file.read(reinterpret_cast<char*>(&client), sizeof(Client));

    // Allow the user to edit the record
    int choice;
    cout << "\nEdit:\n1. Full record\n2. Particular field\nYour choice: ";

    string choiceInput;
    regex validChoicePattern("^[12]$");

    while (true) {
        getline(cin, choiceInput);

        if (regex_match(choiceInput, validChoicePattern)) {
            choice = stoi(choiceInput);
            break;
        }
        else {
            cout << "Wrong choice. Try again: ";
        }
    }

    if (choice == 1) {
        EditClientRecord(client);
    }
    else if (choice == 2) {
        EditClientField(client);
    }

    // Write the modified record back to the file
    file.seekp((recordNumber - 1) * sizeof(Client), ios::beg);
    file.write(reinterpret_cast<const char*>(&client), sizeof(Client));

    file.close();
}
void DeleteRecordByName() {
    FILE* pfile;
    FILE* tempFile;
    Client a;
    bool found = false; // To track if a record is found for deletion

    char fullNameToDelete[30];
    string inputName;
    regex fullNamePattern("([A-Z]{1}[a-z]{1,20} [A-Z]{1}\\.[ ]{1}[A-Z]{1}\\.)"); // Формат "Surname I. P."

    while (true) {
        cout << "Enter the full name to delete (format: 'Surname I. P.'): ";
        getline(cin, inputName);

        // Видалення зайвих пробілів
        inputName = regex_replace(inputName, regex("^ +| +$|( ) +"), "$1");

        // Перевірка введення
        if (regex_match(inputName, fullNamePattern)) {
            break;
        }
        else {
            cout << "Error: Full name must be entered in the format 'Surname I. P.' (Surname max: 20)\n";
        }
    }

    strncpy(fullNameToDelete, inputName.c_str(), 30);

    fopen_s(&pfile, PATH, "rb");
    if (!pfile) {
        cerr << "Failed to open the file!" << endl;
        return;
    }

    fopen_s(&tempFile, "temp.dat", "wb");
    if (!tempFile) {
        cerr << "Failed to create a temporary file!" << endl;
        fclose(pfile);
        return;
    }

    // Read each record and copy it to the temp file if it doesn't match the name to delete
    while (fread(&a, sizeof(Client), 1, pfile) == 1) {
        if (strcmp(a.fullName, fullNameToDelete) != 0) {
            fwrite(&a, sizeof(Client), 1, tempFile);
        }
        else {
            found = true; // Mark that we found and skipped a record
        }
    }

    fclose(pfile);
    fclose(tempFile);

    // Replace the original file with the temp file
    remove(PATH);
    rename("temp.dat", PATH);

    if (found) {
        cout << "Client with name \"" << fullNameToDelete << "\" deleted successfully." << endl;
    }
    else {
        cout << "No records found with the name \"" << fullNameToDelete << "\" for deletion." << endl;
    }
}
void DeleteRecordByPhone(const string& path) {
    string inputPhoneNumber;
    char phoneNumberToDelete[15];
    regex phoneNumberPattern("^\\d{10}$"); // Регулярний вираз для перевірки 9 цифр після "+38"
    bool found = false; // To track if a record is found for deletion

    // Введення та перевірка номера телефону
    while (true) {
        cout << "Enter the phone number of the client to delete (+38): +38";
        getline(cin, inputPhoneNumber);

        // Видаляємо зайві пробіли
        inputPhoneNumber = regex_replace(inputPhoneNumber, regex("^ +| +$|( ) +"), "$1");

        if (regex_match(inputPhoneNumber, phoneNumberPattern)) {
            break; // Номер коректний
        }
        else {
            cout << "Error: Phone number must contain exactly 10 digits after '+38'.\n";
        }
    }

    strncpy(phoneNumberToDelete, inputPhoneNumber.c_str(), sizeof(phoneNumberToDelete));

    FILE* pfile;
    FILE* tempFile;
    Client client;

    fopen_s(&pfile, PATH, "rb");
    if (!pfile) {
        cerr << "Failed to open the file!" << endl;
        return;
    }

    fopen_s(&tempFile, "temp.dat", "wb");
    if (!tempFile) {
        cerr << "Failed to create a temporary file!" << endl;
        fclose(pfile);
        return;
    }

    // Read each record and copy it to the temp file if it doesn't match the phone number to delete
    while (fread(&client, sizeof(Client), 1, pfile) == 1) {
        if (strcmp(client.phoneNumber, phoneNumberToDelete) != 0) {
            fwrite(&client, sizeof(Client), 1, tempFile);
        }
        else {
            found = true; // Mark that we found and skipped a record
        }
    }

    fclose(pfile);
    fclose(tempFile);

    // Replace the original file with the temp file
    remove(path.c_str());
    rename("temp.dat", path.c_str());

    if (found) {
        cout << "Client with phone number \"+38" << phoneNumberToDelete << "\" deleted successfully." << endl;
    }
    else {
        cout << "No records found with the phone number \"+38" << phoneNumberToDelete << "\" for deletion." << endl;
    }
}
void DeleteClient(const string& path) {
    if (!HasRecords(path)) {
        cout << "File is empty. No records to delete." << endl;
        return;
    }

    string input;
    regex validChoicePattern("^[12]$"); // Дозволяємо тільки '1' або '2'

    int choice;

    // Отримуємо і перевіряємо вибір користувача
    while (true) {
        cout << "Choose deletion method:\n1. Delete by Full Name\n2. Delete by Phone Number\nEnter your choice: ";
        getline(cin, input);

        // Видаляємо зайві пробіли на початку та в кінці
        input = regex_replace(input, regex("^ +| +$|( ) +"), "$1");

        // Перевірка вводу
        if (regex_match(input, validChoicePattern)) {
            choice = stoi(input); // Конвертуємо введення в число
            break;
        }
        else {
            cout << "Error: Please enter '1' or '2' only." << endl;
        }
    }

    switch (choice) {
    case 1: {
        DeleteRecordByName();
        break;
    }
    case 2: {
        DeleteRecordByPhone(PATH);
        break;
    }
    default:
        cout << "Invalid choice. Please enter 1 or 2." << endl;
        break;
    }
}
void SortRecordsByCity() {
    FILE* pfile;
    Client a, b;
    bool swapped;

    fopen_s(&pfile, PATH, "rb+");
    if (!pfile) {
        cerr << "Failed to open the file!" << endl;
        return;
    }

    // Count records
    int recordCount = 0;
    while (fread(&a, sizeof(Client), 1, pfile) == 1) {
        recordCount++;
    }

    if (recordCount < 2) {
        cout << "No sorting needed. The file has less than 2 records." << endl;
        fclose(pfile);
        return;
    }

    // Sort records using bubble sort
    do {
        swapped = false;
        rewind(pfile); // Return to the beginning of the file

        for (int i = 0; i < recordCount - 1; i++) {
            // Read two consecutive records
            fread(&a, sizeof(Client), 1, pfile);
            long posA = ftell(pfile) - sizeof(Client); // Position of the first record
            fread(&b, sizeof(Client), 1, pfile);
            long posB = ftell(pfile) - sizeof(Client); // Position of the second record

            // Compare city names (case-insensitive) in ascending order
            if (_stricmp(a.city, b.city) > 0) { // Use _stricmp for case-insensitive comparison
                // Swap records
                fseek(pfile, posA, SEEK_SET); // Position to write the first record
                fwrite(&b, sizeof(Client), 1, pfile); // Write the second record at the first position
                fseek(pfile, posB, SEEK_SET); // Position to write the second record
                fwrite(&a, sizeof(Client), 1, pfile); // Write the first record at the second position
                swapped = true;
            }

            // Move the file pointer to the next record pair
            fseek(pfile, posA + sizeof(Client), SEEK_SET);
        }
    } while (swapped); // Continue until no swaps are made

    fclose(pfile);
    cout << "Sorting by city (ascending) completed." << endl;
}
void SortRecordsBySumOfCredit() {
    FILE* pfile;
    Client a, b;
    bool swapped;

    fopen_s(&pfile, PATH, "rb+");
    if (!pfile) {
        cerr << "Failed to open the file!" << endl;
        return;
    }

    // Count records
    int recordCount = 0;
    while (fread(&a, sizeof(Client), 1, pfile) == 1) {
        recordCount++;
    }

    if (recordCount < 2) {
        cout << "No sorting needed. The file has less than 2 records." << endl;
        fclose(pfile);
        return;
    }

    // Sort records using bubble sort
    do {
        swapped = false;
        rewind(pfile); // Return to the beginning of the file

        for (int i = 0; i < recordCount - 1; i++) {
            // Read two consecutive records
            fread(&a, sizeof(Client), 1, pfile);
            long posA = ftell(pfile) - sizeof(Client); // Position of the first record
            fread(&b, sizeof(Client), 1, pfile);
            long posB = ftell(pfile) - sizeof(Client); // Position of the second record

            // Compare sum of credits in ascending order
            if (a.loanBody > b.loanBody) {
                // Swap records
                fseek(pfile, posA, SEEK_SET); // Position to write the first record
                fwrite(&b, sizeof(Client), 1, pfile); // Write the second record at the first position
                fseek(pfile, posB, SEEK_SET); // Position to write the second record
                fwrite(&a, sizeof(Client), 1, pfile); // Write the first record at the second position
                swapped = true;
            }

            // Move the file pointer to the next record pair
            fseek(pfile, posA + sizeof(Client), SEEK_SET);
        }
    } while (swapped); // Continue until no swaps are made

    fclose(pfile);
    cout << "Sorting by loan body (ascending) completed." << endl;
}
void SortRecords() {
    string input;
    int choice;
    regex validChoicePattern("^[12]$"); // Дозволяємо тільки "1" або "2"

    // Отримуємо і перевіряємо вибір користувача
    while (true) {
        cout << "Choose the parameter to sort by:\n1. City (ascending)\n2. Sum of credit (ascending)\nYour choice: ";
        getline(cin, input);

        // Видаляємо зайві пробіли
        input = regex_replace(input, regex("^ +| +$|( ) +"), "$1");

        if (regex_match(input, validChoicePattern)) {
            choice = stoi(input); // Конвертуємо введення в число
            break;
        }
        else {
            cout << "Error: Please enter '1' or '2' only." << endl;
        }
    }

    switch (choice) {
    case 1:
        SortRecordsByCity();
        break;
    case 2:
        SortRecordsBySumOfCredit();
        break;
    default:
        cout << "Invalid choice!" << endl; // Цей варіант неможливий завдяки перевірці вводу
    }
}


int OnlyOneRecord(const string& path)
{
    ifstream file(path, ios::binary);

    file.seekg(0, ios::end); // Переміщаємось в кінець файлу
    int fileSize = file.tellg(); // Отримуємо розмір файлу в байтах
    file.close();

    return fileSize / sizeof(Client); // Рахуємо кількість записів
}
void FIRST_ByClientName(const string& path) {
    FILE* pfile;
    Client a;

    fopen_s(&pfile, PATH, "rb");
    if (!pfile) {
        cerr << "Failed to open the file!" << endl;
        return;
    }

    if (!HasRecords(path)) {
        cout << "File is empty. No records to search." << endl;
        fclose(pfile);
        return;
    }

    string inputFullName;
    char fullNameToSearch[30];
    regex fullNamePattern("([A-Z]{1}[a-z]{1,20} [A-Z]{1}\\.[ ]{1}[A-Z]{1}\\.)"); // Формат "Surname I. P."

    // Введення та перевірка імені
    while (true) {
        cout << "Enter the full name to search: ";
        getline(cin, inputFullName);

        // Видалення зайвих пробілів
        inputFullName = regex_replace(inputFullName, regex("^ +| +$|( ) +"), "$1");

        if (regex_match(inputFullName, fullNamePattern)) {
            break;
        }
        else {
            cout << "Error: Full name must be entered in the format 'Surname I. P.'\n";
        }
    }

    strncpy(fullNameToSearch, inputFullName.c_str(), sizeof(fullNameToSearch));

    int count = 0;
    bool found = false;
    size_t i = 0;

    cout << fixed << setprecision(2);

    while (fread(&a, sizeof(Client), 1, pfile) == 1) {
        if (strcmp(a.fullName, fullNameToSearch) == 0) {
            if (!found) {
                cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
                cout << "|  №  |   City    |      FullName       |  Phone Number  |  Sum of credit |  Total payable  |  Monthly Payment  | First pay |  loanDate  |" << endl;
                cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
                found = true;
            }

            cout << "| " << setw(3) << left << ++i << " | "
                << setw(10) << left << a.city << "| "
                << setw(19) << left << a.fullName << " | +38"
                << setw(11) << left << a.phoneNumber << " | "
                << setw(14) << right << a.loanBody << " | "
                << setw(15) << right << a.totalAmount << " | "
                << setw(17) << right << a.monthlyPayment << " |   "
                << setw(3) << (a.isPaid ? "+" : "-") << "     |"
                << setw(11) << right << a.loanDate << " |"
                << endl;

            count++;
        }
    }

    if (found) {
        cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
        cout << "Client " << fullNameToSearch << " has " << count << " credit(s)." << endl;
    }
    else {
        cout << "Client " << fullNameToSearch << " has no credits." << endl;
    }

    fclose(pfile);
}
void Second_MAX_MIN(const string& path)
{
    FILE* pfile;
    Client a;
    float min = 10000, max = -1;

    fopen_s(&pfile, PATH, "rb");
    if (!pfile) {
        cerr << "Failed to open the file!" << endl;
        return;
    }
    if (!HasRecords(path)) {
        cout << "File is empty. No records to search." << endl;
        return;
    }



    while (fread(&a, sizeof(Client), 1, pfile) == 1)
    {
        if (a.loanBody > max)
        {
            max = a.loanBody;
        }
        if (a.loanBody < min)
        {
            min = a.loanBody;
        }
    }
    cout << "The minimum credit body: " << min << endl;
    cout << "The maximum credit body: " << max << endl;

    fclose(pfile);
}

time_t convertToTimeT(const char date[]) {
    tm tm = {};
    // Parse date in "dd.mm.yyyy" format
    sscanf(date, "%2d.%2d.%4d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year);

    // Adjust month and year for tm (months are 0-11, years since 1900)
    tm.tm_mon -= 1;         // Months are 0-11 in tm
    tm.tm_year -= 1900;     // Years since 1900 in tm

    return mktime(&tm); // Convert to time_t for easy comparison
}
bool compareDates(const char date1[], const char date2[]) {
    return convertToTimeT(date1) < convertToTimeT(date2);
}
bool isDateBetween(const char targetDate[], const char startDate[], const char endDate[]) {
    time_t target = convertToTimeT(targetDate);
    time_t start = convertToTimeT(startDate);
    time_t end = convertToTimeT(endDate);

    return (target >= start && target <= end);
}

void Third_CreditsByDate(const string& path) {
    FILE* pfile;
    Client a;
    size_t i = 0;
    int count = 0;
    char firstDate[11], secondDate[11];

    fopen_s(&pfile, PATH, "rb");
    if (!pfile) {
        cerr << "Failed to open the file!" << endl;
        return;
    }

    if (!HasRecords(path)) {
        cout << "File is empty. No records to search." << endl;
        fclose(pfile);
        return;
    }

    cout << "Enter the \"first date\" (DD.MM.YYYY): ";
    cin.getline(firstDate, sizeof(firstDate));

    cout << "Enter the \"second date\" (DD.MM.YYYY): ";
    cin.getline(secondDate, sizeof(secondDate));

    // Swap dates if needed
    if (!compareDates(firstDate, secondDate)) {
        swap(firstDate, secondDate);
    }

    // Output header if records exist
    cout << fixed << setprecision(2);
    while (fread(&a, sizeof(Client), 1, pfile) == 1) {
        if (isDateBetween(a.loanDate, firstDate, secondDate)) {
            if (count++ == 0) {
                cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
                cout << "|  №  |   City    |      FullName       |  Phone Number  |  Sum of credit |  Total payable  |  Monthly Payment  | First pay |  loanDate  |" << endl;
                cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
            }
            cout << "| " << setw(3) << left << count << " | "
                << setw(10) << left << a.city << "| "
                << setw(19) << left << a.fullName << " | +38"
                << setw(11) << left << a.phoneNumber << " | "
                << setw(14) << right << a.loanBody << " | "
                << setw(15) << right << a.totalAmount << " | "
                << setw(17) << right << a.monthlyPayment << " |   "
                << setw(3) << (a.isPaid ? "+" : "-") << "     |"
                << setw(11) << right << a.loanDate << " |"
                << endl;
        }
    }

    fclose(pfile);

    if (count > 0) {
        cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
        cout << "There are " << count << " record(s)." << endl;
    }
    else {
        cout << "There are no records in this range :(" << endl;
    }
}
void Forth_Debetors(const string& path)
{
    FILE* pfile;
    Client a;
    bool found = true;
    size_t count = 0;
    size_t i = 0;
    fopen_s(&pfile, PATH, "rb");
    if (!pfile) {
        cerr << "Failed to open the file!" << endl;
        return;
    }
    if (!HasRecords(path)) {
        cout << "File is empty. No records to search." << endl;
        return;
    }

    while (fread(&a, sizeof(Client), 1, pfile) == 1)
    {
        if (a.isPaid == false)
        {
            if (found)
            {
                cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
                cout << "|  №  |   City    |      FullName       |  Phone Number  |  Sum of credit |  Total payable  |  Monthly Payment  | First pay |  loanDate  |" << endl;
                cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
                found = false;
            }
            cout << "| " << setw(3) << left << ++i << " | "
                << setw(10) << left << a.city << "| "
                << setw(19) << left << a.fullName << " | +38"
                << setw(11) << left << a.phoneNumber << " | "
                << setw(14) << right << fixed << setprecision(2) << a.loanBody << " | "
                << setw(15) << right << fixed << setprecision(2) << a.totalAmount << " | "
                << setw(17) << right << fixed << setprecision(2) << a.monthlyPayment << " |   "
                << setw(3) << (a.isPaid ? "+" : "-") << "     |"
                << setw(11) << right << fixed << setprecision(2) << a.loanDate << " |   "
                << endl;
            count++;

        }
    }
    if (count == 1) {
        cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
        cout << "There is " << count << " debetor." << endl;
    }
    else if (count > 1)
    {
        cout << "X----------------------------------------------------------------------------------------------------------------------------------------X" << endl;
        cout << "There are " << count << " debetors." << endl;
    }
    else
    {
        cout << "There are no debetors :) " << endl;
    }
}
void Fifth_TotalAmount(const string& path)
{
    FILE* pfile;
    Client a;
    float answer = 0;
    fopen_s(&pfile, PATH, "rb");
    if (!pfile) {
        cerr << "Failed to open the file!" << endl;
        return;
    }
    if (!HasRecords(path)) {
        cout << "File is empty. No records to search." << endl;
        return;
    }

    while (fread(&a, sizeof(Client), 1, pfile) == 1)
    {
        answer += a.totalAmount;
    }

    cout << endl << "The bank's total profit (for the year) is " << answer << endl << endl;
}

void Requests() {
    string input;
    int choice;
    regex validChoicePattern("^[0-5]$"); // Дозволяємо тільки цифри від 0 до 5

    while (true) {
        system("cls");
        cout << "\nMake a choice what to view:\n"
            << "0. Exit\n"
            << "1. Loans by client's name\n"
            << "2. MAX and MIN loan\n"
            << "3. Loans by a certain period\n"
            << "4. All debtors\n"
            << "5. Bank's total profit (for the year)\n"
            << "Choose an option: ";
        getline(cin, input);

        // Перевірка на коректність вводу
        if (regex_match(input, validChoicePattern)) {
            choice = stoi(input); // Перетворення у число після перевірки
            break;
        }
        else {
            cout << "Error: Please enter a number between 0 and 5.\n";
            system("pause");
        }
    }

    switch (choice) {
    case 0:
        return;
    case 1:
        FIRST_ByClientName(PATH);
        break;
    case 2:
        Second_MAX_MIN(PATH);
        break;
    case 3:
        Third_CreditsByDate(PATH);
        break;
    case 4:
        Forth_Debetors(PATH);
        break;
    case 5:
        Fifth_TotalAmount(PATH);
        break;
    default:
        cout << "Invalid choice!" << endl;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    EnsureFileExists(PATH);

    string input;
    int choice;
    regex integerRegex("^[0-6]$"); // Дозволяємо лише числа від 0 до 6

    while (true) {
        system("cls");
        cout << "\nMenu:\n"
            << "0. Exit\n"
            << "1. View Records\n"
            << "2. Add Record\n"
            << "3. Edit Record\n"
            << "4. Delete Record\n"
            << "5. Sort Records\n"
            << "6. Make a request\n"
            << "Choose an option: ";
        getline(cin, input);

        // Перевіряємо ввід
        if (!regex_match(input, integerRegex)) {
            cout << "Invalid input. Please enter a number between 0 and 6." << endl;
            system("pause");
            continue;
        }

        choice = stoi(input); // Перетворення на число

        // Обробка вибору
        switch (choice) {
        case 0:
            return 0;

        case 1:
            ViewClients(PATH);
            break;

        case 2: {
            Client client;
            cout << "Are you sure? (Y/ any other input): ";
            getline(cin, input);

            if (input == "Y" || input == "y") {
                InputClient(client, PATH);
                WriteClient(PATH, client);
            }
            else {
                cout << "Your choice was canceled :)" << endl;
            }
            break;
        }

        case 3:
            EditClient(PATH);
            break;

        case 4:
            if (!HasRecords(PATH)) {
                cout << "File is empty. No records to delete." << endl;
            }
            else {
                ViewClients(PATH);
                cout << "Are you sure? (Y/ any other input): ";
                getline(cin, input);

                if (input == "Y" || input == "y") {
                    DeleteClient(PATH);
                }
                else {
                    cout << "Your choice was canceled :)" << endl;
                }
            }
            break;

        case 5:
            if (!HasRecords(PATH)) {
                cout << "File is empty." << endl;
            }
            else if (OnlyOneRecord(PATH) < 2) {
                cout << "There are not enough records in the file to sort!" << endl;
            }
            else {
                cout << "Are you sure? (Y/ any other input): ";
                getline(cin, input);

                if (input == "Y" || input == "y") {
                    SortRecords();
                }
                else {
                    cout << "Your choice was canceled :)" << endl;
                }
            }
            break;

        case 6:
            Requests();
            break;

        default:
            cout << "Invalid input." << endl;
        }

        system("pause");
    }
}