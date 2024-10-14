#include <iostream>

using namespace std;

int main(void) {
    std::string code;
    std::cin >> code;

    std::string last_four_words = code.substr(code.size() - 4, 4);
    if (last_four_words == "_eye")
    {
        cout << "Ophthalmologyc";
    }
    else if (last_four_words == "head")
    {
        cout << "Neurosurgery";
    }
    else if (last_four_words == "infl")
    {
        cout << "Orthopedics";
    }    
    else if(last_four_words == "skin")
    {
        cout << "Dermatology";
    }
    else
    {
        cout << "direct recommendation";
    }
    return 0;
}