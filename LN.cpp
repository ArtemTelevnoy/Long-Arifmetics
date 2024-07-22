#include "LN.h"

LN::LN(long long a) : LN(true, a) {}

LN::LN(const LN &a)
{
	this->sign = a.sign;
	this->isNan = a.isNan;
	this->except((this->arr = new uI32[(this->memLen = this->len = a.len)]));
	except(memcpy(this->arr, a.arr, a.len * suI32));
}

LN::LN(LN &&a) noexcept
{
	this->len = a.len;
	this->memLen = a.memLen;
	this->sign = a.sign;
	this->isNan = a.isNan;
	this->arr = a.arr;
	a.len = a.memLen = 0;
	a.sign = a.isNan = false;
	a.arr = nullptr;
}

LN::LN(bool log, long long a)
{
	if (!log)
		return;

	this->sign = a < 0;
	if (a != 0x8000000000000000)
		a = std::abs(a);
	uI32 left = a & 0xFFFFFFFF, right = (a >> 32) & 0xFFFFFFFF;
	this->except(this->arr = new uI32[(this->memLen = this->len = 1 + (right > 0))]);
	this->arr[0] = left;
	if (this->len == 2)
		this->arr[1] = right;

	this->validFormat();
}

LN::LN(const char *a) : LN(!a || !strcmp(a, "NaN"), a, strlen(a)) {}

LN::LN(std::string_view a) : LN(a == "NaN", a, a.length()) {}

LN::LN(bool log, auto a, st length)
{
	if (log)
		this->isNan = true;
	else
	{
		st count = length;
		if ((this->sign = a[0] == '-'))
			count--;
		while (count > 1 && a[length - count] == '0')
			count--;
		this->except((this->arr = new uI32[(this->memLen = this->len = count / 8 + (count % 8 > 0))]));

		for (st i = 0, j = length - 1; i < this->len; i++, count -= 8)
		{
			this->arr[i] = 0x0;
			for (st k = 0; k < (count < 8 ? count : 8); k++)
				this->arr[i] += converter((char)a[j--]) << (4 * k);
		}
	}
	this->validFormat();
}

LN::~LN()
{
	delete[] this->arr;
}

LN operator+(const LN &a, const LN &b)
{
	LN c(false, 0);
	if ((c.isNan = a.isNan || b.isNan))
	{
		c.nanConverter();
		return c;
	}
	if (a.sign && b.sign)
		return -(LN::adder(-a, -b));
	if (a.sign && !b.sign)
		return LN::suber(b, -a);
	if (!a.sign && b.sign)
		return LN::suber(a, -b);

	return LN::adder(a, b);
}

LN operator-(const LN &a, const LN &b)
{
	LN c(false, 0);
	if ((c.isNan = a.isNan || b.isNan))
	{
		c.nanConverter();
		return c;
	}
	if (a.sign && b.sign)
		return LN::suber(-a, -b);
	if (!a.sign && b.sign)
		return LN::adder(a, -b);
	if (a.sign && !b.sign)
		return -LN::adder(-a, b);

	return LN::suber(a, b);
}

LN operator*(const LN &a, const LN &b)
{
	LN c(false, 0);
	if ((c.isNan = a.isNan || b.isNan))
	{
		c.nanConverter();
		return c;
	}
	c.sign = b.sign ^ a.sign;
	LN aa = (b.len > a.len) ? b : a;
	LN bb = (b.len > a.len) ? a : b;

	c.arr = new uI32[(c.memLen = c.len = a.len + b.len)];
	memset(c.arr, 0, c.len * suI32);
	int overflow;
	for (st i = 0; i < bb.len; i++)
	{
		for (st j = 0; j < aa.len; j++)
		{
			uI64 multiply = (uI64)b.arr[i] * a.arr[j];
			uI32 left = multiply >> 32;
			uI32 right = multiply & 0xFFFFFFFF;

			if ((overflow = UINT32_MAX - c.arr[i + j] < right))
			{
				st index = i + j + 1;
				while (overflow)
				{
					int overflow2 = UINT32_MAX - c.arr[index] < overflow;
					c.arr[index++] += overflow;
					overflow = overflow2;
				}
			}
			c.arr[i + j] += right;

			if ((overflow = UINT32_MAX - c.arr[i + j + 1] < left))
			{
				st index = i + j + 2;
				while (overflow)
				{
					int overflow2 = UINT32_MAX - c.arr[index] < overflow;
					c.arr[index++] += overflow;
					overflow = overflow2;
				}
			}
			c.arr[i + j + 1] += left;
		}
	}
	c.validFormat();

	return c;
}

