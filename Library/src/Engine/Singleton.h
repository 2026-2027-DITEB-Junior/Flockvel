#pragma once

/*  This maccro is used to define any class as a Singleton.
*   It should be added INSIDE the class declaration.
*   Notice that the copy constructor and assignment operators
*   are all removed to prevent creating copies.
*   It makes the constructor and destructor private. This means
*   that you won't be able to instantiate this class outside of the
*   `getInstance` function. That is in fact the whole point of the
*   singleton pattern.
*   private constructor and destructor also means that you won't be
*   able to derive from a singleton class. That is by design.
*/
#define CLC_SINGLETON(ClassName)              \
public:                                       \
    ClassName(const ClassName&) = delete;     \
    ClassName& operator=(const ClassName&) = delete; \
    static ClassName& GetInstance() {         \
        static ClassName instance;            \
        return instance;                      \
    }                                         \
private:                                      \
    ClassName() {}                            \
    ~ClassName() {}