#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <numeric>

using namespace std;

const string alphabetup = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string alphabetdown = "abcdefghijklmnopqrstuvwxyz";


class Cipher
{
public:
    virtual string encrypt(const string& text) = 0;
    virtual string decrypt(const string& text) = 0;
    virtual ~Cipher() {}
};


class CaesarCipher : public Cipher
{
    int shift;
public:
    CaesarCipher(int s) : shift(s) {}
    string encrypt(const string& text) override
    {
        string result;
        for (char ch : text)
        {
            size_t pos;
            if ((pos = alphabetup.find(ch)) != string::npos)
                result += alphabetup[(pos + shift) % alphabetup.size()];
            else if ((pos = alphabetdown.find(ch)) != string::npos)
                result += alphabetdown[(pos + shift) % alphabetdown.size()];
            else
                result += ch;
        }
        return result;
    }
    string decrypt(const string& text) override
    {
        string result;
        for (char ch : text)
        {
            size_t pos;
            if ((pos = alphabetup.find(ch)) != string::npos)
                result += alphabetup[(pos + alphabetup.size() - shift) % alphabetup.size()];
            else if ((pos = alphabetdown.find(ch)) != string::npos)
                result += alphabetdown[(pos + alphabetdown.size() - shift) % alphabetdown.size()];
            else
                result += ch;
        }
        return result;
    }
};


class RailFenceCipher : public Cipher
{
    int key;
public:
    RailFenceCipher(int k) : key(k) {}
    string encrypt(const string& text) override
    {
        if (key <= 1) return text;
        vector<string> rail(key);
        int row = 0, step = 1;
        for (char ch : text)
        {
            rail[row] += ch;
            row += step;
            if (row == 0 || row == key - 1) step *= -1;
        }
        string result;
        for (const string& line : rail) result += line;
        return result;
    }
    string decrypt(const string& cipher) override
    {
        if (key <= 1) return cipher;
        vector<int> rail(cipher.length());
        int row = 0, step = 1;
        for (size_t i = 0; i < cipher.length(); ++i)
        {
            rail[i] = row;
            row += step;
            if (row == 0 || row == key - 1) step *= -1;
        }
        vector<string> lines(key, string(cipher.length(), ' '));
        size_t index = 0;
        for (int r = 0; r < key; ++r)
            for (size_t i = 0; i < cipher.length(); ++i)
                if (rail[i] == r)
                    lines[r][i] = cipher[index++];
        string result;
        for (size_t i = 0; i < cipher.length(); ++i)
            result += lines[rail[i]][i];
        return result;
    }
};


class ReversedRailFenceCipher : public Cipher
{
    int key;
public:
    ReversedRailFenceCipher(int k) : key(k) {}
    string encrypt(const string& text) override
    {
        if (key <= 1) return text;
        vector<string> rail(key);
        int row = key - 1, step = -1;
        for (char ch : text)
        {
            rail[row] += ch;
            row += step;
            if (row == 0 || row == key - 1) step *= -1;
        }
        string result;
        for (const string& line : rail) result += line;
        return result;
    }
    string decrypt(const string& cipher) override
    {
        if (key <= 1) return cipher;
        vector<int> rail(cipher.length());
        int row = key - 1, step = -1;
        for (size_t i = 0; i < cipher.length(); ++i)
        {
            rail[i] = row;
            row += step;
            if (row == 0 || row == key - 1) step *= -1;
        }
        vector<string> lines(key, string(cipher.length(), ' '));
        size_t index = 0;
        for (int r = 0; r < key; ++r)
            for (size_t i = 0; i < cipher.length(); ++i)
                if (rail[i] == r)
                    lines[r][i] = cipher[index++];
        string result;
        for (size_t i = 0; i < cipher.length(); ++i)
            result += lines[rail[i]][i];
        return result;
    }
};


typedef vector<vector<bool>> CardanoKey;

class CardanoCipher : public Cipher
{
    CardanoKey key;
    int n;
    CardanoKey rotateKey(const CardanoKey& key)
    {
        CardanoKey rotated(n, vector<bool>(n));
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                rotated[j][n - i - 1] = key[i][j];
        return rotated;
    }
public:
    CardanoCipher(const CardanoKey& k) : key(k), n(k.size()) {}
    string encrypt(const string& text) override
    {
        string padded = text;
        while (padded.size() < n * n) padded += ' ';
        string result(n * n, ' ');
        CardanoKey currentKey = key;
        int idx = 0;
        for (int r = 0; r < 4; ++r)
        {
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    if (currentKey[i][j])
                        result[i * n + j] = padded[idx++];
            currentKey = rotateKey(currentKey);
        }
        return result;
    }
    string decrypt(const string& cipher) override
    {
        string result;
        CardanoKey currentKey = key;
        vector<vector<char>> matrix(n, vector<char>(n));
        for (int i = 0; i < n * n; ++i)
            matrix[i / n][i % n] = cipher[i];
        for (int r = 0; r < 4; ++r)
        {
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    if (currentKey[i][j])
                        result += matrix[i][j];
            currentKey = rotateKey(currentKey);
        }
        return result;
    }
};


