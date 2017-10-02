#ifndef AVAKAR_BLOB_NODE_H
#define AVAKAR_BLOB_NODE_H

#include <atomic>
#include <string_view>
#include <stdint.h>
#include <stdlib.h>

namespace avakar {

struct _rope_node
{
	virtual ~_rope_node();
	virtual size_t read(char * buf, uint64_t offs, size_t len) = 0;

	void addref();
	void release();

	size_t _height = 0;
	std::atomic<size_t> _ref_count = 1;
};

struct _rope_memory_node
	: _rope_node
{
	size_t read(char * buf, uint64_t offs, size_t len) final;
	virtual std::string_view view() const = 0;

};

} // namespace avakar

#endif // AVAKAR_BLOB_NODE_H
