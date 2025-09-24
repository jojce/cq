#include "Encode.h"

unsigned __int8* __fastcall Encode6BitBuf(unsigned __int8* result, char* a2, unsigned int a3, unsigned int* a4)
{
	unsigned int v4; // lr
	int v5; // r9
	int v6; // r8
	unsigned int v7; // r6
	int v8; // r4
	char v9; // r5



	v4 = 0;
	v5 = 0;
	v6 = 0;
	while (a3)
	{
		v7 = EncodeBitMasks[*(unsigned __int8*)result] ^ 9;
		a2[v4] = (((v7 >> (v5 + 2)) | v6) & 0x3F) + 0x3C;
		v6 = (unsigned __int8)(v7 << (6 - v5)) >> 2;
		v8 = v4 + 1;
		if (v5 + 2 >= 6)
		{
			v4 += 2;
			v9 = v6 + 60;
			v5 = 0;
			v6 = 0;
			a2[v8] = v9;
		}
		else
		{
			++v4;
			v5 += 2;
		}
		++result;
		--a3;
	}
	if (v5 > 0)
	{
		*result = v6 + 60;
		a2[v4++] = v6 + 60;
	}
	*a4 = v4;
	return result;
}
