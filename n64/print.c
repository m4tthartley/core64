//
//  Created by Matt Hartley on 10/09/2025.
//  Copyright 2025 GiantJelly. All rights reserved.
//

// #include <stddef.h>
#include <stdint.h>
// #include <math.h>
// #include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>


// Math needed for printing
inline int32_t abs32(int32_t x)
{
	int32_t mask = x >> 31;
	int32_t result = (x ^ mask) - mask;
	return result;
}
inline int64_t abs64(int64_t x)
{
	int64_t mask = x >> 63;
	int64_t result = (x ^ mask) - mask;
	return result;
}
inline double fabs64(double x)
{
	uint64_t* u = (uint64_t*)&x;
	*u &= 0x7FFFFFFFFFFFFFFF;
	return x;
}
inline double fmod64(double x, double y)
{
	double div = x / y;
	int64_t trunc = (int64_t)div;
	double result = x - (double)trunc * y;
	return result;
}
inline int clzll(uint64_t value)
{
	if (!value) {
		return 64;
	}

	int n = 0;
	while (!(value >> 63)) {
		++n;
		value <<= 1;
	}

	return n;
}
int _log2to10_tbl[] = {
	0,0,0,0,1,1,1,2,2,2,3,3,3,3,
	4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,9,9,
	10,10,10,11,11,11,12,12,12,12,13,13,13,
	14,14,14,15,15,15,15,16,16,16,17,17,17,
	18,18,18,18,
};
uint64_t _base10_tbl[] = {
	1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,
	10000000000,100000000000,1000000000000,10000000000000,
	100000000000000,1000000000000000,10000000000000000,
	100000000000000000,1000000000000000000,
};
int ilog10_64(uint64_t input) {
	uint64_t value = input;
	int lz = clzll(value | 1);
	int base2 = 63 - lz;
	int base10TblIndex = _log2to10_tbl[base2] + 1;
	if (value < _base10_tbl[base10TblIndex]) {
		--base10TblIndex;
	}
	return base10TblIndex;
}


uint32_t strsize(char* str) {
	if (!str) {
		return 0;
	}

	int size = 0;
	while(*str++) ++size;
	return size;
}


// SPRINTING
// TODO: Pull out setting buf chars into fucntion with len check
#define _sprint_write(c) if (buf && ci<len-1) buf[ci] = (c);

int sprint_u64(char* buf, int len, uint64_t num) {
	int ci = 0;
	if (!num) {
		// if (buf) {
		// 	if (ci<len-1) {
		// 		buf[ci++] = '0';
		// 	}
		// 	buf[ci] = 0;
		// } else {
		// 	++ci;
		// }
		_sprint_write('0');
		++ci;
		return ci;
	}
	
	int l = ilog10_64(num);

	// if (buf) {
	// 	if (ci+l+1 < len-1) {
	// 		for (int i=0; i<l+1; ++i) {
	// 			buf[ci + (l-i)] = num%10 + '0';
	// 			num /= 10;
	// 		}
	// 		ci += l+1;
	// 	}
	// 	buf[ci] = 0;
	// } else {
	// 	ci += l+1;
	// }

	// for (int i=l; i>=0; --i) {
	// 	uint64_t div = ipow64(10, i);
	// 	uint64_t tmp = i ? num / div : num;
	// 	// buf[ci + (l-i)] = num%10 + '0';
	// 	_sprint_write(tmp%10 + '0');
	// 	++ci;
	// 	// num /= 10;
	// }

	// TODO: Better
	ci += l;
	for (int i=l; i>=0; --i) {
		// uint64_t div = ipow64(10, i);
		// uint64_t tmp = i ? num / div : num;
		// buf[ci + (l-i)] = num%10 + '0';
		_sprint_write(num%10 + '0');
		--ci;
		num /= 10;
	}
	ci += l + 2;

	_sprint_write(0);

	return ci;
}

