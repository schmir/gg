#ifndef GGLIB_H
#define GGLIB_H

#include <google/sparsetable>
#include <google/sparse_hash_map>
#include <google/sparse_hash_set>

#include <algorithm>
#include <vector>
#include <map>

#include <ext/hash_map>
#include <iostream>

using namespace google;
using namespace std;

namespace __gnu_cxx{};  // only in gcc 3.x
using namespace __gnu_cxx;

namespace gg
{

	typedef int ggint;

	typedef pair<ggint,ggint> link;

	struct cdbhasher {
		size_t operator()(const link &p) const {
			unsigned char *c = (unsigned char *)&p;
			size_t h = 5381;
			for (unsigned int i=0; i<sizeof(link); ++i) {
				h += (h << 5);
				h ^= c[i];
			}
			return h;
		}
	};


	struct link_hasher {
		size_t operator()(const link &p) const {
			return (p.first<<4)^p.second;
		}
	};

	class link_set;
	struct interval {
		ggint min;
		ggint max;   // max itself is excluded
		link_set *links;
	};

	struct intervallt {
		bool operator()(const interval &i,ggint s) {
			return i.max <= s;
		}
	};

	class link_set: public sparse_hash_set< link, link_hasher>
	{
	public:
		link_set() {
			set_deleted_key(link(0x7fffffff,0x7fffffff));
		}
	};


	typedef sparse_hash_set<ggint> int_set;

	class vector_link_push_back {
	public:
		vector<link> *v;
		vector_link_push_back(vector<link> &_v) : v(&_v) {}
		void operator() (link t) {
			v->push_back(t);
		}
	};

	class vector_link_push_back_reversed {
	public:
		vector<link> *v;
		vector_link_push_back_reversed(vector<link> &_v) : v(&_v) {}
		void operator() (link t) {
			v->push_back(link(t.second, t.first));
		}
	};

	class vector_int_push_back {
	public:
		vector<ggint> *v;
		vector_int_push_back(vector<ggint> &_v) : v(&_v) {}
		void operator() (link t) {
			v->push_back(t.second);
		}
	};
	

	class sgraph {
	public:
		vector<interval> partitions;

		sgraph() {
			interval i;
			i.min = 0;
			i.max = 0x7fffffff;
			i.links = new link_set();
			partitions.push_back(i);
		}

		void dump();
		void dump_partition(unsigned int num);
		void split_partition(unsigned int num);

		int find_partition(int s) {
			return lower_bound(partitions.begin(), partitions.end(), s, intervallt())-partitions.begin();
		}

		void add_link(link t) {
			interval &i = *lower_bound(partitions.begin(), partitions.end(), t.first, intervallt());
			i.links->insert(t);
			if (i.links->size()>512 && i.max!=i.min+1) {
				split_partition(&i-&*partitions.begin());
			}
		}

		void remove_link(link t) {
			interval &i = *lower_bound(partitions.begin(), partitions.end(), t.first, intervallt());
			i.links->erase(t);

		}

		void add_link(ggint s, ggint e) {
			add_link(link(s,e));
		}

		void remove_link(ggint s, ggint e) {
			remove_link(link(s,e));
		}

		template<class callback>
		void get_links_from(ggint s, callback cb) {
			interval i = *lower_bound(partitions.begin(), partitions.end(), s, intervallt());
			link_set::iterator end = i.links->end();
			for (link_set::iterator it (i.links->begin()); it!=end; ++it) {
				if (it->first==s) {
					cb(*it);
				}
			}
		}

		void get_links_from(ggint s, vector<ggint> &v) {
			get_links_from(s, vector_int_push_back(v));
		}
		
		void remove_links_from(ggint s);

	};


	class dgraph {
	public:
		sgraph forward, backward;
		void add_link(link t) {
			forward.add_link(t);
			backward.add_link(t.second, t.first);
		}
		void remove_link(link t) {
			forward.remove_link(t);
			backward.remove_link(t.second, t.first);
		}

		void add_link(ggint s, ggint e) {
			add_link(link(s,e));
		}

		void remove_link(ggint s, ggint e) {
			remove_link(link(s,e));
		}

		template<class callback>
		void get_links_from(ggint s, callback cb) {
			forward.get_links_from(s, cb);
		}

		void get_links_from(ggint s, vector<ggint> &result) {
			forward.get_links_from(s, result);
		}

		void get_links_to(ggint e, vector<ggint> &result) {
			backward.get_links_from(e, result);
		}

		template<class callback>
		void get_links_to(ggint e, callback cb) {
			backward.get_links_from(e, cb);
		}

		void remove_links(vector<link> &links);
		void remove_links_to(ggint s);
		void remove_links_from(ggint s);
		void dump() {
			forward.dump();
		}
	};
}
#endif
