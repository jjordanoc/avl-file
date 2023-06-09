#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <sstream>

using namespace std;

struct Alumno {
    char key[20];
    char ciclo[10];
    char left[10];
    char right[10];
    char next[10];
};

void readFromConsole(char buffer[], int size) {
    string temp;
    cin >> temp;
    for (int i = 0; i < size; i++)
        buffer[i] = (i < temp.size()) ? temp[i] : ' ';
    buffer[size - 1] = '\0';
    cin.clear();
}

ostream &operator<<(ostream &stream, Alumno &p) {
    stream << p.key << " ";
    stream << p.ciclo << " ";
    stream << p.left << " ";
    stream << p.right << " ";
    stream << p.next << " ";
    stream << flush;
    return stream;
}

istream &operator>>(istream &stream, Alumno &p) {
    stream.read(p.key, 20);
    p.key[19] = '\0';
    stream.read(p.ciclo, 10);
    p.ciclo[9] = '\0';
    stream.read(p.left, 10);
    p.left[9] = '\0';
    stream.read(p.right, 10);
    p.right[9] = '\0';
    stream.read(p.next, 10);
    p.next[9] = '\0';
    return stream;
}


template<typename KeyType, typename RecordType>
class AVLFile {
//    typedef KeyType(*KeyExtractionFunction)(RecordType);
//    typedef int(*NodeExtractionFunction)(RecordType);
    string filename;
    string headerFilename;

//    KeyExtractionFunction getKey;
//    NodeExtractionFunction getNode
    void _search(KeyType key, int nodePos, vector<RecordType> &result, bool flag) {
        if (nodePos == -1) {
            // element not found
            return;
        }
        ifstream file(filename);
        size_t realPos = _physicalPosition(nodePos);
        file.seekg(realPos);
        RecordType node;
        file >> node;
        file.close();
        if (string(node.key) > string(key)) {
            _search(key, stoi(node.left), result, flag);
        } else if (string(node.key) < string(key)) {
            _search(key, stoi(node.right), result, flag);
        } else {
            // element was found
            result.push_back(node);
            while (stoi(node.next) != -1) {
                ifstream in(filename);
                in.seekg(_physicalPosition(stoi(node.next)));
                in >> node;
                result.push_back(node);
            }
        }
    }

    void _insert(RecordType &record, int nodePos, int parentPos, bool left) {
        ifstream file(filename);
        if (nodePos == -1 || file.peek() == std::ifstream::traits_type::eof()) {
            file.close();
            // element not found (insert)
            ofstream out(filename, ios::app | ios::ate);
            string tmp = string(9, ' ');
            tmp[0] = '-';
            tmp[1] = '1';
            strcpy(record.left, tmp.c_str());
            strcpy(record.right, tmp.c_str());
            strcpy(record.next, tmp.c_str());
            out << record;
            out.close();
            // look for parent (if exists)
            if (parentPos != -1) {
                fstream f(filename);
                f.seekg(_physicalPosition(parentPos));
                RecordType parent;
                f >> parent;
                // update parent node
                tmp = to_string(fileSize() - 1);
                ostringstream ss;
                ss << std::left << setfill(' ') << setw(9);
                ss << tmp;
                tmp = ss.str();
                if (left) {
                    strcpy(parent.left, tmp.c_str());
                } else {
                    strcpy(parent.right, tmp.c_str());
                }
                f.seekp(_physicalPosition(parentPos));
                f << parent;
                f.close();
            }
            return;
        }
        size_t realPos = _physicalPosition(nodePos);
        file.seekg(realPos);
        RecordType node;
        file >> node;
        file.close();
        // note that we're inserting equal elements to the left of the node to which the key is equal
        if (string(node.key) > string(record.key)) {
            return _insert(record, stoi(node.left), nodePos, true);
        } else if (string(node.key) < string(record.key)) {
            return _insert(record, stoi(node.right), nodePos, false);
        } else {
            // element not found (insert)
            fstream out(filename, ios::ate | ios::out | ios::in);
            cout << out.tellp() << endl;
            string tmp = string(9, ' ');
            tmp[0] = '-';
            tmp[1] = '1';
            strcpy(record.left, tmp.c_str());
            strcpy(record.right, tmp.c_str());
            strcpy(record.next, node.next);
            cout << out.tellp() << endl;
            out << record;
            cout << out.tellp() << endl;
            tmp = to_string(fileSize() - 1);
            ostringstream ss;
            ss << std::left << setfill(' ') << setw(9);
            ss << tmp;
            tmp = ss.str();
            out.seekp(_physicalPosition(nodePos));
            strcpy(node.next, tmp.c_str());
            out << node;
            out.close();
            return;
        }
    }

    size_t _physicalPosition(size_t logicalPosition) {
        return logicalPosition * sizeof(Alumno);
    }

    size_t fileSize() {
        ifstream infile(filename);
        infile.seekg(0, ios::end);
        return infile.tellg() / sizeof(Alumno);
    }

public:
    AVLFile(const string &_filename) : filename(_filename) {
        // ensure file is created
        ofstream file(filename, ios::app);
        file.close();
    }

    bool add(RecordType &record) {
        _insert(record, 0, -1, false);
    }

    vector<RecordType> search(KeyType key) {
        vector<RecordType> result;
        _search(key, 0, result, false);
        return result;
    }

};

void menu() {
    AVLFile<string, Alumno> avlFile("data.txt");
    bool exit = false;
    while (!exit) {
        cout << "load(l)\n";
        cout << "search(s)\n";
        cout << "add(a)\n";
        cout << "exit(e)\n";
        char op;
        cout << "elige operacion: ";
        cin >> op;
        if (op == 'l') {
//            vector<Alumno> alumnos = avlFile.load();
//            for (auto &tmp: alumnos) {
//                cout << "Nombre:" << tmp.key << endl;
//                cout << "Ciclo:" << tmp.ciclo << endl;
//            }
        } else if (op == 's') {
            char key[20];
            cout << "Nombre: ";
            readFromConsole(key, 20);
            auto alumnos = avlFile.search(key);
            for (const auto &a: alumnos) {
                cout << a.key << "|" << a.ciclo << "|" << a.left << "|" << a.right << "|" << a.next << endl;
            }
        } else if (op == 'a') {
            Alumno alumno;
            cout << "Nombre: ";
            readFromConsole(alumno.key, 20);
            cout << "Ciclo: ";
            readFromConsole(alumno.ciclo, 10);
            avlFile.add(alumno);
        } else if (op == 'e') {
            exit = true;
        }
    }
}


int main() {
    menu();
    return 0;
}
