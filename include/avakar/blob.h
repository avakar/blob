#ifndef AVAKAR_BLOB_H
#define AVAKAR_BLOB_H

#include <string_view>
#include <vector>
#include <stdint.h>

// _ref_counter
#include <atomic>

namespace avakar {

struct _ref_counter
{
	virtual void destroy() = 0;
	virtual void read(char * buf, uint64_t offs, size_t len) = 0;
	virtual uint64_t length() const = 0;

	std::atomic<size_t> count_;
};

struct _in_memory
	: _ref_counter
{
	virtual std::string_view get() const = 0;
	void read(char * buf, uint64_t offs, size_t len) override;
	uint64_t length() const override;
};

struct blob
{
	blob();
	blob(blob const & o);
	blob(blob && o);

	blob(char const * str);
	blob(char const * str, size_t len);
	blob(char const * first, char const * last);
	blob(std::string_view str);

	blob(std::string const & str);
	blob(std::string && str);
	blob(std::vector<char> const & v);
	blob(std::vector<char> && v);

	~blob();

	blob & operator=(blob o);

	bool empty() const;

	blob operator()(size_t offs) const;
	blob operator()(size_t offs, size_t len) const;

	std::string_view str() const;
	operator std::string_view() const;

	friend bool operator==(blob const & lhs, blob const & rhs);
	friend bool operator!=(blob const & lhs, blob const & rhs);
	friend bool operator<(blob const & lhs, blob const & rhs);
	friend bool operator<=(blob const & lhs, blob const & rhs);
	friend bool operator>(blob const & lhs, blob const & rhs);
	friend bool operator>=(blob const & lhs, blob const & rhs);

	friend blob operator+(blob const & lhs, blob const & rhs);

	friend struct rope;
	friend rope operator|(blob const & lhs, blob const & rhs);

private:
	_in_memory * rc_;
	std::string_view sv_;
};

struct rope
{
	rope();
	rope(rope const & o);
	rope(rope && o);
	rope(blob b);
	~rope();

	rope & operator=(rope o);

	bool empty() const;

	rope operator()(size_t offs) const;
	rope operator()(size_t offs, size_t len) const;

	std::string str() const;

	uint64_t length() const;
	void read(char * buf, uint64_t offs, size_t len) const;
	std::string read(uint64_t offs, size_t len) const;

	friend rope operator|(rope lhs, rope rhs);

private:
	_ref_counter * rc_;

	struct sv
	{
		char const * first;
		char const * last;
	};

	struct slice
	{
		uint64_t first;
		uint64_t last;
	};

	union
	{
		sv sv_;
		slice slice_;
	};
};


}

#endif // AVAKAR_BLOB_H
