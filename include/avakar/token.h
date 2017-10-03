#ifndef AVAKAR_TOKEN_H
#define AVAKAR_TOKEN_H

#include <atomic>
#include <type_traits>
#include <utility>

namespace avakar {

struct _token_base
{
	virtual ~_token_base() {}

	std::atomic<size_t> _ref_count = 1;
};

template <typename T>
struct _token final
	: _token_base
{
	template <typename... An>
	_token(An &&... an)
		: value(std::forward<An>(an)...)
	{
	}

	T value;
};

struct token
{
public:
	token();
	token(token && o);
	token(token const & o);
	~token();

	bool empty() const;
	bool unique() const;
	void clear();

	token & operator=(token o);

	template <typename T>
	T & get() const
	{
		auto * tok = static_cast<_token<T> *>(_ptr);
		return tok->value;
	}

	template <typename T, typename... An>
	static token create(An &&... an)
	{
		_token_base * tok = new _token<T>(std::forward<An>(an)...);
		return token(tok);
	}

private:
	explicit token(_token_base * ptr);

	_token_base * _ptr;
};

template <typename T>
T & get(token const & t)
{
	return t.get<T>();
}

template <typename T, typename... An>
token make_token(An &&... an)
{
	return token::create<T>(std::forward<An>(an)...);
}

}

#endif // AVAKAR_TOKEN_H
