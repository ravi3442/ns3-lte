#include <iostream>
#include <string>

using namespace std;

int main(){

	string s1="Hello";
	string s2="xxxxx";
	cout<<s2<<endl;
	s2.replace(3,1, 1,'a');

	cout<<s2;

}
