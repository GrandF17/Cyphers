#include <cstdint>
#include <iostream>

#include "./libs/streebog.h"

using namespace std;

int main() {
    cout << streebog("The quick brown fox jumps over the lazy dog") << endl;

    return 0;
}