int sprint_i64(char* buf, int len, int64_t num) {
	int ci = 0;
	if (num < 0) {
		// if (buf) {
		// 	buf[ci++] = '-';
		// } else {
		// 	++ci;
		// }
		_sprint_write('-');
		++ci;
	}
	num = abs64(num);
	// if (buf) {
	// 	ci += sprint_u64(buf? buf+ci : buf, buf ? len-ci : len, num);
	// } else {
	// 	ci += sprint_u64(buf, len, num);
	// }
	ci += sprint_u64(buf? buf+ci : buf, buf ? len-ci : len, num);
	return ci;
}

int sprint_u32(char* buf, int len, uint32_t num) {
	return sprint_u64(buf, len, num);
}

int sprint_int(char* buf, int len, int32_t num) {
	int ci = 0;
	if (num < 0) {
		// if (buf) {
		// 	buf[ci++] = '-';
		// } else {
		// 	++ci;
		// }
		_sprint_write('-');
		++ci;
	}
	num = abs32(num);
	// if (buf) {
	// 	ci += sprint_u64(buf+ci, len-ci, num);
	// } else {
	// 	ci += sprint_u64(buf, len, num);
	// }
	ci += sprint_u64(buf? buf+ci : buf, buf ? len-ci : len, num);
	return ci;
}

int sprint_f64(char* buf, int len, double input, int precision) {
	double value = fabs64(input);
	int ci = 0;
	if (input < 0.0f) {
		// if (buf) {
		// 	buf[ci++] = '-';
		// } else {
		// 	++ci;
		// }
		_sprint_write('-');
		++ci;
	}

	int64_t wholePart = value;
	double fractPart = value - (double)wholePart;
	// if (buf) {
	// 	ci += sprint_i64(buf+ci, len-ci, wholePart);
	// } else {
	// 	ci += sprint_i64(buf, len, wholePart);
	// }
	ci += sprint_i64(buf? buf+ci : buf, buf ? len-ci : len, wholePart);

	// if (buf) {
	// 	if (ci<len-1) {
	// 		buf[ci++] = '.';
	// 	}
	// } else {
	// 	++ci;
	// }
	_sprint_write('.');
	++ci;

	// if (buf) {
	// 	for (int i=0; i<precision && ci<len-1; ++i) {
	// 		fractPart = fmodf(fractPart*10, 10);
	// 		int fractInt = fractPart;
	// 		buf[ci++] = '0' + fractInt;
	// 	}
	// 	buf[ci] = 0;
	// } else {
	// 	ci += len-1;
	// }

	// Using 50 as roughly the max string length of a float
	// TODO: Maybe implement nice algorithm like Dragon4 or Ryu
	int maxDigits = precision ? precision : 20;
	// for (int i=0; i<maxDigits; ++i) {
	// 	fractPart = fmodf(fractPart*10, 10);
	// 	int fractInt = fractPart;
	// 	if (fractInt) {
	// 		lastNonZero = i;
	// 	}
	// }
	int cistack = ci;
	for (int i=0; i<maxDigits; ++i) {
		fractPart = fmod64(fractPart*10, 10);
		int fractInt = fractPart;

		_sprint_write('0' + fractInt);
		++ci;

		if (!i || fractInt) {
			cistack = ci;
		}
	}

	ci = cistack;
	// ci -= zeroDigits;
	// buf[ci] = 0;
	_sprint_write(0);

	return ci;
}

int sprint_float(char* buf, int len, float input, int precision) {
	return sprint_f64(buf, len, input, precision);
}

int sprint_str(char* buf, int len, char* str) {
	if (!str) {
		str = "(null)";
	}

	int ci = 0;
	while (*str) {
		// if (buf) {
		// 	buf[ci++] = *str;
		// } else {
		// 	++ci;
		// }
		_sprint_write(*str);
		++ci;
		++str;
	}
	
	// if (buf) {
	// 	buf[ci] = 0;
	// }
	// _sprint_write(0);

	return ci;
}

