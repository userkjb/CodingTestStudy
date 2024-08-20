#include <iostream>
#include <string>
#include <vector>

using namespace std;

string ReStr(int n);
string solution(int n);

int main()
{
	for (size_t i = 1; i <= 3; i++)
	{
		cout << solution(i) << endl;
	}

	return 0;
}

string solution(int n)
{
	string answer = "";
	int count = 0;
	
	int x = n / 3;

	if (count < 3)
	{
		
	}
	else
	{
		//for (size_t i = 0; i < length; i++)
		//{
		//	answer += ReStr(n);
		//	count++;
		//}
	}


	// answer += ReStr(n); 이게 1번 실행 될지 2번 실행 될지

	return answer;
}

string ReStr(int n)
{
	vector<string> UsValue;
	UsValue.push_back("4");
	UsValue.push_back("1");
	UsValue.push_back("2");

	string Str = "";

	int y = n % 3;

	Str += UsValue[y];

	return Str;
}
