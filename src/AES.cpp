/****************************************************************************
** ┌─┐┬ ┬┬─┐┌─┐┬─┐┌─┐  ┌─┐┬─┐┌─┐┌┬┐┌─┐┬ ┬┌─┐┬─┐┬┌─
** ├─┤│ │├┬┘│ │├┬┘├─┤  ├┤ ├┬┘├─┤│││├┤ ││││ │├┬┘├┴┐
** ┴ ┴└─┘┴└─└─┘┴└─┴ ┴  └  ┴└─┴ ┴┴ ┴└─┘└┴┘└─┘┴└─┴ ┴
** A Powerful General Purpose Framework
** More information in: https://aurora-fw.github.io/
**
** Copyright (C) 2017 Aurora Framework, All rights reserved.
**
** This file is part of the Aurora Framework. This framework is free
** software; you can redistribute it and/or modify it under the terms of
** the GNU Lesser General Public License version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE included in
** the packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
****************************************************************************/

#include <AuroraFW/Crypto/AES.h>
#include <AuroraFW/CLI/Log.h>
#include <AuroraFW/STDL/LibC/String.h>
#include <AuroraFW/CoreLib/Target/Compiler.h>
#include <AuroraFW/STDL/STL/IOStream.h>

#ifdef AFW_TARGET_CXX
#include <cstdlib>
#elif defined(AFW_TARGET_CC)
#include <stdlib.h>
#endif

namespace AuroraFW
{
	int AES::_nr, AES::_nk;

	/* i - it is the array that holds the CipherText to be decrypted.
	* o - it is the array that holds the output of the for decryption.
	* s - the array that holds the intermediate results during decryption.
	*/
	unsigned char AES::_in[16], AES::_out[16], AES::_stt[4][4];

	// The array that stores the round keys.
	unsigned char AES::_rk[240];
	// The Key input to the AES Program
	unsigned char AES::_key[32];

	/* @bief	Function to get SubBox Value for AES encryptation
	** @param	number for value of subbox array
	** @return	value of suubbox array
	*/
	int AES::_getSBV(const int& n)
	{
		const int sb[256] =
		{
			//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
			0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
			0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
			0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
			0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
			0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
			0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
			0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
			0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
			0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
			0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
			0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
			0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
			0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
			0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
			0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
			0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16  //F
		};
		return sb[n];
	}
	/* @bief		Function to get inverse of SubBox Value
	** @param 	n	number for inverse value of subbox array
	** @return		value of inverse subbox array
	*/
	int AES::_getISBV(const int& n)
	{
		const int isb[256] =
		{
			0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb
			, 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb
			, 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e
			, 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25
			, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92
			, 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84
			, 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06
			, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b
			, 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73
			, 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e
			, 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b
			, 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4
			, 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f
			, 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef
			, 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61
			, 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
		};

		return isb[n];
	}

	void AES::_ke()
	{
		/*
		* The round constant word array, _rc[i], contains the values given by
		* x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
		* Note that i starts at 1, not 0).
		*/
		const int _rc[256] =
		{
			0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
			0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
			0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
			0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
			0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
			0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
			0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
			0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
			0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
			0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
			0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
			0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
			0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
			0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
			0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
			0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d
		};

		int i;
		unsigned char temp[4], k;

		// The first round key is the key itself.
		for(i=0;i<_nk;i++)
		{
			_rk[i*4]=_key[i*4];
			_rk[i*4+1]=_key[i*4+1];
			_rk[i*4+2]=_key[i*4+2];
			_rk[i*4+3]=_key[i*4+3];
		}

		// All other round keys are found from the previous round keys.
		while (i < (AFW_AES_NUM * (_nr+1)))
		{
			for(int j=0;j<4;j++)
			{
				temp[j]= _rk[(i-1) * 4 + j];
			}
			if (i % _nk == 0)
			{
				// This function rotates the 4 bytes in a word to the left once.
				// [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

				// Function RotWord()
				{
					k = temp[0];
					temp[0] = temp[1];
					temp[1] = temp[2];
					temp[2] = temp[3];
					temp[3] = k;
				}

				// SubWord() is a function that takes a four-byte input word and
				// applies the S-box to each of the four bytes to produce an output word.

				// Function Subword()
				{
					temp[0]=_getSBV(temp[0]);
					temp[1]=_getSBV(temp[1]);
					temp[2]=_getSBV(temp[2]);
					temp[3]=_getSBV(temp[3]);
				}

				temp[0] =  temp[0] ^ _rc[i/_nk];
			}
			else if (_nk > 6 && i % _nk == 4)
			{
				// Function Subword()
				{
					temp[0]=_getSBV(temp[0]);
					temp[1]=_getSBV(temp[1]);
					temp[2]=_getSBV(temp[2]);
					temp[3]=_getSBV(temp[3]);
				}
			}
			_rk[i*4+0] = _rk[(i-_nk)*4+0] ^ temp[0];
			_rk[i*4+1] = _rk[(i-_nk)*4+1] ^ temp[1];
			_rk[i*4+2] = _rk[(i-_nk)*4+2] ^ temp[2];
			_rk[i*4+3] = _rk[(i-_nk)*4+3] ^ temp[3];
			i++;
		}
	}

