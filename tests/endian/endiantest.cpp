/*-----------------------------------------------------------------------------
Copyright (C) 2011 SIL International
Responsibility: Tim Eves

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should also have received a copy of the GNU Lesser General Public
    License along with this library in the file named "LICENSE".
    If not, write to the Free Software Foundation, 51 Franklin Street,
    Suite 500, Boston, MA 02110-1335, USA or visit their web page on the
    internet at http://www.fsf.org/licenses/lgpl.html.

Alternatively, the contents of this file may be used under the terms of the
Mozilla Public License (http://mozilla.org/MPL) or the GNU General Public
License, as published by the Free Software Foundation, either version 2
of the License or (at your option) any later version.

Description:
The test harness for the Endian.h template library.  This validates the
Endian.h classes are working correctly and with all fundamental C/C++ integer
types. This may also be used for benchmarking by suppling the base two
magnitude for the number of rounds to run.
Adding -DHAVE_64_LONG to your compilers command line will test 64bit wide
integers in addition to 32, 16 and 8 bit.
-----------------------------------------------------------------------------*/
#include <cstdlib>
#include <string>
#include "inc/Endian.h"

typedef unsigned char 		uint8;
typedef signed char		     int8;
typedef short unsigned int 	uint16;
typedef short signed int 	 int16;
typedef unsigned int 		uint32;
typedef signed int 		     int32;
#if defined(HAVE_64_LONG)
typedef long unsigned int 	uint64;
typedef long signed int 	 int64;
#else
typedef void                uint64;
typedef void                 int64;
#endif

template <typename T8, typename T4, typename T2, typename T1> 
bool test_swaps()
{
	bool t = true;
	
	t &= be::swap<T1>(T1(0xFEU)) == T1(0xFEU);
	t &= le::swap<T1>(T1(0xFEU)) == T1(0xFEU);

	t &= be::swap<T2>(be::swap<T2>(T2(0xFFEEU))) == T2(0xFFEEU);
	t &= le::swap<T2>(le::swap<T2>(T2(0xFFEEU))) == T2(0xFFEEU);
	t &= be::swap<T2>(T2(0xFFEEU)) == le::swap<T2>(T2(0xEEFFU));
#if defined(HAVE_64_LONG)
	t &= be::swap<T4>(be::swap<T4>(T4(0xDEADBEEFU))) == T4(0xDEADBEEFU);
	t &= le::swap<T4>(le::swap<T4>(T4(0xDEADBEEFU))) == T4(0xDEADBEEFU);
	t &= be::swap<T4>(T4(0xDEADBEEFU)) == le::swap<T4>(T4(0xEFBEADDEU));
#endif
    return t;
}


template <typename T8, typename T4, typename T2, typename T1> 
int test_reads(const size_t rounds)
{
	static const unsigned char data[] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,
	                                     0xEF,0xCD,0xAB,0x89,0x67,0x45,0x23,0x01};
	bool t = true;
	
#if defined(HAVE_64_LONG)
	for (size_t r = rounds; r; --r)
	{
	    const unsigned char *p = data;
		if (be::read<T8>(p) != T8(0x0123456789ABCDEF))  return  8;
		if (le::read<T8>(p) != T8(0x0123456789ABCDEF))  return 80;
	}
#endif
	for (size_t r = rounds; r; --r)
	{
	    const unsigned char *p = data;
		t  = be::read<T4>(p) == T4(0x01234567);
		t &= be::read<T4>(p) == T4(0x89ABCDEF);
		if (!t) return  4;
		t  = le::read<T4>(p) == T4(0x89ABCDEF);
		t &= le::read<T4>(p) == T4(0x01234567);
		if (!t) return 40;		
	}
	for (size_t r = rounds; r; --r)
    {
	    const unsigned char *p = data;
		t  = be::read<T2>(p) == T2(0x0123);
		t &= be::read<T2>(p) == T2(0x4567); 
		t &= be::read<T2>(p) == T2(0x89AB); 
		t &= be::read<T2>(p) == T2(0xCDEF); 
		if (!t) return  2;
		t  = le::read<T2>(p) == T2(0xCDEF); 
		t &= le::read<T2>(p) == T2(0x89AB); 
		t &= le::read<T2>(p) == T2(0x4567); 
		t &= le::read<T2>(p) == T2(0x0123);
		if (!t) return 20;
	}
	for (size_t r = rounds; r; --r)
	{
	    const unsigned char *p = data;
		t  = be::read<T1>(p) == T1(0x01); 
		t &= be::read<T1>(p) == T1(0x23);
		t &= be::read<T1>(p) == T1(0x45);
		t &= be::read<T1>(p) == T1(0x67); 
		t &= be::read<T1>(p) == T1(0x89);
		t &= be::read<T1>(p) == T1(0xAB);
		t &= be::read<T1>(p) == T1(0xCD);
		t &= be::read<T1>(p) == T1(0xEF);
		if (!t) return  1;
		t  = le::read<T1>(p) == T1(0xEF);
		t &= le::read<T1>(p) == T1(0xCD);
		t &= le::read<T1>(p) == T1(0xAB);
		t &= le::read<T1>(p) == T1(0x89);
		t &= le::read<T1>(p) == T1(0x67); 
		t &= le::read<T1>(p) == T1(0x45);
		t &= le::read<T1>(p) == T1(0x23);
		t &= le::read<T1>(p) == T1(0x01); 
		if (!t) return 10;
	}
	return 0;
}

int main(int argc , char *argv[])
{
	const size_t rounds = 1UL << atoi(argv[1]);
	int r = 0;

    if   (!test_swaps<uint64, uint32, uint16, uint8>()
       || !test_swaps<int64, int32, int16, int8>())
        return 5;
    
	if (r == 0) r = test_reads<uint64, uint32, uint16, uint8>(rounds);
	if (r == 0) r = test_reads<int64, int32, int16, int8>(rounds);
	return r;
}