LN operator/(const LN &a, const LN &b)
{
	return LN::divider(a, b, true);
}

LN operator%(const LN &a, const LN &b)
{
	return LN::divider(a, b, false);
}

LN LN::operator-() const
{
	LN c(*this);
	if (static_cast< bool >(this))
		c.sign = !c.sign;
	return c;
}

LN LN::operator+() const
{
	LN c(*this);
	return c;
}

LN LN::operator~() const
{
	LN c(*this);
	if ((c.isNan |= c.sign))
	{
		c.nanConverter();
		return c;
	}

	LN one(1), two(2), mid(false, 0), left, right(*this);

	while ((right - left) > one)
	{
		mid = (left + right) / two;
		if ((mid * mid) < *this)
			c = left = mid;
		else if ((mid * mid) == *this)
		{
			c = mid;
			break;
		}
		else
			right = mid;
	}

	c.validFormat();
	return c;
}

LN &LN::operator=(const LN &a)
{
	if (this != &a)
	{
		this->sign = a.sign;
		this->isNan = a.isNan;
		delete[] this->arr;
		this->arr = new uI32[(this->len = this->memLen = a.len)];
		this->except(this->arr);
		except(memcpy(this->arr, a.arr, a.len * suI32));
	}
	return *this;
}

LN &LN::operator=(LN &&a) noexcept
{
	this->len = a.len;
	this->memLen = a.memLen;
	this->sign = a.sign;
	this->isNan = a.isNan;
	delete[] this->arr;
	this->arr = a.arr;
	a.len = a.memLen = 0;
	a.sign = a.isNan = false;
	a.arr = nullptr;
	return *this;
}

LN &LN::operator++()
{
	if (!this->sign)
		this->incrementAbs();
	else
		this->decrementAbs();
	return *this;
}

LN &LN::operator--()
{
	if (this->sign)
		this->incrementAbs();
	else
		this->decrementAbs();
	return *this;
}

LN LN::operator++(int)
{
	LN c = LN(*this);
	if (!this->sign)
		this->incrementAbs();
	else
		this->decrementAbs();
	return c;
}

LN LN::operator--(int)
{
	LN c(*this);
	if (this->sign)
		this->incrementAbs();
	else
		this->decrementAbs();
	return c;
}

LN &LN::operator+=(const LN &a)
{
	return (*this = *this + a);
}

LN &LN::operator-=(const LN &a)
{
	return (*this = *this - a);
}

LN &LN::operator*=(const LN &a)
{
	return (*this = *this * a);
}

LN &LN::operator/=(const LN &a)
{
	return (*this = *this / a);
}

LN &LN::operator%=(const LN &a)
{
	return (*this = *this % a);
}

bool operator==(const LN &a, const LN &b)
{
	if (a.isNan || b.isNan || a.sign != b.sign || a.len != b.len)
		return false;
	for (st i = 0; i < a.len; i++)
		if (a.arr[i] != b.arr[i])
			return false;
	return true;
}

bool operator!=(const LN &a, const LN &b)
{
	return !(a == b);
}

bool operator<(const LN &a, const LN &b)
{
	if (a.isNan || b.isNan)
		return false;
	if (a.sign != b.sign)
		return a.sign > b.sign;
	if (a.len != b.len)
		return (a.len < b.len) ^ a.sign;
	else
		for (st i = 1; i <= a.len; i++)
			if (b.arr[b.len - i] != a.arr[a.len - i])
				return (b.arr[b.len - i] > a.arr[a.len - i]) ^ a.sign;
	return false;
}

bool operator>(const LN &a, const LN &b)
{
	return !a.isNan && !b.isNan && !(a <= b);
}

