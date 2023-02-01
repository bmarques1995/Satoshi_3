#ifndef HELLO_HPP
#define HELLO_HPP

#ifdef ST_PLATFORM_WINDOWS
    #ifdef ST_BUILD_DLL
        #define SATOSHI_API __declspec(dllexport)
    #else
        #define SATOSHI_API __declspec(dllimport)
    #endif
#endif

namespace Satoshi
{
    class SATOSHI_API Hello
    {
    public:
        static void Print();
    private:

    };
}


#endif //HELLO_HPP
