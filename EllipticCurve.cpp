#include <iostream>
#include <vector>
#include <map>
#include <cmath>

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

    cout << "The equation is: y^2 = x^3 + (" << a << " * x ) + (" << b << " ) mod " << r << endl;
    cout << "Points on the Elliptic Curve:" << endl;

    valueP["P"] = k;
    cout << "P = [" << k[0] << ", " << k[1] << "]" << endl;

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
        cout << i << "P = [" << nextP[0] << ", " << nextP[1] << "]" << endl;

        if (nextP[0] == x2Q) {
            if (((nextP[1] % r) + r) % r == (y2Q * -1)) {
                cout << i + 1 << "P = Point of Infinity" << endl;
                break;
            }
        }
    }
}

int main() {
    int a, b, r, x1P, y1P;

    cout << "Let the Elliptic Curve be: y^2 = x^3 + ax + b mod r" << endl;

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
    cout << "The value of P is [" << x1P << ", " << y1P << "]" << endl;

    map<string, vector<int>> valueP;
    getAllP(a, b, r, x1P, y1P, valueP);

    cout << "Enter a value of first person: ";
    int a1;
    cin >> a1;
    vector<int> A = valueP[to_string(a1) + "P"];
    cout << "Therefore, first person sends: [" << A[0] << ", " << A[1] << "]" << endl << endl;

    cout << "Enter a value of second person: ";
    int b1;
    cin >> b1;
    vector<int> B = valueP[to_string(b1) + "P"];
    cout << "Therefore, second person sends: [" << B[0] << ", " << B[1] << "]" << endl << endl;

    map<string, vector<int>> valueFirstPersonP;
    map<string, vector<int>> valueSecondPersonP;

    getAllP(a, b, r, B[0], B[1], valueFirstPersonP);
    vector<int> aKey = valueFirstPersonP[to_string(a1) + "P"];
    cout << "Therefore, from " << a1 << " " << "[" << B[0] << ", " << B[1] << "]" << " we get " << "[" << aKey[0] << ", " << aKey[1] << "]" << endl << endl;

    getAllP(a, b, r, A[0], A[1], valueSecondPersonP);
    vector<int> bKey = valueSecondPersonP[to_string(b1) + "P"];
    cout << "Therefore, from " << b1 << " " << "[" << A[0] << ", " << A[1] << "]" << " we get " << "[" << bKey[0] << ", " << bKey[1] << "]" << endl << endl;

    if (aKey[0] == bKey[0]) {
        cout << "Shared Key = " << aKey[0] << endl;
    }

    return 0;
}