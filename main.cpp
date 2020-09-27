#include <iostream>
#include <fstream>
#include <cstring>

#include "AVLTree.h"
#include "Vector.h"

TVector<unsigned char> strToVec(const char* str) {
    TVector<unsigned char> result;
    for (int i = 0; str[i] != '\0'; ++i) {
        result.PushBack(std::tolower(str[i]));
    }
    return result;
}

std::ostream& operator << (std::ostream& os, const TVector<unsigned char>& vec) {
    for (int i = 0; i < vec.Size(); ++i) {
        os << vec[i];
    }
    return os;
}

int cmp_vectors(const TVector<unsigned char>& first, const TVector<unsigned char>& second) { // 1 - первый больше, -1  - второй больше, 0 - равны
    for (size_t i = 0; i < std::min(first.Size(), second.Size()); ++i) {
        if (first[i] != second[i]) {
            return (first[i] > second[i] ? 1 : -1);
        }
    }
    if (first.Size() == second.Size()) {
        return 0;
    }
    return (first.Size() > second.Size() ? 1 : -1);
}

bool operator < (const TVector<unsigned char>& first, const TVector<unsigned char>& second) {
    return cmp_vectors(first,second) == -1;
}

bool operator > (const TVector<unsigned char>& first, const TVector<unsigned char>& second) {
    return cmp_vectors(first,second) == 1;
}

bool operator == (const TVector<unsigned char>& first, const TVector<unsigned char>& second) {
    return cmp_vectors(first,second) == 0;
}

bool operator != (const TVector<unsigned char>& first, const TVector<unsigned char>& second) {
    return !(first == second);
}

bool operator <= (const TVector<unsigned char>& first, const TVector<unsigned char>& second) {
    return !(first > second);
}

bool operator >= (const TVector<unsigned char>& first, const TVector<unsigned char>& second) {
    return !(first < second);
}



int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

//    std::ifstream is("test_file");
//    std::basic_streambuf<char, std::char_traits<char>>* backup = std::cin.rdbuf();
//    std::cin.rdbuf(is.rdbuf());

    TAvlTree<TVector<unsigned char>, unsigned long long> tree;
    char command[264];
    while (std::cin >> command) {
        if (command[0] == '+') {
            std::cin >> command;
            TVector<unsigned char> str = strToVec(command);
            unsigned long long data;
            std::cin >> data;
            if (tree.Insert(str, data)) {
                std::cout << "OK\n";
            } else {
                std::cout << "Exist\n";
            }
        } else if (command[0] == '-') {
            std::cin >> command;
            TVector<unsigned char> str = strToVec(command);
            if (tree.Erase(str)) {
                std::cout << "OK\n";
            } else {
                std::cout << "NoSuchWord\n";
            }
        } else if (command[0] == '!') {
            std::cin >> command;
            if (strcmp(command, "Save") == 0) {
                std::cin >> command;
                tree.SaveToFile(command);
                std::cout << "OK" << "\n";
            } else if (strcmp(command, "Load") == 0) {
                std::cin >> command;
                tree.LoadFromFile(command);
                std::cout << "OK" << "\n";
            }
        } else {
            TVector<unsigned char> str = strToVec(command);
            std::pair<unsigned long long, bool> res = tree[str];
            if (res.second) {
                std::cout << "OK: " << res.first << "\n";
            } else {
                std::cout << "NoSuchWord\n";
            }
        }
        //tree.Print();
        //tree.CheckBalances();

    }

    //std::cin.rdbuf(backup);
    return 0;
}