#ifndef __MYLANG_TYPES_H__
#define __MYLANG_TYPES_H__
#include <string>

enum MyLang_object_type{
    MYLANG_OBJECT, MYLANG_BOOLEAN, MYLANG_DOUBLE, MYLANG_STRING, MYLANG_CALLABLE
};
class MyLang_Object{
protected:
    enum MyLang_object_type type;
public:
    MyLang_Object(){
        this->type = MyLang_object_type::MYLANG_OBJECT;
    }
    virtual MyLang_object_type getType(){
        return this->type;
    }
    virtual void setType(MyLang_object_type t){
        this->type = t;
    }
};

class MyLang_Boolean: public MyLang_Object{
public:
    bool value;
    MyLang_Boolean(bool value){
        this->value = value;
        setType(MyLang_object_type::MYLANG_BOOLEAN);
    }
};

class MyLang_Double: public MyLang_Object{
public:
    double value;
    MyLang_Double(double value){
        this->value = value;
        setType(MyLang_object_type::MYLANG_DOUBLE);
    }
};

class MyLang_String: public MyLang_Object{
public:
    std::string value;
    MyLang_String(std::string value){
        this->value = value;
        setType(MyLang_object_type::MYLANG_STRING);
    }
};

#endif