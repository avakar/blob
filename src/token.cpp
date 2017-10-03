#include <avakar/token.h>
using namespace avakar;

token::token()
	: _ptr(nullptr)
{
}

token::token(_token_base * ptr)
	: _ptr(ptr)
{
}

token::token(token && o)
	: _ptr(o._ptr)
{
	o._ptr = nullptr;
}

token::token(token const & o)
	: _ptr(o._ptr)
{
	if (_ptr)
		++_ptr->_ref_count;
}

token::~token()
{
	this->clear();
}

bool token::empty() const
{
	return _ptr == nullptr;
}

bool token::unique() const
{
	return _ptr && _ptr->_ref_count == 1;
}

void token::clear()
{
	if (_ptr)
	{
		if (--_ptr->_ref_count == 0)
			delete _ptr;
		_ptr = nullptr;
	}
}

token & token::operator=(token o)
{
	std::swap(_ptr, o._ptr);
	return *this;
}