int sprint_hex_nibble(char* buf, int len, uint8_t x)
{
	int ci = 0;
	bool isLetter = x >= 10;
	char base = '0' + (isLetter * ('A'-'0' - 10));
	char result = base + x;

	_sprint_write(result);
	return 1;
}
int sprint_hex8(char* buf, int len, uint8_t x)
{
	int ci = 0;
	
	// uint8_t mask = 0xF0;
	// int count = 2;
	// for (int c=0; c<count; ++c) {
	// 	sprint_hex_nibble(buf+c, len-c, x&mask >> ((count-c-1)*4));
	// 	++ci;
	// 	mask >>= 4;
	// }

	sprint_hex_nibble(buf+0, len-0, (x&0xF0) >> 4);
	sprint_hex_nibble(buf+1, len-1, (x&0x0F) >> 0);
	ci += 2;

	return ci;
}
int sprint_hex16(char* buf, int len, uint16_t x)
{
	int ci = 0;
	sprint_hex_nibble(buf+0, len-0, (x&0xF000) >> 12);
	sprint_hex_nibble(buf+1, len-1, (x&0x0F00) >> 8);
	sprint_hex_nibble(buf+2, len-2, (x&0x00F0) >> 4);
	sprint_hex_nibble(buf+3, len-3, (x&0x000F) >> 0);
	ci += 4;

	return ci;
}
int sprint_hex32(char* buf, int len, uint32_t x)
{
	int ci = 0;
	sprint_hex_nibble(buf+0, len-0, (x&0xF0000000) >> 28);
	sprint_hex_nibble(buf+1, len-1, (x&0x0F000000) >> 24);
	sprint_hex_nibble(buf+2, len-2, (x&0x00F00000) >> 20);
	sprint_hex_nibble(buf+3, len-3, (x&0x000F0000) >> 16);

	sprint_hex_nibble(buf+4, len-4, (x&0x0000F000) >> 12);
	sprint_hex_nibble(buf+5, len-5, (x&0x00000F00) >> 8);
	sprint_hex_nibble(buf+6, len-6, (x&0x000000F0) >> 4);
	sprint_hex_nibble(buf+7, len-7, (x&0x0000000F) >> 0);
	ci += 8;

	return ci;
}
int sprint_hex64(char* buf, int len, uint64_t x)
{
	sprint_hex32(buf, len, x >> 32);
	sprint_hex32(buf+8, len-8, (x & 0x00000000FFFFFFFF));
	
	return 16;
}

// char* HexStr16(uint16_t value)
// {
// 	uint64_t result = 0;
// 	// char str[5] = {0};
// 	char* str = strcopy("0000");
// 	// sys_zero_memory(str, 5);

// 	// str[0] = ((reg.lo>>4)&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;
// 	// str[1] = (reg.lo&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;

// 	// str[0] = '0';
// 	// str[1] = 'x';
// 	int s = 0;
// 	str[s+0] = HexNibbleStr((value&0xF000)>>12);
// 	str[s+1] = HexNibbleStr((value&0x0F00)>>8);
// 	str[s+2] = HexNibbleStr((value&0x00F0)>>4);
// 	str[s+3] = HexNibbleStr((value&0x000F));

// 	return str;
// 	// sys_copy_memory(&result, str, 5);
// 	// return result;
// }

// char* HexByteStr(reg_t reg)
// {
// 	uint64_t result = 0;
// 	// char str[5] = {0};
// 	char* str = strcopy("0000");
// 	// sys_zero_memory(str, 5);

// 	// str[0] = ((reg.lo>>4)&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;
// 	// str[1] = (reg.lo&0x0F) < 10 ? '0'+reg.lo : 'A'+reg.lo-10;

// 	// str[0] = '0';
// 	// str[1] = 'x';
// 	int s = 0;
// 	str[s+0] = HexNibbleStr(reg.hi>>4);
// 	str[s+1] = HexNibbleStr(reg.hi&0x0F);
// 	str[s+2] = HexNibbleStr(reg.lo>>4);
// 	str[s+3] = HexNibbleStr(reg.lo&0x0F);

