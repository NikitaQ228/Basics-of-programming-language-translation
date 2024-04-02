//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Text {
private:
    char* arr;
    int size, n;

    //Добавление элемента
    void push(char s) {
        if (size >= n) {
            n = n > 0 ? n * 2 : 1;
            char* b = new char[n];
            for (int i = 0; i < n / 2; i++)
                b[i] = arr[i];
            b[size++] = s;
            delete[] arr;
            arr = b;
        }
        else arr[size++] = s;
    }
    //Проверка символа с индексом i на букву латинского алфавита или цифру
    bool check(int i) {
        if ('0' <= arr[i] && arr[i] <= '9' || 'A' <= arr[i] && arr[i] <= 'Z'
            || 'a' <= arr[i] && arr[i] <= 'z') return true;
        return false;
    }
    //Проверка символа с индексом i на цифру
    bool check_num(int i) {
        if ('0' <= arr[i] && arr[i] <= '9') return true;
        return false;
    }
    //Проверка символа с индексом i на разделитель слов
    bool blank(int i) {
        if (arr[i] == ' ' || arr[i] == '\t' || arr[i] == '\n') return true;
        return false;
    }

public:
    //Конструкторы
    Text() : arr(NULL), n(0), size(0) {}
    Text(int N): arr(new char[N]), n(N), size(0) {}
    Text(const Text& T) {
        arr = new char[T.size];
        n = T.size; size = T.size;
        for (int i = 0; i < n; i++)
            arr[i] = T.arr[i];
    }

    //Деструктор
    ~Text() { if (arr) delete[] arr; }

    //Функция обработки текста
    vector <Text> processText() {
        vector <Text> res;
        bool flag = true; // Слово или нет?
        for (int i = 0, count = 0; i < size; i++, flag = true, count = 0) {
            Text word(10);
            for (; i < size && !blank(i); i++) {
                flag = flag && check(i);
                if (flag) {
                    if (i < size - 1 && check_num(i) && check_num(i + 1)) count++;
                    word.push(arr[i]);
                }
            }
            if (flag && count == 1) {
                if (i == size - 1) word.push(arr[i]);
                res.push_back(word);
            }
        }
        return res;
    }

    friend istream& operator >> (istream&, Text&);   //Оператор ввода из консоли
    friend ostream& operator << (ostream&, const Text&);   //Оператор вывода в консоль
};

istream& operator >> (istream& in, Text& T) {
    for (char x; in.get(x); T.push(x));
    return in;
}

ostream& operator << (ostream& out, const Text& T) {
    for (int i = 0; i < T.size; i++) out << T.arr[i];
    return out;
}

int main() {
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    ifstream in_file("input.txt");
    if (!in_file) {
        cout << "The file was not opened!";
        return 0;
    }
    if (in_file.peek() == EOF) {
        cout << "The file is empty!";
        return 0;
    }
    Text txt(20); in_file >> txt;
    in_file.close();
    vector <Text> res = txt.processText();
    if (res.empty()) {
        cout << "There are no words in which there are two and only two consecutive digits.";
        return 0;
    }
    ofstream out_file("output.txt");
    for (Text i : res) {
        out_file << i << ' ';
        cout << i << ' ';
    }
    out_file.close();
    return 0;
}