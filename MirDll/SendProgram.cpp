#include "SendProgram.h"
#include "Encode.h"
#include "MyHook.h"
#include "json/value.h"

HMODULE ws2_32 = GetModuleHandleA("ws2_32.dll");
typedef int(__stdcall* __recv)(SOCKET s, char* buf, int len, int flags);
typedef int(__stdcall* __send)(SOCKET s, const char* buf, int len, int flags);

__recv Myrecv = (__recv)GetProcAddress(ws2_32, "recv");
__send Mysend = (__send)GetProcAddress(ws2_32, "send");

