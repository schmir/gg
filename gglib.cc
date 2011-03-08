#include "gglib.h"

namespace gg
{
	void sgraph::split_partition(unsigned int num) {
		interval i = partitions[num];
		if (i.max==i.min+1) {
			return;
		}

		link_set *ls = i.links;
		unsigned int total = ls->size();
		std::map<int, int> counts;

		link_set::iterator end = ls->end();
		for (link_set::iterator it(ls->begin());it!=end;++it) {
			counts[it->first]++;
		}

		unsigned int first_count = 0;
		int last=-1;
		if (counts.size()==1) {
			int here = ls->begin()->first;
			if (num>0) {
				partitions[num-1].max = here;
			} else if (here>0) {
				interval t;
				t.min = 0;
				t.max = here;
				t.links = new link_set();
				partitions.insert(partitions.begin(), t);
				num += 1;
			}

			if (num+1<partitions.size()) {
				partitions[num+1].min = here+1;
			} else {
				interval t;
				t.min = here+1;
				t.max = 0x7fffffff;
				t.links = new link_set();
				partitions.push_back(t);
			}

			partitions[num].min = here;
			partitions[num].max = here+1;
			return;
		}

		for (std::map<int, int>::iterator mi=counts.begin(); mi!=counts.end(); ++mi) {
			if (first_count+mi->second == total) {
				break;
			}

			first_count += mi->second;
			last = mi->first;

			if (first_count >= total/2) {
				break;
			}
		}

		link_set *first = new link_set();
		link_set *second = new link_set();

		interval s;
		s.min = last+1;
		s.max = partitions[num].max;
		s.links = second;

		for (link_set::iterator it(ls->begin());it!=end;++it) {
			if (it->first <= last) {
				first->insert(*it);
			} else {
				second->insert(*it);
			}
		}

		assert (first->size()>0);
		assert (second->size()>0);
		assert (first->size()+second->size()==total);

		partitions.insert(partitions.begin()+(num+1), s);

		delete ls;
		partitions[num].links = first;
		partitions[num].max = last+1;
	}

	void sgraph::dump() {
		for (unsigned int i=0;i<partitions.size();++i) {
			dump_partition(i);
		}
	}
	void sgraph::dump_partition(unsigned int num) {
		interval i = partitions[num];
		std::cerr << "Partition #" << num << " size=" << i.links->size() << " min/max " << i.min << " " << i.max << std::endl;
	}

	void dgraph::remove_links(std::vector<link> &links) {
		std::vector<link>::iterator end = links.end();
		for (std::vector<link>::iterator it=links.begin();
		     it!=end;
		     ++it) {
			forward.remove_link(*it);
			backward.remove_link(it->second, it->first);
		}
	}

	void dgraph::remove_links_from(ggint s) {
		std::vector<link> links;
		forward.get_links_from(s, vector_link_push_back(links));
		remove_links(links);
	}

	void dgraph::remove_links_to(ggint s) {
		std::vector<link> links;
		backward.get_links_from(s, vector_link_push_back_reversed(links));
		remove_links(links);
	}
}
