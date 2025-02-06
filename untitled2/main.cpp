#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
using namespace std;

string abc = "abcdefghijklmnopqrstuvwxyz"; // aąbcćdeęfghijklłmnńoópqrsśtuvwxyzźż
string name;

string nmessage_encrypted = "C:\\Users\\pc\\CLionProjects\\";

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

// https://www.geeksforgeeks.org/vigenere-cipher/
// https://www.youtube.com/watch?v=heG4Sk6STcc

string Vigenere(const string text, const string key)
{

    string decrypt;
    int keylen = key.size();
    int k = 0;

    for (char character : text)
    {

        int textid = indexes(character);

        if (textid != -1)
        {
            int keyid = indexes(key[k % keylen]);
            int decryptid = (textid - keyid + abc.size()) % abc.size();
            decrypt += letters(decryptid);
            k++;
        }
        else
        {
            decrypt += character;                                                                                       // Jeśli znak nie jest w polskim alfabecie, dodaj go bez zmian
        }
    }
    return decrypt;
}

string Xor(const string text, const string key)
{
    string decrypt = text;
    size_t keyLength = key.length();

    for (size_t i = 0; i < text.length(); ++i)
    {
        decrypt[i] ^= key[i % keyLength];
    }
    return decrypt;
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
    vector<int> tempS = S;

    int i = 0, j = 0;
    for (int k = 0; k < dataLength; ++k)
    {
        i = (i + 1) % 256;
        j = (j + tempS[i]) % 256;
        swap(tempS[i], tempS[j]);
        keyStream[k] = tempS[(tempS[i] + tempS[j]) % 256];
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

int modInverse(int a, int m)
{
    a = a % m;
    for (int x = 1; x < m; x++)
    {
        if ((a * x) % m == 1)
            return x;
    }
}

string affine(const string& cipher, const int key_a, const int key_b)
{
    int a_inv = modInverse(key_a, abc.size());

    string plain;
    for (char character : cipher)
    {
        int idx = indexes(character);
        if (idx != -1)
        {
            int decryptIdx = (a_inv * (idx - key_b + abc.size())) % abc.size();
            plain += letters(decryptIdx);
        }
        else
        {
            plain += character;
        }
    }
    return plain;
}

bool Ask(const std::string& name)
{
    return std::filesystem::exists(name);
}

string nameAsk()
{
    do
    {
        cout << "Podaj nazwe pliku do odszyfrowana: ";
        cin >> name;
        if (Ask(nmessage_encrypted+name))
        {
            return nmessage_encrypted+name;
        }
    }
    while(true);
}

int main(int argc, char* argv[])
{
    int o = -1;
    string message;
    string message_decrypted = "C:\\Users\\pc\\CLionProjects\\c.txt";

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

    if (o == -1)
    {
        cout << "0 - afiniczny \n1 - Vigenere \n2 - XOR \n3 - ARC4 \n";
        cin >> o;
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
            cout << "Podaj nazwe pliku do odszyfrowania: ";
            cin >> message;
        } while (!Ask(nmessage_encrypted + message));

        message = nmessage_encrypted + message;
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

    ofstream message_encryptedFile(message_decrypted);
    message_encryptedFile << encrypt;

    cout << "\nZapisano w: " << message_decrypted << endl;
    return 0;
}
