#ifndef LAB3_LN_H
#define LAB3_LN_H

#include <cstring>
#include <iostream>
#include <stdexcept>

using uI32 = uint32_t;
using uI64 = uint64_t;
using st = size_t;

#define suI32 sizeof(uI32)

// using namespace std;

const std::pair< uI32, char > convert[16] = {
	{ 0x0, '0' }, { 0x1, '1' }, { 0x2, '2' }, { 0x3, '3' }, { 0x4, '4' }, { 0x5, '5' }, { 0x6, '6' }, { 0x7, '7' },
	{ 0x8, '8' }, { 0x9, '9' }, { 0xA, 'A' }, { 0xB, 'B' }, { 0xC, 'C' }, { 0xD, 'D' }, { 0xE, 'E' }, { 0xF, 'F' }
};

class LN
{
  public:
	explicit LN(long long = 0);
	LN(const LN &);
	LN(LN &&) noexcept;
	explicit LN(const char *);
	explicit LN(std::string_view);

	~LN();

	friend LN operator+(const LN &, const LN &);
	friend LN operator-(const LN &, const LN &);
	friend LN operator*(const LN &, const LN &);
	friend LN operator/(const LN &, const LN &);
	friend LN operator%(const LN &, const LN &);

	LN operator++(int);
	LN operator--(int);
	LN operator-() const;
	LN operator+() const;
	LN operator~() const;

	LN &operator=(const LN &);
	LN &operator=(LN &&) noexcept;

	LN &operator++();
	LN &operator--();
	LN &operator+=(const LN &);
	LN &operator-=(const LN &);
	LN &operator*=(const LN &);
	LN &operator/=(const LN &);
	LN &operator%=(const LN &);

	friend bool operator==(const LN &, const LN &);
	friend bool operator!=(const LN &, const LN &);
	friend bool operator<(const LN &, const LN &);
	friend bool operator>(const LN &, const LN &);
	friend bool operator<=(const LN &, const LN &);
	friend bool operator>=(const LN &, const LN &);

	explicit operator bool() const;
	explicit operator long long() const;

	// static void printer(const LN &);
	static int writer(const LN &, FILE *);

  private:
	bool isNan = false, sign = false;
	uI32 *arr = nullptr;
	st len = 0, memLen = 0;

	LN(bool, auto, st);
	explicit LN(bool, long long);

	void validFormat();
	void nanConverter();
	void incrementAbs();
	void decrementAbs();
	void except(const uI32 *, bool = false);
	static void except(void *);
	static st counterNumbers(uI32);
	static uI32 indexNumber(const LN &, st, st);
	static LN divider(const LN &, const LN &, bool);
	static uI32 converter(char);
	static LN adder(const LN &, const LN &);
	static LN suber(const LN &, const LN &);
};

inline LN operator"" _ln(const char *a, st)
{
	return LN(a);
}

inline LN operator"" _ln(const char *a)
{
	return LN(a + (strlen(a) >= 2 && a[0] == '0' && a[1] == 'x') * 2);
}

#endif