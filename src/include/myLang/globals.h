#ifndef __GLOBALS_H__
#define __GLOBALS_H__
class MyLangGlobals{
private:
    MyLangGlobals(){}
public:
    bool hadError = false;
    bool hadRuntimeError = false; 
    MyLangGlobals(MyLangGlobals&) = delete;
    static MyLangGlobals& getInstance(){
        static MyLangGlobals singleton_instance;
        return singleton_instance;
    }
};

#endif