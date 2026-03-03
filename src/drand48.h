#pragma once
#include <cmath>
#include <random>

#define RAND48_SEED_0   (0x330e)
#define RAND48_SEED_1 (0xabcd)
#define RAND48_SEED_2 (0x1234)
#define RAND48_MULT_0 (0xe66d)
#define RAND48_MULT_1 (0xdeec)
#define RAND48_MULT_2 (0x0005)
#define RAND48_ADD (0x000b)

constexpr auto M_PI = 3.1415826535897932385;
const float inf = std::numeric_limits<float>::infinity();

unsigned short _rand48_seed[3] = {
		RAND48_SEED_0,
		 RAND48_SEED_1,
		 RAND48_SEED_2
};
unsigned short _rand48_mult[3] = {
		 RAND48_MULT_0,
		 RAND48_MULT_1,
		 RAND48_MULT_2
};
unsigned short _rand48_add = RAND48_ADD;

inline void
_dorand48(unsigned short xseed[3])
{
	unsigned long accu;
	unsigned short temp[2];

	accu = (unsigned long)_rand48_mult[0] * (unsigned long)xseed[0] +
		(unsigned long)_rand48_add;
	temp[0] = (unsigned short)accu;        /* lower 16 bits */
	accu >>= sizeof(unsigned short) * 8;
	accu += (unsigned long)_rand48_mult[0] * (unsigned long)xseed[1] +
		(unsigned long)_rand48_mult[1] * (unsigned long)xseed[0];
	temp[1] = (unsigned short)accu;        /* middle 16 bits */
	accu >>= sizeof(unsigned short) * 8;
	accu += _rand48_mult[0] * xseed[2] + _rand48_mult[1] * xseed[1] + _rand48_mult[2] * xseed[0];
	xseed[0] = temp[0];
	xseed[1] = temp[1];
	xseed[2] = (unsigned short)accu;
}

inline float erand48(unsigned short xseed[3])
{
	_dorand48(xseed);
	return ldexp((float)xseed[0], -48) +
		ldexp((float)xseed[1], -32) +
		ldexp((float)xseed[2], -16);
}

inline float drand48() {
	return erand48(_rand48_seed);
}

inline void srand48(long seed) {
	_rand48_seed[0] = RAND48_SEED_0;
	_rand48_seed[1] = (unsigned short)seed;
	_rand48_seed[2] = (unsigned short)(seed >> 16);
	_rand48_mult[0] = RAND48_MULT_0;
	_rand48_mult[1] = RAND48_MULT_1;
	_rand48_mult[2] = RAND48_MULT_2;
	_rand48_add = RAND48_ADD;
}

inline float random_float() {
	static std::uniform_real_distribution<float> distribution(0., 1.);
	static std::mt19937 generator;
	return distribution(generator);
}

inline float random_float(float min, float max) {
	return min + (max - min) * random_float();
}

inline int random_int(int min, int max) {
	static std::uniform_int_distribution<int> distribution(min, max);
	static std::mt19937 generator;
	return distribution(generator);
}