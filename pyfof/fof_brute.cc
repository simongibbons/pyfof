#include <vector>
#include <list>
#include <cmath>

namespace {

double dist(double *p1, double *p2, size_t ndim) {
    double d2 = 0.;
    for(size_t i = 0 ; i < ndim ; ++i) {
        d2 += pow(p1[i] - p2[i], 2);
    }
    return sqrt(d2);
}

} // anonymous namespace

// A brute force friends of friends finder without the Rtree accelerator.
std::vector< std::vector<size_t> > friends_of_friends_brute(
	double *data,
	size_t npts,
	size_t ndim,
	double linking_length
) {
  
	typedef std::pair<size_t, double*> Point;

    //Create list of unused points with indices
    std::list<Point> unused;
    std::vector< std::vector< size_t > > groups;
    for(size_t i=0 ; i<npts ; ++i) {
        unused.push_back(std::make_pair(i, data + i*ndim));
    }

    //If there are unused points try to create a new group
    while( unused.size() > 0 ) {
    	auto group = std::vector<size_t>();
    	auto toadd = std::vector<Point>();
        toadd.push_back(unused.front());
        unused.pop_front();

        while (!toadd.empty()) {
        	auto point = toadd.back();
        	toadd.pop_back();
        	group.push_back(point.first);

            auto unused_it = unused.begin();
            while(unused_it != unused.end()) {
                if(dist(unused_it->second, point.second, ndim) < linking_length) {
                    toadd.push_back(*unused_it);
                    unused.erase(unused_it++);
                }  else {
                	++unused_it;
            	}
            }
        }
        groups.push_back(group);
    }
    return groups;
}
