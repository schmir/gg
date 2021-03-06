#ifndef GGLIB_H
#define GGLIB_H

#include <google/sparsetable>
#include <google/sparse_hash_map>
#include <google/sparse_hash_set>

#include <algorithm>
#include <vector>
#include <map>

#include <ext/hash_map>
#include <ext/hash_set>
#include <iostream>


namespace gg
{

	typedef int ggint;

	typedef std::pair<ggint,ggint> link;

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

	class link_set: public google::sparse_hash_set< link, link_hasher>
	{
	public:
		link_set() {
			set_deleted_key(link(0x7fffffff,0x7fffffff));
		}
		ggint maxstartnode() const;
	};


	typedef google::sparse_hash_set<ggint> int_set;

	class vector_link_push_back {
	public:
		std::vector<link> *v;
		vector_link_push_back(std::vector<link> &_v) : v(&_v) {}
		void operator() (link t) {
			v->push_back(t);
		}
	};

	class vector_link_push_back_reversed {
	public:
		std::vector<link> *v;
		vector_link_push_back_reversed(std::vector<link> &_v) : v(&_v) {}
		void operator() (link t) {
			v->push_back(link(t.second, t.first));
		}
	};

	class vector_bool_set {
	public:
		std::vector<bool> *v;
		vector_bool_set(std::vector<bool> &_v) : v(&_v) {}
		void operator() (link t) {
			if (t.second < v->size() && t.second >= 0) {
				(*v)[t.second] = true;
			}
		}
	};

	class allowed_vector_bool_set {
	public:
		std::vector<bool> *v;
		std::vector<bool> *allowed;
		allowed_vector_bool_set(std::vector<bool> &_v, std::vector<bool> &_allowed) : v(&_v), allowed(&_allowed) {}
		void operator() (link t) {
			auto idx = t.second;
			if (idx >= 0 && idx<allowed->size() && idx<v->size() && (*allowed)[idx]) {
				(*v)[t.second] = true;
			}
		}
	};


	class vector_int_push_back {
	public:
		std::vector<ggint> *v;
		vector_int_push_back(std::vector<ggint> &_v) : v(&_v) {}
		void operator() (link t) {
			v->push_back(t.second);
		}
	};
	

	class sgraph {
	public:
		std::vector<interval> partitions;
		unsigned int maxlinks_per_partition;

		sgraph() {
			maxlinks_per_partition = 64;
			interval i;
			i.min = 0;
			i.max = 0x7fffffff;
			i.links = new link_set();
			partitions.push_back(i);
		}

		class siterator {
		public:
			siterator(std::vector<interval>::iterator begin,
				  std::vector<interval>::iterator end) {
				partitions_it = begin;
				partitions_end = end;
				_find_next();
			}

			siterator & operator++() {
				assert (partitions_it != partitions_end);
				assert (linkset_it != linkset_end);

				++linkset_it;
				if (linkset_it == linkset_end) {
					++partitions_it;
					_find_next();
				}
			}

			bool operator==(const siterator &other) const {
				return other.partitions_it == partitions_it;
			}

			bool operator!=(const siterator &other) const {
				return other.partitions_it != partitions_it;
			}

			link operator*() const {
				assert (partitions_it != partitions_end);
				assert (linkset_it != linkset_end);
				return *linkset_it;
			}

		protected:

			void _find_next() {
				while (partitions_it != partitions_end) {
					linkset_it = partitions_it->links->begin();
					linkset_end = partitions_it->links->end();
					if (linkset_it != linkset_end) {
						break;
					}
					++partitions_it;
				}
			}

			std::vector<interval>::iterator partitions_it, partitions_end;
			link_set::iterator linkset_it, linkset_end;
		};

		typedef siterator iterator;

		siterator begin() {
			return siterator(partitions.begin(), partitions.end());
		}

		siterator end() {
			return siterator(partitions.end(), partitions.end());
		}

		void dump() const;
		void dump_partition(unsigned int num) const;
		void split_partition(unsigned int num);
		ggint maxstartnode() const;

