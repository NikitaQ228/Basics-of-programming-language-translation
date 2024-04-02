#define _CRT_SECURE_NO_WARNINGS
#include <iostream> 
#include <vector> 
#include <fstream>
#include <cstring>

using namespace std;

// Файл для вывода
ofstream file_out;

//Типы лексем и состояния
enum types { se, ca, de, en, id, eq, ao, vl, wl, co };
enum SYN_states { SE, OP, CA, _VL, ID, _EQ, OP1, EN, VL1, ID1, EQ1, OP2, EN1, ID2, EQ2, OP3, EN2, ERROR };

const char* print_type[] = { "se", "ca", "de", "en", "id", "eq","ao", "vl", "wl", "co" };

//Лексема
struct Lex {
    char* str;
    types type;
};

//Входные символы и состояния
enum signals { Digit, Letter, Equal, Arithmetic, Compare1, Compare2, Blank, Other };
enum LEX_states { First, VL, AO, EQ, CO1, CO2, ID_KW, End, Error };

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
    return i == '+' || i == '-' || i == '*' || i == '/';
}

bool is_border(char i) {
    return is_blank(i) || is_Arithmetic(i) || i == '=';
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
types is_type(char* str, LEX_states s) {
    if (s == AO) return ao;
    if (s == EQ) return eq;
    if (s == CO1 || s == CO2) return co;
    if (s == ID_KW) {
        if (!strcmp(str, "select")) return se;
        if (!strcmp(str, "case")) return ca;
        if (!strcmp(str, "default")) return de;
        if (!strcmp(str, "end")) return en;
        if (strlen(str) <= 5) return id;
    }
    if (s == VL && atoi(str) < 32768) return vl;
    return wl;
}

//Ф-я создания таблицы автомата для лексичеческого анализа
void create_table1(LEX_states table[8][9]) {
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

//Функция лексического анализа
void LexAnalysis(vector <Lex>& res, char* str) {

    //Создание таблицы
    LEX_states table[8][9];
    create_table1(table);

    //Объявление переменный
    int size = strlen(str);
    int pos = 0, first_pos = 0;
    LEX_states current_state = First, previous_state;
    Lex lexema;

    while (pos <= size) {

        previous_state = current_state;

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

//Ф-я создания таблицы автомата для синтаксического анализа
void creat_table2(SYN_states table[10][18]) {
    table[se][SE] = OP;
    table[se][OP] = ERROR;
    table[se][CA] = ERROR;
    table[se][_VL] = ERROR;
    table[se][ID] = ERROR;
    table[se][_EQ] = ERROR;
    table[se][OP1] = ERROR;
    table[se][EN] = ERROR;
    table[se][VL1] = ERROR;
    table[se][ID1] = ERROR;
    table[se][EQ1] = ERROR;
    table[se][OP2] = ERROR;
    table[se][EN1] = ERROR;
    table[se][ID2] = ERROR;
    table[se][EQ2] = ERROR;
    table[se][OP3] = ERROR;
    table[se][EN2] = ERROR;
    table[se][ERROR] = ERROR;

    table[id][SE] = ERROR;
    table[id][OP] = CA;
    table[id][CA] = ERROR;
    table[id][_VL] = ERROR;
    table[id][ID] = _EQ;
    table[id][_EQ] = ERROR;
    table[id][OP1] = EN;
    table[id][EN] = ERROR;
    table[id][VL1] = ERROR;
    table[id][ID1] = EQ1;
    table[id][EQ1] = ERROR;
    table[id][OP2] = EN1;
    table[id][EN1] = ERROR;
    table[id][ID2] = EQ2;
    table[id][EQ2] = ERROR;
    table[id][OP3] = EN2;
    table[id][EN2] = ERROR;
    table[id][ERROR] = ERROR;

    table[vl][SE] = ERROR;
    table[vl][OP] = CA;
    table[vl][CA] = ERROR;
    table[vl][_VL] = ID;
    table[vl][ID] = ERROR;
    table[vl][_EQ] = ERROR;
    table[vl][OP1] = EN;
    table[vl][EN] = ERROR;
    table[vl][VL1] = ID1;
    table[vl][ID1] = ERROR;
    table[vl][EQ1] = ERROR;
    table[vl][OP2] = EN1;
    table[vl][EN1] = ERROR;
    table[vl][ID2] = ERROR;
    table[vl][EQ2] = ERROR;
    table[vl][OP3] = EN2;
    table[vl][EN2] = ERROR;
    table[id][ERROR] = ERROR;

    table[ao][SE] = ERROR;
    table[ao][OP] = ERROR;
    table[ao][CA] = OP;
    table[ao][_VL] = ERROR;
    table[ao][ID] = ERROR;
    table[ao][_EQ] = ERROR;
    table[ao][OP1] = ERROR;
    table[ao][EN] = OP1;
    table[ao][VL1] = ERROR;
    table[ao][ID1] = ERROR;
    table[ao][EQ1] = ERROR;
    table[ao][OP2] = ERROR;
    table[ao][EN1] = OP2;
    table[ao][ID2] = ERROR;
    table[ao][EQ2] = ERROR;
    table[ao][OP3] = ERROR;
    table[ao][EN2] = OP3;
    table[ao][ERROR] = ERROR;

    table[eq][SE] = ERROR;
    table[eq][OP] = ERROR;
    table[eq][CA] = ERROR;
    table[eq][_VL] = ERROR;
    table[eq][ID] = ERROR;
    table[eq][_EQ] = OP1;
    table[eq][OP1] = ERROR;
    table[eq][EN] = ERROR;
    table[eq][VL1] = ERROR;
    table[eq][ID1] = ERROR;
    table[eq][EQ1] = OP2;
    table[eq][OP2] = ERROR;
    table[eq][EN1] = ERROR;
    table[eq][ID2] = ERROR;
    table[eq][EQ2] = OP3;
    table[eq][OP3] = ERROR;
    table[eq][EN2] = ERROR;
    table[eq][ERROR] = ERROR;

    table[ca][SE] = ERROR;
    table[ca][OP] = ERROR;
    table[ca][CA] = _VL;
    table[ca][_VL] = ERROR;
    table[ca][ID] = ERROR;
    table[ca][_EQ] = ERROR;
    table[ca][OP1] = ERROR;
    table[ca][EN] = VL1;
    table[ca][VL1] = ERROR;
    table[ca][ID1] = ERROR;
    table[ca][EQ1] = ERROR;
    table[ca][OP2] = ERROR;
    table[ca][EN1] = ERROR;
    table[ca][ID2] = ERROR;
    table[ca][EQ2] = ERROR;
    table[ca][OP3] = ERROR;
    table[ca][EN2] = ERROR;
    table[ca][ERROR] = ERROR;

    table[de][SE] = ERROR;
    table[de][OP] = ERROR;
    table[de][CA] = ERROR;
    table[de][_VL] = ERROR;
    table[de][ID] = ERROR;
    table[de][_EQ] = ERROR;
    table[de][OP1] = ERROR;
    table[de][EN] = ID2;
    table[de][VL1] = ERROR;
    table[de][ID1] = ERROR;
    table[de][EQ1] = ERROR;
    table[de][OP2] = ERROR;
    table[de][EN1] = ID2;
    table[de][ID2] = ERROR;
    table[de][EQ2] = ERROR;
    table[de][OP3] = ERROR;
    table[de][EN2] = ERROR;
    table[de][ERROR] = ERROR;

    table[en][SE] = ERROR;
    table[en][OP] = ERROR;
    table[en][CA] = ERROR;
    table[en][_VL] = ERROR;
    table[en][ID] = ERROR;
    table[en][_EQ] = ERROR;
    table[en][OP1] = ERROR;
    table[en][EN] = SE;
    table[en][VL1] = ERROR;
    table[en][ID1] = ERROR;
    table[en][EQ1] = ERROR;
    table[en][OP2] = ERROR;
    table[en][EN1] = SE;
    table[en][ID2] = ERROR;
    table[en][EQ2] = ERROR;
    table[en][OP3] = ERROR;
    table[en][EN2] = SE;
    table[en][ERROR] = ERROR;

    table[wl][SE] = ERROR;
    table[wl][OP] = ERROR;
    table[wl][CA] = ERROR;
    table[wl][_VL] = ERROR;
    table[wl][ID] = ERROR;
    table[wl][_EQ] = ERROR;
    table[wl][OP1] = ERROR;
    table[wl][EN] = ERROR;
    table[wl][VL1] = ERROR;
    table[wl][ID1] = ERROR;
    table[wl][EQ1] = ERROR;
    table[wl][OP2] = ERROR;
    table[wl][EN1] = ERROR;
    table[wl][ID2] = ERROR;
    table[wl][EQ2] = ERROR;
    table[wl][OP3] = ERROR;
    table[wl][EN2] = ERROR;
    table[wl][ERROR] = ERROR;

    table[co][SE] = ERROR;
    table[co][OP] = ERROR;
    table[co][CA] = ERROR;
    table[co][_VL] = ERROR;
    table[co][ID] = ERROR;
    table[co][_EQ] = ERROR;
    table[co][OP1] = ERROR;
    table[co][EN] = ERROR;
    table[co][VL1] = ERROR;
    table[co][ID1] = ERROR;
    table[co][EQ1] = ERROR;
    table[co][OP2] = ERROR;
    table[co][EN1] = ERROR;
    table[co][ID2] = ERROR;
    table[co][EQ2] = ERROR;
    table[co][OP3] = ERROR;
    table[co][EN2] = ERROR;
    table[co][ERROR] = ERROR;
}

//Функция определения ожидаемой лексемы на месте ошибки
types Error_type(SYN_states S, bool& fl_vl) {
    switch (S)
    {
    case SE: 
        return se;
    case CA: 
        return ca;
    case OP:
    case OP1:
    case OP2:
    case OP3:
        fl_vl = true;
    case ID:
    case ID1:
    case ID2:
        return id;
    case _VL:
    case VL1:
        return vl;
    case _EQ:
    case EQ1:
    case EQ2:
        return eq;
    case EN:
    case EN1:
    case EN2:
        return en;
    default: 
        return wl;
    }
}

//Фунукция вывода результата работы синтаксического анализа
void output_syn(SYN_states temp, SYN_states prev, int pos) {
    bool fl_vl = false;
    if (temp == SE && pos) {
        file_out << "OK";
        cout << "OK";
    }
    else if (temp == ERROR) {
        types typ = Error_type(prev, fl_vl);
        file_out << pos - 1 << ' ' << print_type[typ];
        cout << pos - 1 << ' ' << print_type[typ];
    }
    else {
        types typ = Error_type(prev, fl_vl);
        file_out << pos << ' ' << print_type[typ];
        cout << pos << ' ' << print_type[typ];
    }
    if (fl_vl) {
        file_out << ' ' << print_type[vl];
        cout << ' ' << print_type[vl];
    }
}

//Функция синтаксичеческого анализа
void Syntactic(const vector <Lex>& list) {

    //Создание таблицы
    SYN_states table[10][18];
    creat_table2(table);

    //Объявление и инциализация переменных
    int pos = 0, size = list.size();
    SYN_states current_state = SE, previous_state = SE;

    //Цикл перехода по состояниям
    while (pos < size && current_state != ERROR) {
        previous_state = current_state;
        current_state = table[list[pos].type][current_state];
        ++pos;
    }

    //Выводим результат
    output_syn(current_state, previous_state, pos);
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

//Вывод результата работы лексического анализа
void output_lex(const vector <Lex>& arr) {

    //Вывод слов
    for (Lex i : arr) {
        cout << i.str << '[' << print_type[i.type] << "] ";
        file_out << i.str << '[' << print_type[i.type] << "] ";
    }

    cout << endl;
    file_out << endl;

}

int main() {

    //Открытие файла
    ifstream file_in("input.txt", ios::binary);
    file_out.open("output.txt");

    //Проверкa на файл
    if (!file_in) return 0;

    //Чтение из файла
    char* txt = input(file_in);

    //Закрытие файла
    file_in.close();

    //вектор с результатом
    vector <Lex> res;

    LexAnalysis(res, txt);

    //Вывод результата в файл
    output_lex(res);

    Syntactic(res);

    //Закрытие файла
    file_out.close();

    //Освобождение памяти
    delete[] txt;
    for (Lex i : res) 
        delete[] i.str;

    return 0;
}