#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
using namespace std;

string abc = "abcdefghijklmnopqrstuvwxyz"; // aąbcćdeęfghijklłmnńoópqrsśtuvwxyzźż
string nmessage = "C:\\Users\\pc\\CLionProjects\\";

int indexes(char character)
{
    for (int i = 0; i < abc.size(); ++i)
    {
        if (character == abc[i]) return i;
    }
    return -1;
}

char letters(int idx)
{
    if (idx >= 0 && idx < abc.size())
    {
        return abc[idx];
    }
    return ' ';
}

string Vigenere(const string text, const string key)
{
    string encrypt;
    int keylen = key.size();
    int k = 0;

    for (char character : text)
    {
        int textid = indexes(character);

        if (textid != -1)
        {
            int keyid = indexes(key[k % keylen]);
            int encryptid = (textid + keyid) % abc.size();
            encrypt += letters(encryptid);
            k++;
        }
        else
        {
            encrypt += character; // Jeśli znak nie jest w polskim alfabecie, dodaj go bez zmian
        }
    }
    return encrypt;
}

string Xor(const string text, const string key)
{
    string encrypt = text;
    size_t keyLength = key.length();

    for (size_t i = 0; i < text.length(); ++i)
    {
        encrypt[i] ^= key[i % keyLength];
    }
    return encrypt;
}

void initKey(const string key, vector<int>&S)
{
    int keyLength = key.size();
    for (int i = 0; i < 256; ++i)
    {
        S[i] = i;
    }

    int j = 0;
    for (int i = 0; i < 256; ++i)
    {
        j = (j + S[i] + key[i % keyLength]) % 256;
        swap(S[i], S[j]);
    }
}

vector<int> genKey(const vector<int>&S, int dataLength)
{
    vector<int> keyStream(dataLength);
    vector<int> temp = S;

    int i = 0, j = 0;
    for (int k = 0; k < dataLength; ++k)
    {
        i = (i + 1) % 256;
        j = (j + temp[i]) % 256;
        swap(temp[i], temp[j]);
        keyStream[k] = temp[(temp[i] + temp[j]) % 256];
    }

    return keyStream;
}

string arc4(const string data, const string key)
{
    vector<int> S(256);
    initKey(key, S);

    vector<int> keyStream = genKey(S, data.size());
    string result = data;

    for (size_t i = 0; i < data.size(); ++i)
    {
        result[i] ^= keyStream[i];
    }

    return result;
}

string affine(const string& text, const int key_a, const int key_b)
{
    string cipher;
    for (char character : text)
    {
        int idx = indexes(character);
        if (idx != -1)
        {
            int encryptIdx = (key_a * idx + key_b) % abc.size();
            cipher += letters(encryptIdx);
        }
        else
        {
            cipher += character;
        }
    }
    return cipher;
}

bool Ask(const std::string& name)
{
    return std::filesystem::exists(name);
}

int main(int argc, char* argv[])
{
    int o = -1;
    string message;
    string message_encrypted = "C:\\Users\\pc\\CLionProjects\\b.txt";

    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "--alg" && i + 1 < argc)
        {
            o = stoi(argv[++i]);
        }
        else if (string(argv[i]) == "--plik" && i + 1 < argc)
        {
            message = argv[++i];
        }
    }

    while (o < 0 || o > 3)
    {
        cout << "0 - afiniczny \n1 - Vigenere \n2 - XOR \n3 - ARC4 \n";
        cin >> o;

        if (o < 0 || o > 3)
        {
            cout << "Error. Podaj 0, 1, 2 lub 3\n";
        }
    }

    if (message.empty())
    {
        do
        {
            cout << "Podaj nazwe pliku do zaszyfrowania: ";
            cin >> message;
        } while (!Ask(nmessage + message));

        message = nmessage + message;
    }

    ifstream messageFile(message);
    if (!messageFile.is_open())
    {
        cout << "\nNie znaleziono pliku" << endl;
        return 1;
    }

    string read((istreambuf_iterator<char>(messageFile)), istreambuf_iterator<char>());
    string encrypt;

    if (o == 0)
    {
        int key_a, key_b;
        cout << "Podaj klucz a (liczbowy): ";
        cin >> key_a;
        cout << "Podaj klucz b (liczbowy): ";
        cin >> key_b;
        encrypt = affine(read, key_a, key_b);
    }
    else if (o == 1)
    {
        string key;
        cout << "Podaj klucz: ";
        cin >> key;
        encrypt = Vigenere(read, key);
    }
    else if (o == 2)
    {
        string key;
        cout << "Podaj klucz: ";
        cin >> key;
        encrypt = Xor(read, key);
    }
    else if (o == 3)
    {
        string key;
        cout << "Podaj klucz: ";
        cin >> key;
        encrypt = arc4(read, key);
    }

    ofstream message_encryptedFile(message_encrypted);
    message_encryptedFile << encrypt;

    cout << "\nZapisano w: " << message_encrypted << endl;
    return 0;
}
