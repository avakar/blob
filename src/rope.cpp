#include <avakar/rope.h>
using namespace avakar;

struct _rope_cat_node final
	: _rope_node
{
	size_t read(char * buf, uint64_t offs, size_t len) override
	{
		if (len == 0)
			return 0;

		size_t total_read = 0;
		for (rope & ch: children)
		{
			if (offs >= ch.length())
			{
				offs -= ch.length();
				continue;
			}

			size_t r = ch.read(buf + total_read, offs, len - total_read);
			total_read += r;

			if (total_read == len)
				break;
			offs = 0;
		}

		return total_read;
	}

	rope children[3];
};

rope::rope()
	: _root(nullptr), _offset(0), _length(0)
{
}

rope::rope(rope const & o)
	: _root(o._root), _offset(o._offset), _length(o._length)
{
	if (_root)
		_root->addref();
}

rope::rope(rope && o)
	: _root(o._root), _offset(o._offset), _length(o._length)
{
	o._root = nullptr;
}

/*rope::rope(blob && b)
	: _root(b._root), _length(b.size())
{
	if (_root)
		_offset = b._sv.data() - b._root->view().data();
	else
		_offset = reinterpret_cast<uint64_t>(b._sv.data());
}*/

rope::~rope()
{
	if (_root)
		_root->release();
}

rope & rope::operator=(rope o)
{
	std::swap(_root, o._root);
	_offset = o._offset;
	_length = o._length;
	return *this;
}

bool rope::empty() const
{
	return _length != 0;
}

uint64_t rope::length() const
{
	return _length;
}

rope rope::operator()(uint64_t offs, uint64_t len) const
{
	if (offs >= _length)
		return rope();

	uint64_t rem = _length - offs;
	if (rem < len)
		len = rem;

	if (len == 0)
		return rope();

	rope r = *this;
	r._length = len;
	if (r._root)
		r._offset += offs;
	else
		r._offset = reinterpret_cast<uint64_t>(reinterpret_cast<char const *>(r._offset) + offs);
	return r;
}

std::string rope::str() const
{
	uint64_t len = this->length();
	if (len > (size_t)-1)
		throw std::length_error("rope too long");

	return this->read(0, (size_t)len);
}

size_t rope::read(char * buf, uint64_t offs, size_t len) const
{
	if (_root)
		return _root->read(buf, offs, len);

	rope r = (*this)(offs);
	if (len > r._length)
		len = static_cast<size_t>(r._length);

	auto * data = reinterpret_cast<char const *>(r._offset);
	std::copy(data, data + len, buf);
	return len;
}

std::string rope::read(uint64_t offs, size_t len) const
{
	std::string r;
	if (len != 0)
	{
		r.resize(len);
		this->read(&r[0], offs, len);
	}
	return r;
}

rope avakar::operator|(rope lhs, rope rhs)
{
	auto llen = lhs.length();
	auto rlen = rhs.length();

	if (llen == 0 && rlen == 0)
		return rope();

	if (llen == 0)
		return std::move(rhs);

	if (rlen == 0)
		return std::move(lhs);

	auto * p = new _rope_cat_node();
	p->children[0] = std::move(lhs);
	p->children[1] = std::move(rhs);

	rope r;
	r._root = p;
	r._offset = 0;
	r._length = llen + rlen;
	return r;
}

rope & rope::operator|=(rope rhs)
{
	*this = std::move(*this) | std::move(rhs);
	return *this;
}
