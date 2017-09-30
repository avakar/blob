#include <avakar/blob.h>
#include <algorithm>
using namespace avakar;

#include <assert.h>

void _in_memory::read(char * buf, uint64_t offs, size_t len)
{
	auto sv = this->get();

	assert(offs <= sv.size());
	assert(sv.size() - offs >= len);
	std::copy(sv.data() + offs, sv.data() + offs + len, buf);
}

uint64_t _in_memory::length() const
{
	return this->get().size();
}

struct _string_storage final
	: _in_memory
{
	void destroy() override
	{
		delete this;
	}

	std::string_view get() const override
	{
		return str_;
	}

	std::string str_;
};

struct _vector_storage final
	: _in_memory
{
	void destroy() override
	{
		delete this;
	}

	std::string_view get() const override
	{
		return { v_.data(), v_.size() };
	}

	std::vector<char> v_;
};

struct _cat_rope final
	: _ref_counter
{
	void destroy() override
	{
		delete this;
	}

	void read(char * buf, uint64_t offs, size_t len) override
	{
		auto lhs_len = lhs.length();
		if (offs + len <= lhs_len)
		{
			lhs.read(buf, offs, len);
		}
		else if (offs >= lhs_len)
		{
			rhs.read(buf, offs - lhs_len, len);
		}
		else
		{
			size_t chunk = (size_t)(lhs_len - offs);
			lhs.read(buf, offs, chunk);
			rhs.read(buf + chunk, 0, len - chunk);
		}
	}

	uint64_t length() const override
	{
		return lhs.length() + rhs.length();
	}

	rope lhs;
	rope rhs;
};

blob::blob()
	: rc_(nullptr), sv_()
{
}

blob::blob(char const * str)
	: rc_(nullptr), sv_(str)
{
}

blob::blob(char const * str, size_t len)
	: rc_(nullptr), sv_(str, str + len)
{
}

blob::blob(char const * first, char const * last)
	: rc_(nullptr), sv_(first, last)
{
}

blob::blob(blob const & o)
	: rc_(o.rc_), sv_(o.sv_)
{
	if (rc_)
		++rc_->count_;
}

blob::blob(blob && o)
	: rc_(o.rc_)
{
	o.rc_ = nullptr;
}

blob::blob(std::string const & str)
	: rc_(nullptr), sv_(str)
{
}

blob::blob(std::string && str)
{
	auto * p = new _string_storage();
	p->str_ = std::move(str);

	rc_ = p;
	sv_ = p->get();
}

blob::blob(std::vector<char> const & v)
	: rc_(nullptr), sv_(v.data(), v.size())
{
}

blob::blob(std::vector<char> && v)
{
	auto * p = new _vector_storage();
	p->v_ = std::move(v);

	rc_ = p;
	sv_ = p->get();
}

blob::blob(std::string_view str)
	: rc_(nullptr), sv_(str)
{
}

blob::~blob()
{
	if (rc_ && --rc_->count_ == 0)
		rc_->destroy();
}

blob & blob::operator=(blob o)
{
	rc_ = o.rc_;
	sv_ = o.sv_;

	o.rc_ = nullptr;
	return *this;
}

bool blob::empty() const
{
	return sv_.empty();
}

std::string_view blob::str() const
{
	return sv_;
}

blob::operator std::string_view() const
{
	return sv_;
}

blob blob::operator()(size_t offs) const
{
	blob r = *this;
	r.sv_ = r.sv_.substr(offs);
	return r;
}

blob blob::operator()(size_t offs, size_t len) const
{
	blob r = *this;
	r.sv_ = r.sv_.substr(offs, len);
	return r;
}

bool avakar::operator==(blob const & lhs, blob const & rhs)
{
	return lhs.sv_ == rhs.sv_;
}

bool avakar::operator!=(blob const & lhs, blob const & rhs)
{
	return !(lhs == rhs);
}

bool avakar::operator<(blob const & lhs, blob const & rhs)
{
	return lhs.sv_ < rhs.sv_;
}

bool avakar::operator<=(blob const & lhs, blob const & rhs)
{
	return !(rhs.sv_ < lhs.sv_);
}

bool avakar::operator>(blob const & lhs, blob const & rhs)
{
	return rhs.sv_ < lhs.sv_;
}

bool avakar::operator>=(blob const & lhs, blob const & rhs)
{
	return !(lhs.sv_ < rhs.sv_);
}

blob avakar::operator+(blob const & lhs, blob const & rhs)
{
	std::string r;
	r.reserve(lhs.sv_.size() + rhs.sv_.size());
	r.assign(lhs.sv_.data(), lhs.sv_.size());
	r.append(rhs.sv_.data(), rhs.sv_.size());
	return blob(std::move(r));
}

rope::rope()
	: rc_(nullptr), sv_{ nullptr, nullptr }
{
}

rope::rope(rope const & o)
	: rc_(o.rc_)
{
	if (rc_)
	{
		++rc_->count_;
		slice_ = o.slice_;
	}
	else
	{
		sv_ = o.sv_;
	}
}

rope::rope(rope && o)
	: rc_(o.rc_)
{
	if (rc_)
	{
		o.rc_ = nullptr;
		slice_ = o.slice_;
	}
	else
	{
		sv_ = o.sv_;
	}
}

rope::rope(blob b)
	: rc_(b.rc_)
{
	if (rc_)
	{
		slice_.first = b.sv_.data() - b.rc_->get().data();
		slice_.last = slice_.first + b.sv_.size();
		b.rc_ = nullptr;
	}
	else
	{
		sv_.first = b.sv_.data();
		sv_.last = sv_.first + b.sv_.size();
	}
}

rope::~rope()
{
	if (rc_ && --rc_->count_ == 0)
		rc_->destroy();
}

rope & rope::operator=(rope o)
{
	if (rc_ && --rc_->count_ == 0)
		rc_->destroy();
	rc_ = o.rc_;
	o.rc_ = nullptr;

	if (rc_)
		slice_ = o.slice_;
	else
		sv_ = o.sv_;

	return *this;
}

bool rope::empty() const
{
	return rc_? slice_.first == slice_.last: sv_.first == sv_.last;
}

rope rope::operator()(size_t offs) const
{
	rope r = *this;
	if (rc_)
		r.slice_.first += offs;
	else
		r.sv_.first += offs;
	return r;
}

rope rope::operator()(size_t offs, size_t len) const
{
	rope r = *this;
	if (rc_)
	{
		r.slice_.first += offs;
		r.slice_.last = r.slice_.first + len;
	}
	else
	{
		r.sv_.first += offs;
		r.sv_.last = r.sv_.first + len;
	}
	return r;
}

std::string rope::str() const
{
	uint64_t len = this->length();
	if (len > (size_t)-1)
		throw std::length_error("rope too long");

	return this->read(0, (size_t)len);
}

uint64_t rope::length() const
{
	return rc_? slice_.last - slice_.first: sv_.last - sv_.first;
}

void rope::read(char * buf, uint64_t offs, size_t len) const
{
	if (rc_)
		rc_->read(buf, offs, len);
	else
		std::copy(sv_.first + offs, sv_.first + offs + len, buf);
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
	auto * p = new _cat_rope();
	p->lhs = std::move(lhs);
	p->rhs = std::move(rhs);

	rope r;
	r.rc_ = p;
	r.slice_.first = 0;
	r.slice_.last = p->lhs.length() + p->rhs.length();
	return r;
}
