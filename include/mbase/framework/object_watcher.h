#ifndef MBASE_OBJECT_WATCHER_H
#define MBASE_OBJECT_WATCHER_H

#include <mbase/common.h>
#include <mbase/list.h>
#include <mbase/vector.h>

MBASE_BEGIN

// IteratorType is mbase::object_watcher

template<typename IteratorType, typename TargetObject>
struct object_watcher {
    IteratorType mItSelf;
    mutable TargetObject* mSubject = NULL;
};

// mbase::vector<object_watcher

template<typename TargetObject>
struct list_object_watcher {
    typename mbase::list<list_object_watcher<TargetObject>>::iterator mItSelf;
    mutable TargetObject* mSubject = NULL;
};

template<typename TargetObject>
struct vector_object_watcher {
    typename mbase::list<vector_object_watcher<TargetObject>>::iterator mItSelf;
    mutable TargetObject* mSubject = NULL;
};



MBASE_END

#endif // MBASE_OBJECT_WATCHER_H