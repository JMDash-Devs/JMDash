#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <windows.h>
#include <cstdlib>
#include <functional>

enum class JJTokenType {
    Identifier,
    Keyword,
    Number,
    String,
    Symbol,
    Operator,
    Braces,
    EndOfFile,
    Unknown
};

struct JJToken {
    JJTokenType type;
    std::string value;
};

class JTokenizer {
public:
    JTokenizer(const std::string& input) : stream(input), currentChar(' '), pos(0) {
        nextChar();
    }

    JJToken nextJJToken() {
        skipWhitespace();
        if (stream.eof()) return JJToken{ JJTokenType::EndOfFile, "" };
        if (isalpha(currentChar)) return parseIdentifier();
        if (isdigit(currentChar)) return parseNumber();
        if (currentChar == '{' || currentChar == '}') return parseBraces();
        if (currentChar == '+' || currentChar == '-') return parseOperator();
        if (currentChar == 'i' && peek() == 'f') return parseKeyword();
        return JJToken{ JJTokenType::Unknown, std::string(1, currentChar) };
    }

private:
    std::istringstream stream;
    char currentChar;
    size_t pos;

    void nextChar() {
        if (stream.eof()) currentChar = '\0';
        else stream.get(currentChar);
    }

    char peek() {
        if (stream.eof()) return '\0';
        return stream.peek();
    }

    void skipWhitespace() {
        while (isspace(currentChar)) nextChar();
    }

    JJToken parseIdentifier() {
        std::string value;
        while (isalnum(currentChar) || currentChar == '_') {
            value += currentChar;
            nextChar();
        }
        return JJToken{ JJTokenType::Identifier, value };
    }

    JJToken parseNumber() {
        std::string value;
        while (isdigit(currentChar)) {
            value += currentChar;
            nextChar();
        }
        return JJToken{ JJTokenType::Number, value };
    }

    JJToken parseBraces() {
        std::string value(1, currentChar);
        nextChar();
        return JJToken{ JJTokenType::Braces, value };
    }

    JJToken parseOperator() {
        std::string value(1, currentChar);
        nextChar();
        return JJToken{ JJTokenType::Operator, value };
    }

    JJToken parseKeyword() {
        std::string value;
        while (isalpha(currentChar)) {
            value += currentChar;
            nextChar();
        }
        return JJToken{ JJTokenType::Keyword, value };
    }
};

enum class ErrorType {
    SyntaxError, FileError, RegistrationError, CodeGenerationError
};

class Error {
public:
    ErrorType type;
    std::string message;
    Error(ErrorType t, const std::string& msg) : type(t), message(msg) {}
    void print() const {
        std::cerr << "Error: " << message << std::endl;
    }
};

void jtry(const std::function<void()>& func) {
    try {
        func();
    }
    catch (const Error& e) {
        e.print();
    }
}

void jthrow(const Error& error) {
    throw error;
}

class Parser {
public:
    Parser(JTokenizer& tokenizer) : JTokenizer(tokenizer), currentJJToken(tokenizer.nextJJToken()) {}

    void parse() {
        while (currentJJToken.type != JJTokenType::EndOfFile) {
            processJJToken();
            currentJJToken = JTokenizer.nextJJToken();
        }
    }

private:
    JTokenizer& JTokenizer;
    JJToken currentJJToken;

    void processJJToken() {
        switch (currentJJToken.type) {
        case JJTokenType::Identifier:
            std::cout << "Parsed identifier: " << currentJJToken.value << std::endl;
            break;
        case JJTokenType::Number:
            std::cout << "Parsed number: " << currentJJToken.value << std::endl;
            break;
        case JJTokenType::Operator:
            std::cout << "Parsed operator: " << currentJJToken.value << std::endl;
            break;
        case JJTokenType::Braces:
            std::cout << "Parsed brace: " << currentJJToken.value << std::endl;
            break;
        case JJTokenType::Keyword:
            std::cout << "Parsed keyword: " << currentJJToken.value << std::endl;
            break;
        default:
            jthrow(Error(ErrorType::SyntaxError, "Unknown JJToken: " + currentJJToken.value));
            break;
        }
    }
};

class IRGenerator {
public:
    void generate(const std::vector<JJToken>& JJTokens, std::ostream& out) {
        for (const auto& JJToken : JJTokens) {
            switch (JJToken.type) {
            case JJTokenType::Identifier:
                out << "IR: Identifier - " << JJToken.value << std::endl;
                break;
            case JJTokenType::Number:
                out << "IR: Number - " << JJToken.value << std::endl;
                break;
            case JJTokenType::Operator:
                out << "IR: Operator - " << JJToken.value << std::endl;
                break;
            case JJTokenType::Braces:
                out << "IR: Braces - " << JJToken.value << std::endl;
                break;
            case JJTokenType::Keyword:
                out << "IR: Keyword - " << JJToken.value << std::endl;
                break;
            default:
                out << "IR: Unknown JJToken - " << JJToken.value << std::endl;
                break;
            }
        }
    }
};

class CodeGenerator {
public:
    void generateExecutable(const std::string& assemblyCode, const std::string& exePath) {
        std::string assemblyFilePath = exePath + ".asm";
        std::ofstream asmFile(assemblyFilePath);
        if (!asmFile.is_open()) {
            jthrow(Error(ErrorType::FileError, "Failed to create assembly file"));
        }

        asmFile << assemblyCode;
        asmFile.close();

        std::string command = "gcc " + assemblyFilePath + " -o " + exePath;
        int result = std::system(command.c_str());
        if (result != 0) {
            jthrow(Error(ErrorType::CodeGenerationError, "Failed to generate executable"));
        }

        std::cout << "Executable generated at: " << exePath << std::endl;
    }
};

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        jthrow(Error(ErrorType::FileError, "Cannot open file: " + filePath));
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        jthrow(Error(ErrorType::FileError, "Cannot open file: " + filePath));
    }
    file << content;
}

void registerJmdFileExtension() {
    HKEY hKey;
    const char* subKey = "Software\\Classes\\.jmdpjf";
    const char* value = "JMDashProjectFile";

    if (RegCreateKeyExA(HKEY_CURRENT_USER, subKey, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, NULL, 0, REG_SZ, reinterpret_cast<const BYTE*>(value), strlen(value));
        RegCloseKey(hKey);
        std::cout << "Successfully registered .jmdpjf file extension." << std::endl;
    }
    else {
        jthrow(Error(ErrorType::RegistrationError, "Failed to register .jmdpjf file extension"));
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--register-jmd-file-extension") {
        jtry(registerJmdFileExtension);
        return 0;
    }

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input file> <output executable>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];  // Input file path
    std::string exePath = argv[2];   // Output executable path

    jtry([&]() {
        std::string code = readFile(filePath);
        JTokenizer tokenizer(code);
        Parser parser(tokenizer);
        parser.parse();

        std::vector<JJToken> tokens;  // Update with actual tokens if needed
        IRGenerator irGenerator;
        std::ostringstream irOutput;
        irGenerator.generate(tokens, irOutput);

        writeFile("output.ir", irOutput.str());

        CodeGenerator codeGenerator;
        codeGenerator.generateExecutable(irOutput.str(), exePath);
        });

    return 0;
}