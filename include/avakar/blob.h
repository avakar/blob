#ifndef AVAKAR_BLOB_H
#define AVAKAR_BLOB_H

#include "../../src/node.h"

#include <string_view>
#include <vector>
#include <stdint.h>

namespace avakar {

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
	size_t size() const;

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

	blob & operator+=(blob const & rhs);

private:
	_rope_memory_node * _root;
	std::string_view _sv;

	friend struct rope;
};

}

#endif // AVAKAR_BLOB_H
