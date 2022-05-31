#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class IMyClass {
public:
    virtual ~IMyClass() {}
    virtual string ToString() = 0;
};

class MyClass : public IMyClass {       // The class
public:             // Access specifier
    virtual string ToString() {
        stringstream ss;
        ss << "To string! ";
        return ss.str();
    }
};


class MyClass2 : public IMyClass {       // The class
public:             // Access specifier
    virtual string ToString() {
        stringstream ss;
        ss << "YOOOOO  ";
        return ss.str();
    }
};
//
//int main()
//{
//    IMyClass* myObj;
//    MyClass c;
//    MyClass2 d;
//    myObj = &c;
//    DebugHelper::WriteLine(myObj->ToString();
//}