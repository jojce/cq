#include "Decode.h"




unsigned __int8* __fastcall Decode6BitBuf(unsigned __int8* result, char* a2, unsigned int a3, unsigned int* a4)
{
	signed int v4; // r9
	int v5; // r6
	unsigned int v6; // r12
	int v7; // lr
	unsigned int v8; // r5
	unsigned __int8 v9; // r5

	v4 = 2;
	v5 = 0;
	v6 = 0;
	LOBYTE(v7) = 0;
	while (a3)
	{
		v8 = *(unsigned __int8*)result;
		if (v8 < 0x3C)
			break;
		v9 = v8 - 60;
		if (v5 < 2)
			goto LABEL_5;
		a2[v6++] = DecodeBitMasks[(unsigned __int8)(((v9 & 0x3Fu) >> (6 - v4)) | v7) ^ 0x9E] ^ 0x34;
		if (v4 <= 5)
		{
			v4 += 2;
			v5 = 0;
		LABEL_5:
			v7 = byte_EF33CB[v4 - 2] & (v9 << v4);
			v5 = v5 - v4 + 8;
			goto LABEL_6;
		}
		v4 = 2;
		v5 = 0;
	LABEL_6:
		++result;
		--a3;
	}
	*a4 = v6;
	return result;
}