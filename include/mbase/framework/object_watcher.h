#ifndef MBASE_OBJECT_WATCHER_H
#define MBASE_OBJECT_WATCHER_H

#include <mbase/common.h>
#include <mbase/list.h>
#include <mbase/vector.h>

MBASE_BEGIN

template<typename TargetObject>
struct list_object_watcher {
    mbase::list<list_object_watcher<TargetObject>>::iterator mItSelf;
    mutable TargetObject* mSubject = NULL;
};

template<typename TargetObject>
struct vector_object_watcher {
    mbase::list<vector_object_watcher<TargetObject>>::iterator mItSelf;
    mutable TargetObject* mSubject = NULL;
};

MBASE_END

#endif // MBASE_OBJECT_WATCHER_H