bool operator<=(const LN &a, const LN &b)
{
	return !a.isNan && !b.isNan && (a < b || a == b);
}

bool operator>=(const LN &a, const LN &b)
{
	return !a.isNan && !b.isNan && !(a < b);
}

LN::operator bool() const
{
	return !this->isNan && (this->len != 1 || this->arr[0]);
}

LN::operator long long() const
{
	if (this->isNan || this->len > 2 ||
		this->len > 1 && (this->arr[1] > 0x80000000 || this->arr[1] == 0x80000000 && this->arr[0] || !this->sign && this->arr[1] > 0x7FFFFFFF))
		throw std::runtime_error("Impossible convert to long long");

	return (long long)((this->sign ? -1 : 1) * this->arr[0] + (this->sign ? -1 : 1) * ((this->len > 1 ? this->arr[1] : 0) * 0x100000000));
}

uI32 LN::converter(char a)
{
	for (std::pair b : convert)
		if (b.second == toupper(a))
			return b.first;
	return -1;
}

LN LN::adder(const LN &a, const LN &b)
{
	LN c(false, 0);
	c.sign = false;
	c.except((c.arr = new uI32[(c.memLen = c.len = 1 + (a.len > b.len ? a.len : b.len))]));

	int overFlow = 0;
	for (st i = 0; i < c.len; i++)
	{
		uI32 aa = i > a.len - 1 ? 0 : a.arr[i];
		uI32 bb = i > b.len - 1 ? 0 : b.arr[i];
		c.arr[i] = aa + bb + overFlow;
		overFlow = UINT32_MAX - aa < bb || UINT32_MAX - aa - bb < overFlow;
	}
	if (overFlow)
		c.arr[c.len++] = overFlow;
	c.validFormat();

	return c;
}

LN LN::suber(const LN &a, const LN &b)
{
	LN c(false, 0);
	LN aa = (b < a) ? a : b;
	LN bb = (b < a) ? b : a;

	c.sign = a < b;
	c.except((c.arr = new uI32[(c.memLen = c.len = aa.len)]));

	int overflow = 0;
	for (st i = 0; i < aa.len; i++)
	{
		uI32 aaa = i > aa.len - 1 ? 0 : aa.arr[i];
		uI32 bbb = i > bb.len - 1 ? 0 : bb.arr[i];
		c.arr[i] = aaa - bbb - overflow;
		overflow = bbb > aaa || aaa - bbb < overflow;
	}
	c.validFormat();

	return c;
}

LN LN::divider(const LN &a, const LN &b, bool answerSave)
{
	LN c;
	if ((c.isNan = a.isNan || !(bool)b))
	{
		c.nanConverter();
		return c;
	}

	LN aa(a), bb(b);
	aa.sign = bb.sign = false;
	LN predRes[16];
	predRes[0] = LN();
	if (aa < bb)
	{
		aa.sign = a.sign;
		return answerSave ? predRes[0] : aa;
	}

	LN indexer[16];
	indexer[0] = predRes[0];
	for (int i = 1; i < 0x10; i++)
	{
		predRes[i] = (predRes[i - 1] + bb);
		indexer[i] = LN(i);
	}

	st ind = 0, length = counterNumbers(a.arr[a.len - 1]);

	LN sixteen(0x10), bin;
	do
	{
		uI32 he = indexNumber(aa, ind++, length);
		(bin *= sixteen) += indexer[he];

		int m, l = 0, r = 0x10;
		while (r - l > 1)
		{
			if (predRes[(m = (l + r) / 2)] > bin)
				r = m;
			else
				l = m;
		}
		if (answerSave)
		{
			c *= sixteen;
			uI32 overflow = UINT32_MAX - c.arr[0] < l;
			c.arr[0] += l;
			st cIndex = 1;
			while (overflow)
			{
				uI32 overflow2 = UINT32_MAX - c.arr[cIndex] < overflow;
				c.arr[cIndex++] += overflow;
				overflow = overflow2;
			}
		}

		bin -= predRes[l];

	} while (ind < a.len * 8 - 8 + length);

	if (!answerSave)
	{
		bin.sign = a.sign;
		bin.validFormat();
		return bin;
	}

	c.sign = a.sign ^ b.sign;
	c.validFormat();
	return c;
}

