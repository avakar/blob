#include <avakar/blob.h>
#include <algorithm>

#include "storage.h"
using namespace avakar;

blob::blob()
	: _sv()
{
}

blob::blob(char const * str)
	: _sv(str)
{
}

blob::blob(char const * str, size_t len)
	: _sv(str, len)
{
}

blob::blob(char const * first, char const * last)
	: _sv(first, last - first)
{
}

blob::blob(std::string const & str)
	: _sv(str)
{
}

blob::blob(std::string && str)
{
	_storage = make_token<std::string>(std::move(str));
	_sv = get<std::string>(_storage);
}

blob::blob(std::vector<char> const & v)
	: _sv(v.data(), v.size())
{
}

blob::blob(std::vector<char> && v)
{
	_storage = make_token<std::vector<char>>(std::move(v));
	auto & stored = get<std::vector<char>>(_storage);
	_sv = { stored.data(), stored.size() };
}

blob::blob(std::string_view str)
	: _sv(str)
{
}

bool blob::empty() const
{
	return _sv.empty();
}

size_t blob::size() const
{
	return _sv.size();
}

std::string_view blob::str() const
{
	return _sv;
}

blob::operator std::string_view() const
{
	return _sv;
}

blob blob::operator()(size_t offs) const
{
	blob r = *this;
	r._sv = r._sv.substr(offs);
	return r;
}

blob blob::operator()(size_t offs, size_t len) const
{
	blob r = *this;
	r._sv = r._sv.substr(offs, len);
	return r;
}

bool avakar::operator==(blob const & lhs, blob const & rhs)
{
	return lhs._sv == rhs._sv;
}

bool avakar::operator!=(blob const & lhs, blob const & rhs)
{
	return !(lhs == rhs);
}

bool avakar::operator<(blob const & lhs, blob const & rhs)
{
	return lhs._sv < rhs._sv;
}

bool avakar::operator<=(blob const & lhs, blob const & rhs)
{
	return !(rhs < lhs);
}

bool avakar::operator>(blob const & lhs, blob const & rhs)
{
	return rhs < lhs;
}

bool avakar::operator>=(blob const & lhs, blob const & rhs)
{
	return !(lhs < rhs);
}

blob avakar::operator+(blob const & lhs, blob const & rhs)
{
	std::string r;
	r.reserve(lhs._sv.size() + rhs._sv.size());
	r.assign(lhs._sv.data(), lhs._sv.size());
	r.append(rhs._sv.data(), rhs._sv.size());
	return blob(std::move(r));
}

blob & blob::operator+=(blob const & rhs)
{
	*this = *this + rhs;
	return *this;
}
