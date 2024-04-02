#define _CRT_SECURE_NO_WARNINGS
#include <iostream> 
#include <vector> 
#include <fstream>
#include <cstring>

using namespace std;

//Входные символы и состояния
enum signals { Digit, Letter, Other};
enum states { First, Before, OneDigit, TwoDigit, More2x, Error};

//Проверки на буквы латинского алфавита, цифры и разделители
bool is_blank(const char& i) {
    if (i == ' ' || i == '\t' || i == '\n' || i == '\0' || i == '\r') return true;
    return false;
}
bool is_num(const char& i) {
    if ('0' <= i && i <= '9') return true;
    return false;
}
bool is_ABC(const char& i) {
    if (('A' <= i && i <= 'Z') || ('a' <= i && i <= 'z')) return true;
    return false;
}

//Опрделение сигнала по си`мволу
signals check(const char& c) {
    if (is_num(c)) return Digit;
    if (is_ABC(c)) return Letter;
    return Other;
}

//Ф-я создания таблицы автомата
void create_table(states(*table)[6]) {
    table[Digit][First] = OneDigit;
    table[Digit][Before] = OneDigit;
    table[Digit][OneDigit] = TwoDigit;
    table[Digit][TwoDigit] = More2x;
    table[Digit][More2x] = More2x;
    table[Digit][Error] = Error;

    table[Letter][First] = Before;
    table[Letter][Before] = Before;
    table[Letter][OneDigit] = More2x;
    table[Letter][TwoDigit] = TwoDigit;
    table[Letter][More2x] = More2x;
    table[Letter][Error] = Error;

    table[Other][First] = Error;
    table[Other][Before] = Error;
    table[Other][OneDigit] = Error;
    table[Other][TwoDigit] = Error;
    table[Other][More2x] = Error;
    table[Other][Error] = Error;
}

//Считывание текста из файла
char* input(ifstream& file_in) {

    //Считывание размера
    int size = 0;
    file_in.seekg(0, ios::end);
    size = file_in.tellg();
    file_in.seekg(0, ios::beg);

    //Считывание строки
    char* line = new char[size + 1];
    file_in.read(&line[0], size);
    line[size] = '\0';

    return line;
}

//Вывод
void output(const vector <char*> & arr) {
    //Проверка на пустоту
    if (arr.empty()) return;

    //Открытие файла
    ofstream file_out("output.txt");

    //Вывод слов и осзвобождение памяти
    for (char* i : arr) {
        cout << i << ' ';
        file_out << i << ' ';
        delete[] i;
    }

    //Закрытие файла
    file_out.close();
}

//Функция лексического анализа
vector <char*> LexAnalysis(char* str) {
    //вектор с результатом
    vector <char*> res;

    //Создание таблицы
    states table[3][6];
    create_table(table);

    //Объявление переменный
    int pos = 0, first_pos;
    states current_state;
    char* lexema;

    while (str[pos] != '\0') {

        //Переход на начало лексемы
        current_state = First;
        first_pos = pos;

        //Цикл перехода по состояниям
        while (!is_blank(str[pos]))
            current_state = table[check(str[pos++])][current_state];

        //Если вышли в состоянии TwoDigit, 
        //то лексема подходит по заданию
        if (current_state == TwoDigit) {
            int length = pos - first_pos;
            lexema = new char[length + 1];
            strncpy(&lexema[0], &str[0] + first_pos, length);
            lexema[length] = '\0';
            res.push_back(lexema);
        }

        /*Если оказались в состоянии Error, то были введены посторонние символы.
        (т.е слово считается неверным)
        Если оказались в состоянии OneDigit, Before или More2x, 
        то лексема не подходит под условия задания.
        В обоих случаях мы не записываем результат в вектор.*/

        ++pos;
    }
    return res;
}

int main() {

    //Открытие файла
    ifstream file_in("input.txt", ios::binary);

    //Проверки на файл
    if (!file_in) return 0;
    if (file_in.peek() == EOF) return 0;

    //Чтение из файла
    char* txt = input(file_in);

    //Закрытие файла
    file_in.close();

    vector <char*> res = LexAnalysis(txt);

    //Вывод результата в файл
    output(res);

    delete[] txt;
    return 0;
}