uI32 LN::indexNumber(const LN &a, st index, st firstCount)
{
	return (index < firstCount ? (a.arr[a.len - 1] >> ((firstCount - index - 1) * 4))
							   : (a.arr[a.len - 2 - (index -= firstCount) / 8] >> ((7 - index) * 4))) &
		   0xF;
}

void LN::incrementAbs()
{
	if (this->isNan)
		return;
	int overflow = UINT32_MAX == this->arr[0]++;
	st ind = 1;
	while (ind < this->len && overflow)
	{
		int overflow2 = UINT32_MAX - this->arr[ind] < overflow;
		this->arr[ind++] += overflow;
		overflow = overflow2;
	}
	if (overflow)
	{
		if (this->memLen > this->len)
			this->arr[ind] = 1;
		else
		{
			uI32 *newArr = new uI32[(this->memLen *= 2)];
			this->except(newArr);
			except(memcpy(this->arr, newArr, this->len++ * suI32));
			delete[] this->arr;
			this->arr = newArr;
		}
	}
	this->validFormat();
}

void LN::decrementAbs()
{
	if (this->isNan)
		return;
	if (!static_cast< bool >(*this))
	{
		this->sign = true;
		this->arr[0] = 1;
		return;
	}
	if (this->arr[0] == 1)
	{
		this->sign = false;
		this->arr[0] = 0;
		return;
	}
	int overflow = !this->arr[0]--;
	st ind = 1;
	while (overflow)
	{
		int overflow2 = this->arr[ind] < overflow;
		this->arr[ind++] -= overflow;
		overflow = overflow2;
	}
	this->validFormat();
}

void LN::validFormat()
{
	if (this->isNan)
	{
		this->nanConverter();
		return;
	}
	st j = 0, i = this->len - 1;
	while (j != this->len - 1 && !this->arr[i--])
		j++;

	this->len -= j;
	if (j && this->memLen / this->len >= 2)
	{
		uI32 *newArr = new uI32[(this->memLen >>= 1)];
		this->except(newArr, true);
		except(memcpy(newArr, this->arr, this->len * suI32));
		delete[] this->arr;
		this->arr = newArr;
	}

	if (!static_cast< bool >(this))
		this->sign = false;
}

void LN::nanConverter()
{
	this->sign = false;
	this->len = this->memLen = 0;
	delete[] this->arr;
	this->arr = nullptr;
}

/*void LN::printer(const LN &a)
{
	if (a.isNan)
	{
		std::cout << "NaN";
		return;
	}
	if (a.sign)
		std::cout << "-";
	printf("%X", a.arr[a.len - 1]);
	for (st i = 2; i <= a.len; i++)
	{
		for (int j = 0; j < 8 - counterNumbers(a.arr[a.len - i]); j++)
			std::cout << '0';
		printf("%X", a.arr[a.len - i]);
	}
	std::cout << " ending" << std::endl;
}*/

void LN::except(const uI32 *a, bool mode)
{
	if (!a)
	{
		if (mode)
		{
			delete[] this->arr;
			this->arr = nullptr;
		}
		throw std::runtime_error("Out of memory");
	}
}

void LN::except(void *a)
{
	if (!a)
		throw std::runtime_error("Invalid coping");
}

st LN::counterNumbers(uI32 a)
{
	if (!a)
		return 1;
	st count = 0;
	while (a > 0)
	{
		a >>= 4;
		count++;
	}
	return count;
}

int LN::writer(const LN &a, FILE *wp)
{
	if (a.isNan)
		return fprintf(wp, "NaN") < 0;
	if (a.sign && fprintf(wp, "-") < 0 || fprintf(wp, "%X", a.arr[a.len - 1]) < 0)
		return 1;

	for (st i = 2; i <= a.len; i++)
	{
		for (int j = 0; j < 8 - counterNumbers(a.arr[a.len - i]); j++)
			if (fprintf(wp, "0") < 0)
				return 1;
		if (fprintf(wp, "%X", a.arr[a.len - i]) < 0)
			return 1;
	}

	return fprintf(wp, "\n") < 0;
}
