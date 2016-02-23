#include "tpl_algorithm.h"
#include <iostream>

using namespace std;

int main(){
	cout << "Hello, World!" << endl;
	tpl::TplAlgorithm ta;
	const string path = "/Users/user/marcus/study/4-2/毕设/TPL/benchmark/ispd2005/bigblue1";
	ta.load_circuit(path);
	return 0;
}