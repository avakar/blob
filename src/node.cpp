#include "node.h"
using namespace avakar;

_rope_node::~_rope_node()
{
}

void _rope_node::addref()
{
	++_ref_count;
}

void _rope_node::release()
{
	if (--_ref_count == 0)
		delete this;
}

size_t _rope_memory_node::read(char * buf, uint64_t offs, size_t len)
{
	std::string_view sv = this->view();
	if (offs >= sv.size())
		return 0;

	size_t rem = sv.size() - static_cast<size_t>(offs);
	if (len > rem)
		len = rem;

	std::copy(sv.data() + offs, sv.data() + offs + len, buf);
	return len;
}
