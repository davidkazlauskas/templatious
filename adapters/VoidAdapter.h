/*
 * =====================================================================================
 *
 *       Filename:  VoidAdapter.h
 *
 *    Description:  Void dummy adapter
 *
 *        Version:  1.0
 *        Created:  05/26/2014 08:00:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef VOIDADAPTER_JBAF2JRW
#define VOIDADAPTER_JBAF2JRW

#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace adapters {

template <>
struct CollectionAdapter< void > {

    void* instantiate() { return nullptr; }

    template <class Col,class Val>
	bool add(Col& c,const Val& i) {
        return false;
	}

	void* instantiate(int size) { return nullptr; }

    template <class Col>
	void* begin(Col& c) {
		return nullptr;
	}

    template <class Col>
	void* end(Col& c) {
		return nullptr;
	}

    template <class Col>
	const void* begin(const Col& c) {
		return nullptr;
	}

    template <class Col>
	const void* end(const Col& c) {
		return nullptr;
	}

    template <class Col>
	int getSize(const Col& c) {
		return -1;
	}

    template <class Col>
	void* getByIndex(Col& c,int i) { }

    template <class Col,class Iter>
    bool erase(Col& c,Iter beg) {
        return false;
    }

    template <class Col,class Iter>
    bool erase(Col& c,Iter beg,Iter end) {
        return false;
    }

    template <class Col>
    void* iter_at(Col& c,int i) {
        return nullptr;
    }
};

}
}

#endif /* end of include guard: VOIDADAPTER_JBAF2JRW */


