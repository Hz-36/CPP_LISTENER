#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

void xStartListener(int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        return;
    }

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listener, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed!" << std::endl;
        closesocket(listener);
        WSACleanup();
        return;
    }

    if (listen(listener, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed!" << std::endl;
        closesocket(listener);
        WSACleanup();
        return;
    }

    std::cout << "Listening for incoming connections on port " << port << "..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(listener, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed!" << std::endl;
            closesocket(listener);
            WSACleanup();
            return;
        }

        std::cout << "Received an incoming connection!" << std::endl;

        char buffer[1024];
        int bytesRead;

        // Separate Schleife für die Eingabe vom Benutzer
        while (true) {
            std::string userInput;
            std::cout << "Enter a command: ";
            std::getline(std::cin, userInput);

            // Befehl an den Client senden
            send(clientSocket, userInput.c_str(), userInput.length(), 0);

            if (userInput == "exit") {
                break; // Beende die Eingabeschleife, wenn der Benutzer "exit" eingibt
            }
        }

        // Schleife zum Empfangen der Ausgabe vom Client
        while (true) {
            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead <= 0) {
                std::cerr << "Connection closed or error." << std::endl;
                closesocket(clientSocket);
                break;
            }

            // Ausgabe der empfangenen Daten
            std::string command(buffer, bytesRead);
            std::cout << "Received output: " << command;
        }

        closesocket(clientSocket);
    }

    closesocket(listener);
    WSACleanup();
}

int main() {
    xStartListener(8080); // Rufe die Listener-Funktion mit dem gewünschten Port auf
    return 0;
}
