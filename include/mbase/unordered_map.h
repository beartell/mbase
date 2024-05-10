#ifndef MBASE_UMAP_H
#define MBASE_UMAP_H

#include <mbase/common.h>
#include <mbase/allocator.h>
#include <mbase/vector.h>
#include <mbase/list.h>
#include <utility>

MBASE_STD_BEGIN

template<typename Key, typename Value, typename Allocator = mbase::allocator_simple<Value>>
class unordered_map {
public:
	unordered_map() noexcept {}


private:
	mbase::vector<Key> _Keys;
	mbase::vector<mbase::list<Value>> _Bucket;
};

MBASE_STD_END

#endif // MBASE_UMAP_H