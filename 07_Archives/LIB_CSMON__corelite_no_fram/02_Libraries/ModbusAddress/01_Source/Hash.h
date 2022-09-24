#pragma once

#ifndef _CRYPTO_H_
#define _CRYPTO_H_

int Hash(
	const char* pKey,
	const int keySize,
	char* pSrc,
	const int srcSize,
	char* pDst,
	const int dstSizeMax,
	int* const pDstSize
	);

#endif /* _CRYPTO_H_ */
