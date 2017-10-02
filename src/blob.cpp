#include <avakar/blob.h>
#include <algorithm>

#include "storage.h"
using namespace avakar;

blob::blob()
	: _root(nullptr), _sv()
{
}

blob::blob(char const * str)
	: _root(nullptr), _sv(str)
{
}

blob::blob(char const * str, size_t len)
	: _root(nullptr), _sv(str, str + len)
{
}

blob::blob(char const * first, char const * last)
	: _root(nullptr), _sv(first, last)
{
}

blob::blob(blob const & o)
	: _root(o._root), _sv(o._sv)
{
	if (_root)
		_root->addref();
}

blob::blob(blob && o)
	: _root(o._root)
{
	o._root = nullptr;
}

blob::blob(std::string const & str)
	: _root(nullptr), _sv(str)
{
}

blob::blob(std::string && str)
{
	_root = new _rope_string_node(std::move(str));
	_sv = _root->view();
}

blob::blob(std::vector<char> const & v)
	: _root(nullptr), _sv(v.data(), v.size())
{
}

blob::blob(std::vector<char> && v)
{
	_root = new _rope_vector_node(std::move(v));
	_sv = _root->view();
}

blob::blob(std::string_view str)
	: _root(nullptr), _sv(str)
{
}

blob::~blob()
{
	if (_root)
		_root->release();
}

blob & blob::operator=(blob o)
{
	std::swap(_root, o._root);
	_sv = o._sv;
	return *this;
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
