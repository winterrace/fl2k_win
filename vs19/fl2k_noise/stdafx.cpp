// stdafx.cpp : Quelldatei, die nur die Standard-Includes einbindet.
// fl2k_fm.pch ist der vorkompilierte Header.
// stdafx.obj enth�lt die vorkompilierten Typinformationen.

#include "stdafx.h"

// TODO: Auf zus�tzliche Header verweisen, die in STDAFX.H
// und nicht in dieser Datei erforderlich sind.


#ifdef __cplusplus
FILE iob[] = { *stdin, *stdout, *stderr };
extern "C" {
	FILE * __cdecl _iob(void) { return iob; }
}
#endif
