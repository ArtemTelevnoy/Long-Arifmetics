# Long-Arifmetics
Class for working with long numbers
LN:
```
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

	static int writer(const LN &, FILE *);
}
```

All operators work as usual(just override). __writer__ - write number in file
