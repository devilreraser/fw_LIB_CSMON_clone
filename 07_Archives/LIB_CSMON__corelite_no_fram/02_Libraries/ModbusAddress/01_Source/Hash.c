#include "Hash.h"

#include <stdint.h>


typedef enum _LocalConsts
{
	Prime8Count = 29,
	NoPrime8Count = 31,
	PrimeIndArrLen_2 = 7,
	PrimeIndArrLen = 2 * PrimeIndArrLen_2,
	PolynomOrder = 5,
	SumCharCount = 4,
} LocalConsts;


static const unsigned char prime8[Prime8Count] =
{
	149, 179, 157, 139, 163, 173, 167, 151,
	233, 241, 229, 251,		 239,
	191, 227, 197, 181, 199, 223, 211, 193,
	103, 137, 109, 101, 113, 131, 127, 107,
};


static const unsigned char noPrime8[NoPrime8Count] =
{
	253, 221, 209, 187, 161, 143, 133, 119,
	115,  95,  91,  85,  77,  69,  65, 255,
	238, 230, 195, 190, 165, 154, 138, 130,
	105, 114, 102,  78,  66,  42,  30
};


static void InitPrimeInd(uint32_t src, unsigned char pDst[PrimeIndArrLen_2])
{
	unsigned char* const pDstEnd = pDst + PrimeIndArrLen_2;
	for (; pDstEnd > pDst; ++pDst)
	{
		*pDst = src % Prime8Count;
		src /= Prime8Count;
	}
}

unsigned char primeIndArr[PrimeIndArrLen];

int Hash(
	const char* pKey,
	const int keySize,
	char* pSrc,
	const int srcSize,
	char* const pDst,
	const int dstSizeMax,
	int* const pDstSize
	)
{
	if (0 == pKey)
	{
		return -1;
	}

	if (10 > keySize)
	{
		return -2;
	}

	if (0 == pSrc)
	{
		return -3;
	}

	if (1 > srcSize)
	{
		return -4;
	}

	if (0 == pDst)
	{
		return -5;
	}

	{
		const int dstSizeMin = (keySize < srcSize) ? srcSize : keySize;

		if (dstSizeMin > dstSizeMax)
		{
			return -6;
		}
	}

	if (dstSizeMax & 0x01)
	{
		return -6;
	}

	if (0 == pDstSize)
	{
		return -7;
	}


	unsigned char k = *pKey++;
	const unsigned char YY = (k & 0x0F) + (k >> 4) * 10;

	k = *pKey++;
	const unsigned char MM = ((k & 0x0F) + (k >> 4) * 10) - 1;

	k = *pKey++;
	const unsigned char DD = ((k & 0x0F) + (k >> 4) * 10) - 1;

	k = *pKey++;
	const unsigned char hh = (k & 0x0F) + (k >> 4) * 10;

	k = *pKey++;
	const unsigned char mm = (k & 0x0F) + (k >> 4) * 10;

	k = *pKey++;
	const unsigned char ss = (k & 0x0F) + (k >> 4) * 10;

	const uint16_t months = 12 * YY + MM;
	const uint16_t days = 31 * months + DD;
	const uint32_t hours = 24 * (uint32_t)days + hh;
	const uint32_t minutes = 60 * hours + mm;
	const uint32_t seconds = 60 * minutes + ss;

	InitPrimeInd(seconds, (unsigned char*)primeIndArr);

	k = *pKey++;
	uint32_t subSeconds = ((uint32_t)k << 24);

	k = *pKey++;
	subSeconds += ((uint32_t)k << 16);

	k = *pKey++;
	subSeconds += ((uint16_t)k << 8);

	k = *pKey++;
	subSeconds += k;

	uint32_t u32rest = subSeconds << 24;
	u32rest += subSeconds << 16;
	u32rest += subSeconds << 8;
	u32rest += subSeconds;
	u32rest -= seconds;

	InitPrimeInd(u32rest, (unsigned char*)(primeIndArr + PrimeIndArrLen_2));

	uint16_t sum = (0xFFFF & u32rest) ^ (u32rest >> 16);
	unsigned char ip;
	unsigned char in;
	uint16_t i = seconds;
	uint32_t ii = u32rest;
    
	ip = Prime8Count - 1;
	in = NoPrime8Count - 1;

	typedef enum _Pass
	{
		PassDone = 0,
		PassBackward,
		PassForward,
	} Pass;

	Pass pass;
	for (pass = PassForward; PassDone != pass; --pass)
	{
		if (PassBackward == pass)
		{
			const char* const pSrcEnd_2 = pSrc + (srcSize >> 1);
			char* psf = pSrc;
			char* psb = pSrc + srcSize - 1;
			for (; pSrcEnd_2 > psf; ++psf, --psb)
			{
				const char temp = *psf;
				*psf = *psb;
				*psb = temp;
			}
		}

		uint16_t ofs = 0;
		const char* const pDstEnd = pDst + dstSizeMax;
		char* pd;
        unsigned char p = prime8[ip];
        unsigned char n = noPrime8[in];
		for (pd = pDst; pDstEnd > pd; ++ofs)
		{
			//const char* const pSrcEnd = pSrc + srcSize;
			unsigned char charCntr;
			for (charCntr = SumCharCount; charCntr; --charCntr)
			{
			    unsigned char cntr;
				for (cntr = PolynomOrder; cntr; --cntr)
				{
					{
						p = prime8[ip];
						ip = ip ? (ip-1) : (Prime8Count - 1);
						ii *= i * p;
					}
					{
						n = noPrime8[in];
						in = in ? (in-1) : (NoPrime8Count - 1);
						ii += n;
					}
				}

				++i;

				{
					const unsigned char in = ii % NoPrime8Count;
					const unsigned char n = noPrime8[in];

					const unsigned char ii_mod = ii % PrimeIndArrLen;
					const unsigned char primeInd = primeIndArr[ii_mod];
					const unsigned char p = prime8[primeInd];

					const uint16_t charInd = (ofs + charCntr) % srcSize;
					const unsigned char c = pSrc[charInd];

					sum += (c * (charInd + 1) + n) * p;
					sum = ((sum & 0xFF) << 8) | (sum >> 8);
				}
			}

			if (PassForward == pass)
			{
				*pd++ = sum >> 8;
				*pd++ = 0xFF & sum;
			}
			else
			{
				*pd++ += sum >> 8;
				*pd++ += 0xFF & sum;
			}
		}
	}

	*pDstSize = dstSizeMax;
	return 0;
}