		int find_partition(int s) const {
			return lower_bound(partitions.begin(), partitions.end(), s, intervallt())-partitions.begin();
		}

		void add_link(link t) {
			interval &i = *lower_bound(partitions.begin(), partitions.end(), t.first, intervallt());
			i.links->insert(t);
			if (i.links->size()>maxlinks_per_partition && i.max!=i.min+1) {
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

		void get_reachable_from(std::vector<ggint> &nodes) const {
			__gnu_cxx::hash_set<int> done;

			for (auto it = nodes.begin(); it!=nodes.end(); ++it) {
				done.insert(*it);
			}

			std::vector<ggint> children;

			for (unsigned int i=0; i<nodes.size(); ++i) {
				int n = nodes[i];
				get_links_from(n, children);
				auto end = children.end();
				for (auto c = children.begin(); c != end; ++c) {
					if (done.find(*c) == done.end()) {
						nodes.push_back(*c);
						done.insert(*c);
					}
				}
				children.clear();
			}
		}

		template<class callback>
		void get_links_from(std::vector<ggint> &nodes, callback cb) const {
			for (auto it=nodes.begin(); it!=nodes.end(); ++it) {
				get_links_from(*it, cb);
			}
		}

		template<class callback>
		void get_links_from(ggint s, callback cb) const {
			interval i = *lower_bound(partitions.begin(), partitions.end(), s, intervallt());
			link_set::iterator end = i.links->end();
			for (link_set::iterator it (i.links->begin()); it!=end; ++it) {
				if (it->first==s) {
					cb(*it);
				}
			}
		}


		void get_links_from(std::vector<ggint> &nodes, std::vector<bool> &v, std::vector<bool> &allowed) const {
			get_links_from(nodes, allowed_vector_bool_set(v, allowed));
		}

		void get_links_from(std::vector<ggint> &nodes, std::vector<bool> &v) const {
			get_links_from(nodes, vector_bool_set(v));
		}

		void get_links_from(ggint s, std::vector<ggint> &v) const {
			get_links_from(s, vector_int_push_back(v));
		}

		void get_links_from(std::vector<ggint> &nodes, std::vector<ggint> &v) const {
			get_links_from(nodes, vector_int_push_back(v));
		}

		void remove_links_from(ggint s);
		unsigned int size() const;
	};


	class dgraph {
	public:
		typedef sgraph::iterator iterator;
		iterator begin() {
			return forward.begin();
		}

		iterator end() {
			return forward.end();
		}

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
		void get_links_from(ggint s, callback cb) const {
			forward.get_links_from(s, cb);
		}

		void get_links_from(ggint s, std::vector<ggint> &result) const {
			forward.get_links_from(s, result);
		}

		void get_links_to(ggint e, std::vector<ggint> &result) const {
			backward.get_links_from(e, result);
		}

		void get_reachable_from(std::vector<ggint> &nodes) const {
			return forward.get_reachable_from(nodes);
		}

		void get_links_from(std::vector<ggint> &nodes, std::vector<bool> &res) const {
			ggint m = 1 + maxendnode();
			if (res.size() < m) {
				res.resize(m);
			}

			return forward.get_links_from(nodes, res);
		}

		void get_links_from(std::vector<ggint> &nodes, std::vector<bool> &res, std::vector<bool> &allowed) const {
			ggint m = 1 + maxendnode();
			if (res.size() < m) {
				res.resize(m);
			}

			return forward.get_links_from(nodes, res, allowed);
		}

		template<class callback>
		void get_links_to(ggint e, callback cb) const {
			backward.get_links_from(e, cb);
		}

		void remove_links(std::vector<link> &links);
		void remove_links_to(ggint s);
		void remove_links_from(ggint s);
		void dump() const {
			forward.dump();
		}
		unsigned int size() const {
			return forward.size();
		}
		ggint maxstartnode() const {
			return forward.maxstartnode();
		}
		ggint maxendnode() const {
			return backward.maxstartnode();
		}
	};
	typedef dgraph::iterator dgraph_iterator;
}
#endif
