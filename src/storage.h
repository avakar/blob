#ifndef AVAKAR_BLOB_STORAGE_H
#define AVAKAR_BLOB_STORAGE_H

#include "node.h"
#include <utility>

namespace avakar {

struct _rope_string_node final
	: _rope_memory_node
{
	_rope_string_node(std::string && str)
		: _str(std::move(str))
	{
	}

	std::string_view view() const override
	{
		return _str;
	}

private:
	std::string _str;
};

struct _rope_vector_node final
	: _rope_memory_node
{
	_rope_vector_node(std::vector<char> && vec)
		: _vec(std::move(vec))
	{
	}

	std::string_view view() const override
	{
		return { _vec.data(), _vec.size() };
	}

private:
	std::vector<char> _vec;
};

}

#endif // AVAKAR_BLOB_STORAGE_H
