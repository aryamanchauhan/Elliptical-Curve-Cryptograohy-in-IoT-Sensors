#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // Initialize server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address
    serverAddr.sin_port = htons(8080); // Port 8080

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, addrSize) < 0) {
        cerr << "Error connecting to server" << endl;
        return 1;
    }

    cout << "Connected to server" << endl;

    // Elliptic curve parameters
    int a, b, r, x1P, y1P;

    cout << "Enter a value: ";
    cin >> a;
    cout << "Enter b value: ";
    cin >> b;
    cout << "Enter r value: ";
    cin >> r;
    cout << "Enter x1 value of P: ";
    cin >> x1P;
    cout << "Enter y1 value of P: ";
    cin >> y1P;

    // Send elliptic curve parameters to server
    send(clientSocket, &a, sizeof(int), 0);
    send(clientSocket, &b, sizeof(int), 0);
    send(clientSocket, &r, sizeof(int), 0);
    send(clientSocket, &x1P, sizeof(int), 0);
    send(clientSocket, &y1P, sizeof(int), 0);

    cout << "Sent curve parameters to server" << endl;

    // Receive points on elliptic curve from server
    map<string, vector<int>> valueP;
    for (int i = 2; i < 30; i++) {
        vector<int> point(2);
        recv(clientSocket, point.data(), point.size() * sizeof(int), 0);
        valueP[to_string(i) + "P"] = point;
    }

    cout << "Received points on the elliptic curve from server" << endl;

    // Print received points
    cout << "Points on the Elliptic Curve:" << endl;
    for (const auto &pair : valueP) {
        cout << pair.first << " = [" << pair.second[0] << ", " << pair.second[1] << "]" << endl;
    }

    // Close socket
    close(clientSocket);

    return 0;
}
