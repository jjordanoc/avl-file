#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

struct Alumno {
    char key[20];
    char ciclo[10];
};

ostream &operator<<(ostream &stream, Alumno &p) {
    stream << p.key << " ";
    stream << p.ciclo;
    stream << "\n";
    stream << flush;
    return stream;
}

istream &operator>>(istream &stream, Alumno &p) {
    stream.read(p.key, 20);
    p.key[19] = '\0';
    stream.read(p.ciclo, 10);
    p.ciclo[9] = '\0';
    stream.get();
    return stream;
}


template<typename KeyType, typename RecordType>
class AVLFile {
//    typedef KeyType(*KeyExtractionFunction)(RecordType);
//    typedef int(*NodeExtractionFunction)(RecordType);
    string filename;
    string headerFilename;

//    KeyExtractionFunction getKey;
//    NodeExtractionFunction getNode;
    int _search(KeyType key, int nodePos) {
        if (nodePos == -1) {
            // element not found
            return -1;
        }
        ifstream file(filename);
        size_t realPos = _physicalPosition(nodePos);
        file.seekg(realPos);
        RecordType node;
        file >> node;
        file.close();
        if (node.key > key) {
            return _search(key, node.left);
        } else if (node.key < key) {
            return _search(key, node.right);
        } else {
            // element was found
            return realPos;
        }
    }

    void _insert(RecordType &record, int nodePos, int parentPos, bool left) {
        ifstream file(filename);
        if (nodePos == -1 || file.peek() == std::ifstream::traits_type::eof()) {
            file.close();
            // element not found (insert)
            ofstream out(filename, ios::app | ios::ate);
            // save position
            size_t physicalRecordPos = out.tellp();
            ofstream headerOut(headerFilename, ios::app);
            headerOut << physicalRecordPos << "\n";
            size_t lastInsertedPos = headerOut.tellp();
            cout << headerOut.tellp() << " " << sizeof(size_t) << endl;
            headerOut.close();
            // write record at the end of file
            out << record;
            out.close();
            // look for parent (if exists)
            if (parentPos != -1) {
                fstream f(filename);
                f.seekg(_physicalPosition(parentPos));
                RecordType parent;
                f >> parent;
                // update parent node
                if (left) {
                    parent.left = lastInsertedPos - 2;
                } else {
                    parent.right = lastInsertedPos - 2;
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
        if (node.key >= record.key) {
            return _insert(record, node.left, nodePos, true);
        } else if (node.key < record.key) {
            return _insert(record, node.right, nodePos, false);
        }
    }

    size_t _physicalPosition(size_t logicalPosition) {
        ifstream headerFile(headerFilename);
        headerFile.seekg(sizeof(size_t) * logicalPosition);
        size_t physicalPos;
        headerFile >> physicalPos;
        headerFile.close();
        return physicalPos;
    }

public:
    AVLFile(const string &_filename) : filename(_filename) {
        // ensure file is created
        headerFilename = filename + "_header.txt";
        ofstream file(filename, ios::app);
        file.close();
        ofstream headerFile(headerFilename, ios::app);
        headerFile.close();
    }

    bool add(RecordType &record) {
        _insert(record, 0, -1, false);
    }

    vector<RecordType> search(KeyType key) {

    }

};

void menu() {
    AVLFile<string, Alumno> avlFile("data.txt");
    bool exit = false;
    while (!exit) {
        cout << "load(l)\n";
        cout << "read(r)\n";
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
        } else if (op == 'r') {
//            int pos;
//            cout << "pos: ";
//            cin >> pos;
//            Alumno alumno = avlFile.readRecord(pos);
//            if (read) {
//                cout << "Codigo:" << alumno.codigo << endl;
//                cout << "Nombre:" << alumno.nombre << endl;
//                cout << "Apellidos:" << alumno.apellidos << endl;
//                cout << "Carrera:" << alumno.carrera << endl;
//            }
        } else if (op == 'a') {
            Alumno alumno;
            cout << "Nombre: ";
            cin >> alumno.key;
            cout << "Ciclo: ";
            cin >> alumno.ciclo;
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