class VigenereCipher : public Cipher
{
    int shift, blockSize;
    static char processChar(char c, int shift, bool encrypt)
    {
        if (isalpha(c))
        {
            char base = islower(c) ? 'a' : 'A';
            return encrypt ? (c - base + shift) % 26 + base : (c - base - shift + 26) % 26 + base;
        }
        return c;
    }
    static void processBlock(const string& block, int shift, bool encrypt, string& result, int index)
    {
        string processed;
        for (char c : block) processed += processChar(c, shift, encrypt);
        result.replace(index, block.size(), processed);
    }
public:
    VigenereCipher(int s, int b) : shift(s), blockSize(b) {}
    string encrypt(const string& text) override
    {
        return process(text, true);
    }
    string decrypt(const string& text) override
    {
        return process(text, false);
    }
    string process(const string& text, bool encrypt)
    {
        if (blockSize <= 0)
        {
            string result = text;
            processBlock(text, shift, encrypt, result, 0);
            return result;
        }
        string result = text;
        vector<thread> threads;
        for (size_t i = 0; i < text.size(); i += blockSize)
        {
            string block = text.substr(i, blockSize);
            threads.emplace_back(processBlock, block, shift, encrypt, ref(result), i);
        }
        for (auto& t : threads) t.join();
        return result;
    }
};


class AffineCipher : public Cipher
{
    int a, b, blockSize;
    static int modInverse(int a, int m)
    {
        a %= m;
        for (int x = 1; x < m; ++x)
            if ((a * x) % m == 1)
                return x;
        return -1;
    }
    static char processChar(char c, int a, int b, bool encrypt)
    {
        if (isalpha(c))
        {
            char base = islower(c) ? 'a' : 'A';
            int x = c - base;
            return encrypt ? (a * x + b) % 26 + base : (modInverse(a, 26) * (x - b + 26)) % 26 + base;
        }
        return c;
    }
    static void processBlock(const string& block, int a, int b, bool encrypt, string& result, int index)
    {
        string processed;
        for (char c : block) processed += processChar(c, a, b, encrypt);
        result.replace(index, block.size(), processed);
    }
public:
    AffineCipher(int a_, int b_, int bs) : a(a_), b(b_), blockSize(bs) {}
    string encrypt(const string& text) override
    {
        return process(text, true);
    }
    string decrypt(const string& text) override
    {
        return process(text, false);
    }
    string process(const string& text, bool encrypt)
    {
        if (blockSize <= 0)
        {
            string result = text;
            processBlock(text, a, b, encrypt, result, 0);
            return result;
        }
        string result = text;
        vector<thread> threads;
        for (size_t i = 0; i < text.size(); i += blockSize)
        {
            string block = text.substr(i, blockSize);
            threads.emplace_back(processBlock, block, a, b, encrypt, ref(result), i);
        }
        for (auto& t : threads) t.join();
        return result;
    }
};


int main()
{
    cout << "CIPHER TOOL\n";
    cout << "Select cipher: (1) Rail Fence, (2) Caesar, (3) Cardano, (4) Vigenere, (5) Affine, (6) Reversed Rail Fence\n> ";
    int method; cin >> method;

    cout << "Mode: (1) Encrypt, (2) Decrypt\n> ";
    int modeChoice; cin >> modeChoice;
    bool encrypt = (modeChoice == 1);

    cout << "Input source: (1) Manual, (2) File\n> ";
    int inputMode; cin >> inputMode;
    string inputText;
    if (inputMode == 1)
    {
        cout << "Enter text:\n> ";
        cin.ignore(); getline(cin, inputText);
    }
    else {
        string filename;
        cout << "Enter file name:\n> ";
        cin >> filename;
        ifstream inFile(filename);
        inputText.assign((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    }

    Cipher* cipher = nullptr;

    if (method == 1)
    {
        int key;
        cout << "Enter rail height:\n> ";
        cin >> key;
        cipher = new RailFenceCipher(key);
    }
    else if (method == 2)
    {
        int shift;
        cout << "Enter Caesar shift:\n> ";
        cin >> shift;
        cipher = new CaesarCipher(shift);
    }
    else if (method == 3)
    {
        int n;
        cout << "Enter matrix size:\n> ";
        cin >> n;
        CardanoKey key(n, vector<bool>(n));
        cout << "Enter " << n * n << " values (0 or 1):\n";
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
            {
                int temp; cin >> temp;
                key[i][j] = static_cast<bool>(temp);
            }
        cipher = new CardanoCipher(key);
    }
    else if (method == 4)
    {
        int shift, blockSize;
        cout << "Enter shift:\n> ";
        cin >> shift;
        cout << "Enter block size:\n> ";
        cin >> blockSize;
        cipher = new VigenereCipher(shift, blockSize);
    }
    else if (method == 5)
    {
        int a, b, blockSize;
        cout << "Enter key a (coprime with 26):\n> ";
        cin >> a;
        cout << "Enter key b:\n> ";
        cin >> b;
        cout << "Enter block size:\n> ";
        cin >> blockSize;
        cipher = new AffineCipher(a, b, blockSize);
    }
    else if (method == 6)
    {
        int key;
        cout << "Enter rail height:\n> ";
        cin >> key;
        cipher = new ReversedRailFenceCipher(key);
    }


    auto start = chrono::high_resolution_clock::now();
    string output = encrypt ? cipher->encrypt(inputText) : cipher->decrypt(inputText);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "\nOperation completed in " << elapsed.count() << " seconds.\n";

    cout << "Export result to file? (1) Yes, (2) No\n> ";
    int expOut; cin >> expOut;
    if (expOut == 1)
    {
        string outFile;
        cout << "Enter output file name:\n> ";
        cin >> outFile;
        ofstream fout(outFile);
        fout << output;
        cout << "Saved.\n";
    }
    else
    {
        cout << "\nResult:\n" << output << "\n";
    }

    delete cipher;
    return 0;
}