#include <iostream>

using namespace std;

int main(void) {
    int number;
    std::cin >> number;

    int answer = 0;

    for (int i = 0; i < number; i++)
    {
        answer += number % 100;
        number /= 100;
    }

    std::cout << answer<< std::endl;
    return 0;
}