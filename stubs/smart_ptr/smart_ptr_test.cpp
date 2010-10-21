#include <boost/shared_ptr.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::stringstream;
using std::vector;


using boost::shared_ptr;

struct Test_Type
{
	string s_;
	int v_;
	Test_Type():
		s_(string("Test_Type")), v_(31102010)
	{}
	Test_Type(string p_s, int p_v):
		s_(p_s), v_(p_v)
	{}

};

vector<shared_ptr<Test_Type> > GenerateData()
{
	vector<shared_ptr<Test_Type> > vec;
	for(int i=0; i<10; ++i) {
		stringstream s;
		s << "Test_Type_" << i;
		shared_ptr<Test_Type> test_type_ptr(new Test_Type(s.str(), i));
		vec.push_back(test_type_ptr);
	}
	return vec;
}

int main()
{
	vector<shared_ptr<Test_Type> > vec = GenerateData();
	for(int i=0; i<vec.size(); ++i) {
		cout << vec[i]->s_ << "," << vec[i]->v_
			<< endl;
	}

}
