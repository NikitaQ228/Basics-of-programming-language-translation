#define _CRT_SECURE_NO_WARNINGS
#include <iostream> 
#include <vector> 
#include <fstream>
#include <cstring>

using namespace std;

enum types { kw, id, eq, ao, vl, wl, co };

struct Lex {
    char* str;
    types type;
};

//Входные символы и состояния
enum signals { Digit, Letter, Equal, Arithmetic, Compare1, Compare2, Blank, Other };
enum states { First, VL, AO, EQ, CO1, CO2, ID_KW, End, Error };

const char* print_type[] = { "[kw]", "[id]", "[eq]","[ao]", "[vl]", "[wl]", "[co]" };

//Проверки
bool is_blank(char i) {
    return (i == ' ' || i == '\t' || i == '\n' || i == '\0' || i == '\r');
}

bool is_num(char i) {
        return ('0' <= i && i <= '9');
}

bool is_ABC(char i) {
    return (('A' <= i && i <= 'Z') || ('a' <= i && i <= 'z'));
}

bool is_Arithmetic(char i) {
    return i == '+' || i == '-';
}

bool is_border(char i) {
    return is_blank(i) || is_Arithmetic(i) || i == '=';
}

bool is_key_word(char* word) {
    return !(strcmp(word, "select") && strcmp(word, "case") && strcmp(word, "default") && strcmp(word, "end"));
}

//Опрделение сигнала по символу
signals check(char c) {
    if (c == '=') return Equal;
    if (c == '<') return Compare1;
    if (c == '>') return Compare2;
    if (is_num(c)) return Digit;
    if (is_ABC(c)) return Letter;
    if (is_Arithmetic(c)) return Arithmetic;
    if (is_blank(c)) return Blank;
    return Other;
}

//Определение типа лексемы
types is_type(char* str, states s) {
    if (s == AO) return ao;
    if (s == EQ) return eq;
    if (s == CO1 || s == CO2) return co;
    if (s == ID_KW && is_key_word(str)) return kw;
    if (s == ID_KW && strlen(str) <= 5) return id;
    if (s == VL && atoi(str) < 32768) return vl;
    return wl;
}

//Ф-я создания таблицы автомата
void create_table(states table[8][9]) {
    table[Digit][First] = VL;
    table[Digit][VL] = VL;
    table[Digit][AO] = End;
    table[Digit][EQ] = End;
    table[Digit][CO1] = End;
    table[Digit][CO2] = End;
    table[Digit][ID_KW] = ID_KW;
    table[Digit][End] = End;
    table[Digit][Error] = Error;

    table[Letter][First] = ID_KW;
    table[Letter][VL] = Error;
    table[Letter][AO] = End;
    table[Letter][EQ] = End;
    table[Letter][CO1] = End;
    table[Letter][CO2] = End;
    table[Letter][ID_KW] = ID_KW;
    table[Letter][End] = End;
    table[Letter][Error] = Error;

    table[Equal][First] = EQ;
    table[Equal][VL] = End;
    table[Equal][AO] = End;
    table[Equal][EQ] = End;
    table[Equal][CO1] = CO2;
    table[Equal][CO2] = CO2;
    table[Equal][ID_KW] = End;
    table[Equal][End] = End;
    table[Equal][Error] = Error;

    table[Arithmetic][First] = AO;
    table[Arithmetic][VL] = End;
    table[Arithmetic][AO] = End;
    table[Arithmetic][EQ] = End;
    table[Arithmetic][CO1] = End;
    table[Arithmetic][CO2] = End;
    table[Arithmetic][ID_KW] = End;
    table[Arithmetic][End] = End;
    table[Arithmetic][Error] = Error;

    table[Compare1][First] = CO1;
    table[Compare1][VL] = End;
    table[Compare1][AO] = End;
    table[Compare1][EQ] = End;
    table[Compare1][CO1] = End;
    table[Compare1][CO2] = End;
    table[Compare1][ID_KW] = End;
    table[Compare1][End] = End;
    table[Compare1][Error] = Error;

    table[Compare2][First] = CO2;
    table[Compare2][VL] = End;
    table[Compare2][AO] = End;
    table[Compare2][EQ] = End;
    table[Compare2][CO1] = CO2;
    table[Compare2][CO2] = End;
    table[Compare2][ID_KW] = End;
    table[Compare2][End] = End;
    table[Compare2][Error] = Error;

    table[Blank][First] = First;
    table[Blank][VL] = End;
    table[Blank][AO] = End;
    table[Blank][EQ] = End;
    table[Blank][CO1] = End;
    table[Blank][CO2] = End;
    table[Blank][ID_KW] = End;
    table[Blank][End] = First;
    table[Blank][Error] = End;

    table[Other][First] = Error;
    table[Other][VL] = Error;
    table[Other][AO] = Error;
    table[Other][EQ] = Error;
    table[Other][CO1] = Error;
    table[Other][CO2] = End;
    table[Other][ID_KW] = Error;
    table[Other][End] = End;
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
void output(const vector <Lex>& arr) {
    //Проверка на пустоту
    if (arr.empty()) return;

    //Открытие файла
    ofstream file_out("output.txt");

    //Вывод слов и осзвобождение памяти
    for (Lex i : arr) {
        cout << i.str << print_type[i.type] << ' ';
        file_out << i.str << print_type[i.type] << ' ';
    }

    cout << endl;
    file_out << endl;

    for (Lex i : arr) {
        if (i.type == id) {
            cout << i.str << ' ';
            file_out << i.str << ' ';
        }
    }

    cout << endl;
    file_out << endl;

    for (Lex i : arr) {
        if (i.type == vl) {
            cout << i.str << ' ';
            file_out << i.str << ' ';
        }
        delete[] i.str;
    }

    //Закрытие файла
    file_out.close();
}

//Функция лексического анализа
void LexAnalysis(vector <Lex>& res, char* str) {

    //Создание таблицы
    states table[8][9];
    create_table(table);

    //Объявление переменный
    int size = strlen(str) + 1;
    int pos = 0, first_pos = 0;
    states current_state = First;
    Lex lexema;

    while (pos <= size) {

        states previous_state = current_state;

        //переход по состояниям
        current_state = table[check(str[pos])][current_state];
        
        //Выделение лексемы
        if (current_state == End) {
            int length = pos - first_pos;
            lexema.str = new char[length + 1];
            strncpy(&lexema.str[0], &str[0] + first_pos, length);
            lexema.str[length] = '\0';
            lexema.type = is_type(lexema.str, previous_state);
            res.push_back(lexema);

            current_state = table[check(str[pos])][First];
            first_pos = pos;
        }
        
        ++pos;

        if (current_state == First) first_pos = pos;
    }
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

    //вектор с результатом
    vector <Lex> res;

    LexAnalysis(res, txt);

    //Вывод результата в файл
    output(res);

    delete[] txt;
    return 0;
}