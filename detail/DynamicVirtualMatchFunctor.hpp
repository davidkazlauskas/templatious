//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

#ifndef DYNAMICVIRTUALMATCHFUNCTOR_8CHO77L5
#define DYNAMICVIRTUALMATCHFUNCTOR_8CHO77L5

#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <algorithm>

#include <templatious/util/Exceptions.hpp>
#include <templatious/detail/VirtualMatchFunctor.hpp>

namespace templatious {

TEMPLATIOUS_BOILERPLATE_EXCEPTION(
    DynamicVMatchFunctorNoIdToRemoveException,
    "Couldn't find the specified id to remove." );

/**
 * Dynamic virtual match functor. This class can attach
 * VirtualMatchFunctors for processing with default
 * or custom priority. Threadsafe to attach/detach
 * and match virtual packs.
 */
struct DynamicVMatchFunctor : public VirtualMatchFunctor {
    DynamicVMatchFunctor(bool isBroadcast = false) :
        _isBroadcast(isBroadcast), _idCount(0) {}

    DynamicVMatchFunctor(const DynamicVMatchFunctor&) = delete;

    DynamicVMatchFunctor(DynamicVMatchFunctor&& d) :
        _idCount(d._idCount)
    {
        d.swapTo(*this);
    }

    /**
     * Attach virtual match functor to the processing queue.
     * Returns the id of attached functor to possibly refer
     * to it in the future. Threadsafe.
     * @param[in] vmf unique pointer to a VirtualMatchFunctor
     * to attach.
     * @param[in] priority Priority to set for processing.
     * Match functors with higher priority are processed earlier
     * than the ones with lower priority. Defaults to 128.
     */
    int attach(std::unique_ptr<VirtualMatchFunctor> vmf,int priority = 128) {
        LockGuard l(_mtx);

        int offset = _fctors.size();
        int id = _idCount++;
        _fctors.push_back(std::move(vmf));
        _queue.push_back(QueueUnit(id,offset,priority));
        std::sort( _queue.begin(), _queue.end(),
            [](const QueueUnit& a,const QueueUnit& b) {
                return a._priority > b._priority;
            });

        return id;
    }

    /**
     * Detach virtual match functor with the specified id
     * and return it to the caller wrapped around
     * std::unique_ptr. Throws if no such id exists. Threadsafe.
     * @param[in] id The id of a functor to return.
     */
    std::unique_ptr<VirtualMatchFunctor> detach(int id) {
        LockGuard l(_mtx);

        // it breaks my heart to write boilerplates
        // now, but, I'd rather this be standalone
        // class than be dependant on the entire
        // ecosystem of templatious
        auto end = _queue.end();
        auto i = _queue.begin();
        for (; i != end; ++i) {
            if (i->_id == id) {
                break;
            }
        }

        if (i == end) {
            throw DynamicVMatchFunctorNoIdToRemoveException();
        }

        QueueUnit qu = *i;
        _queue.erase(i);

        i = _queue.begin();
        end = _queue.end();
        // adjust for removal
        for (; i != end; ++i) {
            if (i->_offset > qu._offset) {
                --i->_offset;
            }
        }

        auto fIter = _fctors.begin() + qu._offset;
        ValueType result = std::move(*fIter);
        _fctors.erase(fIter);
        return std::move(result);
    }

    /**
     * Clear all the existing VMatch functors. Threadsafe.
     */
    void clear() {
        LockGuard l(_mtx);
        _fctors.clear();
        _queue.clear();
    }

    /**
     * Try to match virtual pack to this match functor.
     * Only one pack can be processed at a time by
     * multiple threads.
     * @param[in] vp VirtualPack to match.
     */
    bool tryMatch(VirtualPack& vp) override {
        LockGuard l(_mtx);

        auto end = _queue.end();
        for (auto i = _queue.begin(); i != end; ++i) {
            auto& curr = _fctors[i->_offset];
            if (curr->tryMatch(vp)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Try to match virtual pack to this match functor.
     * Only one pack can be processed at a time by
     * multiple threads.
     * @param[in] vp VirtualPack to match.
     */
    bool tryMatch(VirtualPack& vp) const override {
        LockGuard l(_mtx);

        auto end = _queue.end();
        for (auto i = _queue.begin(); i != end; ++i) {
            auto& curr = _fctors[i->_offset];
            if (curr->tryMatch(vp)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Try to match virtual pack to this match functor.
     * Only one pack can be processed at a time by
     * multiple threads.
     * @param[in] vp VirtualPack to match.
     */
    bool tryMatch(const VirtualPack& vp) override {
        LockGuard l(_mtx);

        auto end = _queue.end();
        for (auto i = _queue.begin(); i != end; ++i) {
            auto& curr = _fctors[i->_offset];
            if (curr->tryMatch(vp)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Try to match virtual pack to this match functor.
     * Only one pack can be processed at a time by
     * multiple threads.
     * @param[in] vp VirtualPack to match.
     */
    bool tryMatch(const VirtualPack& vp) const override {
        LockGuard l(_mtx);

        auto end = _queue.cend();
        for (auto i = _queue.cbegin(); i != end; ++i) {
            auto& curr = _fctors[i->_offset];
            if (curr->tryMatch(vp)) {
                return true;
            }
        }
        return false;
    }
private:
    struct QueueUnit {
        QueueUnit(int id,int offset,int priority) :
            _id(id), _offset(offset), _priority(priority) {}

        int _id;
        int _offset;
        int _priority;
    };

    void swapTo(DynamicVMatchFunctor& other) {
        LockGuard l(_mtx);
        other._fctors = std::move(_fctors);
        other._queue = std::move(_queue);
    }

    typedef std::unique_ptr< VirtualMatchFunctor > ValueType;
    typedef std::lock_guard< std::mutex > LockGuard;
    mutable std::mutex _mtx;
    bool _isBroadcast;
    int _idCount;
    std::vector< ValueType > _fctors;
    std::vector< QueueUnit > _queue;
};

}

#endif /* end of include guard: DYNAMICVIRTUALMATCHFUNCTOR_8CHO77L5 */
