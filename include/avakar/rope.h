#ifndef AVAKAR_ROPE_H
#define AVAKAR_ROPE_H

#include "blob.h"
#include "../../src/node.h"

namespace avakar {

struct rope
{
	rope();
	rope(rope const & o);
	rope(rope && o);
	rope(blob && o);

	template <typename Blob,
		std::enable_if_t<std::is_convertible<Blob &&, blob>::value, int> = 1>
	rope(Blob && b)
		: rope(blob(std::forward<Blob>(b)))
	{
	}

	~rope();

	rope & operator=(rope o);

	bool empty() const;
	uint64_t length() const;

	rope operator()(uint64_t offs, uint64_t len = (uint64_t)-1) const;

	std::string str() const;

	size_t read(char * buf, uint64_t offs, size_t len) const;
	std::string read(uint64_t offs, size_t len) const;

	friend rope operator|(rope lhs, rope rhs);

	rope & operator|=(rope rhs);

private:
	_rope_node * _root;
	uint64_t _offset;
	uint64_t _length;
};

}

#endif // AVAKAR_ROPE_H
