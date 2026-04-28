🔐 Multi-Cipher Encryption Tool (C++)

A console-based C++ application that implements multiple classical encryption algorithms with support for file input/output, multithreading, and performance measurement.

🚀 Features
Multiple cipher algorithms:
Caesar Cipher
Rail Fence Cipher
Reversed Rail Fence Cipher
Cardano (Grille) Cipher
Vigenère Cipher (block-based, multithreaded)
Affine Cipher (block-based, multithreaded)
Encrypt and decrypt functionality
Manual input or file-based processing
Multithreading support for block processing (Vigenère, Affine)
Execution time measurement
Export results to file
⚙️ How It Works
User selects cipher type
Chooses mode (encrypt / decrypt)
Provides input (manual or file)
Program processes text using selected algorithm
Result is displayed or saved to file
Execution time is measured using chrono
🧠 Architecture

The project is built around an abstract base class:

class Cipher {
    virtual string encrypt(const string& text) = 0;
    virtual string decrypt(const string& text) = 0;
};

Each cipher is implemented as a separate class:

CaesarCipher
RailFenceCipher
ReversedRailFenceCipher
CardanoCipher
VigenereCipher
AffineCipher

This allows easy extensibility and clean polymorphism.

🧵 Multithreading

For performance optimization:

Vigenère Cipher and Affine Cipher split input into blocks
Each block is processed in a separate std::thread
Results are merged into a shared output buffer
📌 Example Usage
CIPHER TOOL
Select cipher: (2) Caesar
Mode: Encrypt
Input: Hello World
Shift: 3

Output: Khoor Zruog
📂 Input Options
Manual text input
File input (reads entire file into memory)
💾 Output Options
Print result to console
Save result to external file
🛠️ Technologies
C++17
STL (vector, string, thread, mutex, fstream)
Multithreading (std::thread)
Chrono library for benchmarking
💡 Key Concepts Used
Object-Oriented Programming (OOP)
Polymorphism (Cipher base class)
Classical cryptography algorithms
Multithreading and parallel processing
File I/O
Performance measurement
⚠️ Notes
Affine cipher requires a to be coprime with 26
Cardano cipher requires valid binary matrix key
No encryption for non-alphabetic characters (they remain unchanged)
🚀 Possible Improvements
Add modern encryption algorithms (AES, RSA)
Improve thread safety with mutex/atomic optimizations
GUI interface (Qt or ImGui)
Streaming file processing for large inputs
Better key validation and error handling
👨‍💻 Purpose

This project was developed to practice:

cryptographic algorithms
object-oriented design in C++
multithreading and performance optimization
real-world CLI tool architecture