// 	return str;
// 	// sys_copy_memory(&result, str, 5);
// 	// return result;
// }

int vsprint(char* buf, int len, char* fmt, va_list args) {
	int ci = 0;

	while (*fmt) {
		if (fmt[0] == '%') {
			char* subBuf = buf ? buf+ci : buf;
			int subLen = buf ? len-ci : len;

			if (fmt[1] == 'i') {
				int intlen = sprint_int(subBuf, subLen, va_arg(args, int32_t));
				fmt += 2;
				ci += intlen;
				continue;
			}
			else if (fmt[1] == 'u') {
				int intlen = sprint_u32(subBuf, subLen, va_arg(args, uint32_t));
				fmt += 2;
				ci += intlen;
				continue;
			}
			else if (fmt[1] == 'l' && fmt[2] == 'i') {
				int intlen = sprint_i64(subBuf, subLen, va_arg(args, int64_t));
				ci += intlen;
				fmt += 3;
				continue;
			}
			else if (fmt[1] == 'l' && fmt[2] == 'u') {
				int intlen = sprint_u64(subBuf, subLen, va_arg(args, uint64_t));
				ci += intlen;
				fmt += 3;
				continue;
			}
			// else if (fmt[1] == 'f') {
			// 	int insertLen = sprint_float(subBuf, subLen, va_arg(args, double), 3);
			// 	ci += insertLen;
			// 	fmt += 2;
			// 	continue;
			// }
			// else if (fmt[1] == 'l' && fmt[2] == 'f') {
			// 	int insertLen = sprint_f64(subBuf, subLen, va_arg(args, double), 3);
			// 	ci += insertLen;
			// 	fmt += 3;
			// 	continue;
			// }

			else if (fmt[1]=='4' && fmt[2]=='x') {
				int intlen = sprint_hex_nibble(subBuf, subLen, va_arg(args, int));
				ci += intlen;
				fmt += 3;
				continue;
			}
			else if (fmt[1]=='8' && fmt[2]=='x') {
				int intlen = sprint_hex8(subBuf, subLen, va_arg(args, int));
				ci += intlen;
				fmt += 3;
				continue;
			}
			else if (fmt[1]=='1' && fmt[2]=='6' && fmt[3]=='x') {
				int intlen = sprint_hex16(subBuf, subLen, va_arg(args, int));
				ci += intlen;
				fmt += 4;
				continue;
			}
			else if (fmt[1]=='3' && fmt[2]=='2' && fmt[3]=='x') {
				int intlen = sprint_hex32(subBuf, subLen, va_arg(args, int));
				ci += intlen;
				fmt += 4;
				continue;
			}
			else if (fmt[1]=='6' && fmt[2]=='4' && fmt[3]=='x') {
				int intlen = sprint_hex64(subBuf, subLen, va_arg(args, uint64_t));
				ci += intlen;
				fmt += 4;
				continue;
			}

			else if (fmt[1] == 'f') {
				int insertLen = sprint_f64(subBuf, subLen, va_arg(args, double), 3);
				ci += insertLen;
				fmt += 2;
				continue;
			}
			else if (fmt[1] == 's') {
				ci += sprint_str(subBuf, subLen, va_arg(args, char*));
				fmt += 2;
				continue;
			}
			else if (fmt[1] == 'c') {
				// ci += sprint_str(subBuf, subLen, va_arg(args, char*));
				char c = (char)va_arg(args, int);
				_sprint_write(c);
				++ci;
				fmt += 2;
				continue;
			}
		}

		// if (buf) {
		// 	buf[ci++] = fmt[0];
		// } else {
		// 	++ci;
		// }
		_sprint_write(fmt[0]);
		++ci;
		++fmt;
	}

	// if (buf) {
	// 	buf[ci] = 0;
	// }
	if (buf && ci<len) buf[ci] = 0; 
	return ci;
}

int sprint(char* buf, int len, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ci = vsprint(buf, len, fmt, args);
	va_end(args);
	return ci;
}