	void AES::_ark(const int& round)
	{
		int i,j;
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				_stt[j][i] ^= _rk[round * AFW_AES_NUM * 4 + i * AFW_AES_NUM + j];
			}
		}
	}

	void AES::_sb()
	{
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<4;j++)
			{
				_stt[i][j] = _getSBV(_stt[i][j]);
			}
		}
	}

	void AES::_isb()
	{
		int i,j;
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				_stt[i][j] = _getISBV(_stt[i][j]);
			}
		}
	}

	void AES::_sr()
	{
		unsigned char temp;

		// Rotate first row 1 columns to left
		temp=_stt[1][0];
		_stt[1][0]=_stt[1][1];
		_stt[1][1]=_stt[1][2];
		_stt[1][2]=_stt[1][3];
		_stt[1][3]=temp;

		// Rotate second row 2 columns to left
		temp=_stt[2][0];
		_stt[2][0]=_stt[2][2];
		_stt[2][2]=temp;

		temp=_stt[2][1];
		_stt[2][1]=_stt[2][3];
		_stt[2][3]=temp;

		// Rotate third row 3 columns to left
		temp=_stt[3][0];
		_stt[3][0]=_stt[3][3];
		_stt[3][3]=_stt[3][2];
		_stt[3][2]=_stt[3][1];
		_stt[3][1]=temp;
	}

	void AES::_isr()
	{
		unsigned char temp;

		// Rotate first row 1 columns to right
		temp=_stt[1][3];
		_stt[1][3]=_stt[1][2];
		_stt[1][2]=_stt[1][1];
		_stt[1][1]=_stt[1][0];
		_stt[1][0]=temp;

		// Rotate second row 2 columns to right
		temp=_stt[2][0];
		_stt[2][0]=_stt[2][2];
		_stt[2][2]=temp;

		temp=_stt[2][1];
		_stt[2][1]=_stt[2][3];
		_stt[2][3]=temp;

		// Rotate third row 3 columns to right
		temp=_stt[3][0];
		_stt[3][0]=_stt[3][1];
		_stt[3][1]=_stt[3][2];
		_stt[3][2]=_stt[3][3];
		_stt[3][3]=temp;
	}

	void AES::_mc()
	{
		int i;
		unsigned char Tmp,Tm,t;
		for(i=0;i<4;i++)
		{
			t=_stt[0][i];
			Tmp = _stt[0][i] ^ _stt[1][i] ^ _stt[2][i] ^ _stt[3][i] ;
			Tm = _stt[0][i] ^ _stt[1][i] ; Tm = AFW_AES_TIME(Tm); _stt[0][i] ^= Tm ^ Tmp ;
			Tm = _stt[1][i] ^ _stt[2][i] ; Tm = AFW_AES_TIME(Tm); _stt[1][i] ^= Tm ^ Tmp ;
			Tm = _stt[2][i] ^ _stt[3][i] ; Tm = AFW_AES_TIME(Tm); _stt[2][i] ^= Tm ^ Tmp ;
			Tm = _stt[3][i] ^ t ; Tm = AFW_AES_TIME(Tm); _stt[3][i] ^= Tm ^ Tmp ;
		}
	}

	void AES::_imc()
	{
		int i;
		unsigned char a,b,c,d;
		for(i=0;i<4;i++)
		{
			a = _stt[0][i];
			b = _stt[1][i];
			c = _stt[2][i];
			d = _stt[3][i];


			_stt[0][i] = AFW_AES_MULTI(a, 0x0e) ^ AFW_AES_MULTI(b, 0x0b) ^ AFW_AES_MULTI(c, 0x0d) ^ AFW_AES_MULTI(d, 0x09);
			_stt[1][i] = AFW_AES_MULTI(a, 0x09) ^ AFW_AES_MULTI(b, 0x0e) ^ AFW_AES_MULTI(c, 0x0b) ^ AFW_AES_MULTI(d, 0x0d);
			_stt[2][i] = AFW_AES_MULTI(a, 0x0d) ^ AFW_AES_MULTI(b, 0x09) ^ AFW_AES_MULTI(c, 0x0e) ^ AFW_AES_MULTI(d, 0x0b);
			_stt[3][i] = AFW_AES_MULTI(a, 0x0b) ^ AFW_AES_MULTI(b, 0x0d) ^ AFW_AES_MULTI(c, 0x09) ^ AFW_AES_MULTI(d, 0x0e);
		}
	}

	void AES::_c()
	{
		int i,j,round=0;

		//Copy the input PlainText to s array.
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				_stt[j][i] = _in[i*4 + j];
			}
		}

		// Add the First round key to the s before starting the rounds.
		_ark(0);

		// There will be Nr rounds.
		// The first Nr-1 rounds are identical.
		// These Nr-1 rounds are executed in the loop below.
		for(round=1;round<_nr;round++)
		{
			_sb();
			_sr();
			_mc();
			_ark(round);
		}

		// The last round is given below.
		// The MixColumns function is not here in the last round.
		_sb();
		_sr();
		_ark(_nr);

		// The encryption process is over.
		// Copy the s array to output array.
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				_out[i*4+j]=_stt[j][i];
			}
		}
	}

	void AES::_ic()
	{
		int i,j,r=0;

		//Copy the input CipherText to s array.
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				_stt[j][i] = _in[i*4 + j];
			}
		}

		// Add the First round key to the s before starting the rounds.
		_ark(_nr);

		// There will be Nr rounds.
		// The first Nr-1 rounds are identical.
		// These Nr-1 rounds are executed in the loop below.
		for(r=_nr-1;r>0;r--)
		{
			_isr();
			_isb();
			_ark(r);
			_imc();
		}

		// The last round is given below.
		// The MixColumns function is not here in the last round.
		_isr();
		_isb();
		_ark(0);

		// The decryption process is over.
		// Copy the s array to output array.
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				_out[i*4+j]=_stt[j][i];
			}
		}
	}

	unsigned char* AES::encrypt(const unsigned char k[32],
								const int& ks,
								unsigned char i[16])
	{
		if (ks==128 || ks==192 || ks==256)
		{
			_nk = ks / 32;
			_nr = _nk + 6;

			/* for(int i=0;i<_nk*4;i++)
			{
				k[i]=k[i];
				i[i]=i[i];
			} */
			memcpy(_key, k, _nk * 4);
			memcpy(_in, i, _nk * 4);

			// The KeyExpansion routine must be called before encryption.
			_ke();

			// The next function call encrypts the PlainText with the Key using AES algorithm.
			_c();

			return _out;
		}
		else
		{
		   CLI::Log(CLI::Error, "aes encrypt: invalid keysize");
		   return NULL;
		}
	}

	/* @brief	AES decrypt function
	** @param k	AES Key
	** @param ks	AES Key size
	** @param i	input block
	** @return uchar	output block
	*/
	unsigned char* AES::decrypt(const unsigned char k[32],
								const int& ks,
								unsigned char i[16])
	{
		if (ks==128 || ks==192 || ks==256)
		{
			_nk = ks / 32;
			_nr = _nk + 6;

			memcpy(_key, k, _nk * 4);
			memcpy(_in, i, _nk * 4);

			//The Key-Expansion routine must be called before the decryption routine.
			_ke();

			// The next function call decrypts the CipherText with the Key using AES algorithm.
			_ic();

			return _out;
		}
		else
		{
			CLI::Log(CLI::Error, "aes decrypt: invalid keysize");
			return NULL;
		}
	}
}
