#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <ctime>
#include <random>
#include <algorithm>

#include "camellia.h"

#define DEFAULT_KEY_FILE "key.bin"
#define DEFAULT_SIZE_FILE "size.bin"

using namespace::std;

vector<uint8_t> read(string filename) {
    ifstream myfile(filename, ios::binary);
    vector<uint8_t> data((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>());
    myfile.close();
    return data;
}

void hexdump(vector<uint8_t> data_vec) {
    uint8_t data[data_vec.size()];
    copy(data_vec.begin(), data_vec.end(), data);

    uint32_t width = 0;
    uint32_t length = data_vec.size();
    for (uint8_t *p = data; length > 0; p++) {
        if (width >= 16) {
            cout << endl;
            width = 0;
        }
        printf("%02x ", *p);
        --length;
        ++width;
    }
    cout << endl;
}

vector<uint8_t> search_for_key(int key_len) {
    vector<uint8_t> key;

    if (DEFAULT_KEY_FILE) {
        key = read(DEFAULT_KEY_FILE);
    }
    
    return key;
}

void save_to(vector<uint8_t> data, string output_filename, int size) {
    ofstream outfile(output_filename, ios::out | ios::binary);
    if(size != 0) { 
        outfile.write((char *)&data[0], size);
    } else {
        outfile.write((char *)&data[0], data.size());
    }
}

vector<uint8_t> generate_key(int key_len) {
    srand(time(nullptr));
    vector<uint8_t> res;
    uint8_t KEY_SIZE = key_len;

    for (int i = 0; i < KEY_SIZE; i++) {
        res.push_back(rand() & 0xFF);
    }

    return res;
}

vector<uint8_t> encrypt(vector<uint8_t> &data, vector<uint8_t> &key) {
    uint8_t ekey[272] = {0};
    uint8_t ukey[key.size()];
    uint32_t keysize = key.size() * 8;

    copy(key.begin(), key.end(), ukey);

    Camellia::ekeygen(ukey, ekey, keysize);

    vector<uint8_t> result;
    for (int i = 0; i < data.size(); i += 16) {
        uint8_t tmp[16] = {0};
        uint8_t res[16] = {0};
        copy(data.begin() + i, data.begin() + i + 16, tmp);
        Camellia::encrypt(tmp, ekey, res, keysize);

        for (int j = 0; j < 16; j++) {
            result.push_back(res[j]);
        }
    }
    return result;
}

vector<uint8_t> decrypt(vector<uint8_t> &data, vector<uint8_t> &key) {
    uint8_t ekey[272] = {0};
    uint8_t ukey[key.size()];
    uint32_t keysize = key.size() * 8;

    copy(key.begin(), key.end(), ukey);

    Camellia::ekeygen(ukey, ekey, keysize);

    vector<uint8_t> result;
    for (int i = 0; i < data.size(); i += 16) {
        uint8_t tmp[16] = {0};
        uint8_t res[16] = {0};
        copy(data.begin() + i, data.begin() + i + 16, tmp);
        Camellia::decrypt(tmp, ekey, res, keysize);

        for (int j = 0; j < 16; j++) {
            result.push_back(res[j]);
        }
    }
    return result;
}

int main(int argc, char **argv) {
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
    // breaf tutorial
    // argv[1] - what to do: e or d
    // argv[2] - from which file
    // argv[3] - to which file
    // argv[4] - key len not strongly required
    // key and size of a file
    // we read from 2 files
    // -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
    if(argc < 4){
        cout << "Incorrect entry data" << endl;
        return 1;
    }

    cout << argv[1] << " " << argv[2] << endl;

    string filename = argv[2];
    vector<uint8_t> data = read(filename);
    vector<uint8_t> key;

    cout << data.size() << endl;

    if(!strcmp(argv[1], "encrypt")) {
        int key_len = atoi(argv[4]);
        if(key_len % 8 != 0 || key_len > 32) {
            cout << "Uncorrect key lenght\n"<<
            "Generated 128-bit key" << endl;
            key_len = 16;   // if uncorrect key_len we gen the min len
        }

        ifstream iff(DEFAULT_KEY_FILE);
        iff.seekg(0, ios::end);
        if(!iff || iff.tellg() != key_len) {
            cout << "gen key" << endl;
            key = generate_key(key_len);
            save_to(key, DEFAULT_KEY_FILE, 0);
        } else {
            cout << "read file" << endl;
            key = read(DEFAULT_KEY_FILE);
        }

        hexdump(key);
        ofstream ost(DEFAULT_SIZE_FILE);
        ost << data.size();
        cout << "Key len: " << key.size() << endl;
        // -_-_-_-_-_-_-_-_-_-_-_-_-_-_
        cout << "Size of file saved to:" << DEFAULT_SIZE_FILE << endl;

        vector<uint8_t> encrypted = encrypt(data, key);
        string output_file = argv[3];
        save_to(encrypted, output_file, 0);
        // -_-_-_-_-_-_-_-_-_-_-_-_-_-_
        cout << "Encoded to: " + output_file << endl;
    } else if(!strcmp(argv[1], "decrypt")) {
        int key_len = atoi(argv[4]);
        if(key_len % 8 != 0 || key_len > 32) {
            cout << "Uncorrect key length" << endl;
            return -1;
        }

        key = search_for_key(key_len);
        if(key.empty()) {
            cout << "Key not found by default path\n"<<
            "Please create a " << DEFAULT_KEY_FILE << " whith correct key!" << endl;
            return -1; 
        }

        int size;
        ifstream size_file(DEFAULT_SIZE_FILE);
        size_file >> size;
        size_file.close();

        vector<uint8_t> decrypted = decrypt(data, key);
        string output_file = argv[3];
        save_to(decrypted, output_file, size);
        cout << "Decrypted to: " + output_file << endl;      
    } else {
        cout << "Incorrect option" << endl;
    }
    return 0;
}