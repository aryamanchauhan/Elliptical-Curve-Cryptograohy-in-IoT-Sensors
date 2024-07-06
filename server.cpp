#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

// Function to perform modular arithmetic 
int Mod(int A, int B) {
    int ans = A % B;
    if (ans < 0) {
        ans = B + ans;
    }
    return ans;
}

// Function to calculate the modular inverse
int modInverse(int A, int M) {
    for (int X = 1; X < M; X++) {
        if (((A % M) * (X % M)) % M == 1) {
            return X;
        }
    }
    return 1;
}

// Function to calculate the next point using point addition on the elliptic curve
vector<int> NextPValueAddition(int a, int b, int r, int x1P, int y1P, int x2Q, int y2Q) {
    int s1 = (y2Q - y1P);
    int s2 = (x2Q - x1P);

    if (s1 < 0 && s2 < 0) {
        s1 = s1 * (-1);
        s2 = s2 * (-1);
    } else if (s2 < 0 && s1 >= 0) {
        s1 = s1 * (-1);
        s2 = s2 * (-1);
    }

    s1 = Mod(s1, r);
    s2 = modInverse(s2, r);

    int s = (s1 * s2) % r;

    int x3 = Mod(((s * s) - x1P - x2Q), r);
    int y3 = Mod(((s * (x1P - x3)) - y1P), r);

    vector<int> nextP = {x3, y3};
    return nextP;
}

// Function to calculate the next point using point doubling on the elliptic curve
vector<int> NextPValueDoubling(int a, int b, int r, int x1P, int y1P) {
    int s1 = Mod(((3 * (x1P * x1P)) + a), r);
    int s2 = modInverse((2 * y1P), r);
    int s = (s1 * s2) % r;

    int x2Q = x1P;
    vector<int> nextP = {x1P, y1P};
    return nextP;
}

// Function to calculate all points on the elliptic curve
void getAllP(int a, int b, int r, int x1P, int y1P, map<string, vector<int>>& valueP) {
    int x2Q = x1P;
    int y2Q = y1P;

    vector<int> k = {x1P, y1P};

    valueP["P"] = k;

    for (int i = 2; i < 30; i++) {
        vector<int> nextP;
        if (i == 2) {
            nextP = NextPValueDoubling(a, b, r, x1P, y1P);
        } else {
            nextP = NextPValueAddition(a, b, r, x1P, y1P, x2Q, y2Q);
        }

        x1P = nextP[0];
        y1P = nextP[1];

        valueP[to_string(i) + "P"] = nextP;

        if (nextP[0] == x2Q) {
            if (((nextP[1] % r) + r) % r == (y2Q * -1)) {
                break;
            }
        }
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // Initialize server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080); // Port 8080

    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error binding socket" << endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 1) < 0) {
        cerr << "Error listening for connections" << endl;
        return 1;
    }

    cout << "Server listening on port 8080..." << endl;

    // Accept incoming connection
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
    if (clientSocket < 0) {
        cerr << "Error accepting connection" << endl;
        return 1;
    }

    cout << "Connection established with client" << endl;

    // Elliptic curve parameters
    int a, b, r, x1P, y1P;

    cout << "Let the Elliptic Curve be: y^2 = x^3 + ax + b mod r" << endl;

    // Receive elliptic curve parameters from client
    recv(clientSocket, &a, sizeof(int), 0);
    recv(clientSocket, &b, sizeof(int), 0);
    recv(clientSocket, &r, sizeof(int), 0);
    recv(clientSocket, &x1P, sizeof(int), 0);
    recv(clientSocket, &y1P, sizeof(int), 0);

    cout << "Received curve parameters from client" << endl;

    map<string, vector<int>> valueP;
    getAllP(a, b, r, x1P, y1P, valueP);

    // Send computed points on elliptic curve to client
    for (const auto &pair : valueP) {
        send(clientSocket, pair.second.data(), pair.second.size() * sizeof(int), 0);
    }

    cout << "Sent points on the elliptic curve to client" << endl;

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
