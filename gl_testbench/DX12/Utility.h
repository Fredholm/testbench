#ifndef UTILITY_H
#define UTILITY_H

#include <Windows.h>
#include <exception>
#include <sstream>
#include <intrin.h>

class com_exception : public std::exception
{
public:
    com_exception(HRESULT hr) : result(hr) {}

    virtual const char* what() const override
    {
        static char s_str[64] = { 0 };
        sprintf_s(s_str, "Failure with HRESULT of %08X",
            static_cast<unsigned int>(result));
        return s_str;
    }

private:
    HRESULT result;
};

constexpr const char * translateHR(HRESULT hr)
{
    if (hr == 0x00000000) return "S_OK: Operation successful";
    if (hr == 0x80004004) return "E_ABORT: Operation aborted";
    if (hr == 0x80070005) return "E_ACCESSDENIED: General access denied error";
    if (hr == 0x80004005) return "E_FAIL: Unspecified failure";
    if (hr == 0x80070006) return "E_HANDLE: Handle that is not valid";
    if (hr == 0x80070057) return "E_INVALIDARG: One or more arguments are not valid";
    if (hr == 0x80004002) return "E_NOINTERFACE: No such interface supported";
    if (hr == 0x80004001) return "E_NOTIMPL: Not implemented";
    if (hr == 0x8007000E) return "E_OUTOFMEMORY: Failed to allocate necessary memory";
    if (hr == 0x80004003) return "E_POINTER: Pointer that is not valid";
    if (hr == 0x8000FFFF) return "E_UNEXPECTED: Unexpected failure";
    if (hr == 0x887A0001) return "E_INVALID_CALL: The application provided invalid parameter data.";
    return "UNKNOWN HRESULT";
}


inline void ThrowWithMessage(HRESULT hr, const char * file, size_t line)
{
    if (FAILED(hr))
    {
        std::stringstream err;
        err << file << "(" << line << ")" << std::endl;
        err << translateHR(hr) << std::endl;
        printf("%s\n", err.str().c_str());
        OutputDebugStringA(err.str().c_str());
        __debugbreak();
        throw com_exception(hr);
    }
}

#define ThrowIfFailed(hr) ThrowWithMessage(hr, __FILE__, __LINE__);

#endif // !UTILITY_H
