# Windows TCP C++ Listener
This C++ listener program allows you to create a shell session over a network connection and interact with a remote client. The program was developed using the following libraries:

- **WinSock2:** This Windows-specific library is used for network communication. It allows for the creation of socket connections and data exchange over these connections.

- **Windows.h:** This library provides functions for the Windows console, necessary for UTF-8 output.

# The program provides the following features:
1. **Create Shell Session:** You can establish a shell session over a network connection with a remote client.

2. **Interactive Input and Output:** You can send commands to the remote client and display the output of executed commands.

3. **AJAX Support:** The program supports processing input and output commands via AJAX requests from a web server.

# How It Works
The program works by creating a listener socket and waiting for incoming connections. Once a connection is established, a shell session with the client is initiated. You can enter commands and view the output of those commands. The output can also be sent to a web server via AJAX requests.

# Instructions
To run the program, follow these steps:
1. Clone this repository to your local computer:

```bash
  git clone https://github.com/Hz-36/CPP_LISTENER.git
```

3. Compile the program using a C++ compiler that supports the required libraries. For example:

```bash
  g++ -o Hz36Listener main.cpp -lws2_32
```

4. Make sure to add the -lws2_32 flag to link the WinSock2 library.

3. Execute the compiled program:

```bash
  ./Hz36Listener
```

4. The program will listen for incoming connections on port 8080 by default. You can change the port number in the xStartListener function if desired.

5. You can enter commands and view the output in the console window. The output can also be sent to a web server via AJAX requests if you've added that functionality.

Please note that this program has been tested only on Windows and uses specific Windows libraries. If you intend to use it on other operating systems, modifications to the code may be necessary.

Thank you for using this listener program! If you have any questions or suggestions, please feel free to reach out.
