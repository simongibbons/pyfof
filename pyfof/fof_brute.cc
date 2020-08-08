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
    std::list< std::list< size_t > > groups;
    for(size_t i=0 ; i<npts ; ++i) {
        unused.push_back(std::make_pair(i, data + i*ndim));
    }

    //If there are unused points try to create a new group
    while( unused.size() > 0 ) {
        std::list<Point> toadd;
        toadd.push_back(unused.front());
        unused.pop_front();

        //Look through all points found in the group and attempt to 
        for(auto toadd_it = toadd.begin() ; toadd_it != toadd.end() ; ++toadd_it) {
            auto unused_it = unused.begin();
            while(unused_it != unused.end()) {
                if(dist(unused_it->second, toadd_it->second, ndim) < linking_length) {
                    toadd.push_back(*unused_it);
                    unused.erase(unused_it++);
                } else {
                    ++unused_it;
                }
            }
        }

        std::list<size_t> group;
        for(const auto& p : toadd) {
            group.push_back(p.first);
        }
        groups.push_back(group);
    }


    auto result = std::vector<std::vector<size_t>>();
    for (const auto& group : groups) {
        result.push_back(std::vector<size_t>(group.cbegin(), group.cend()));
    }
    return result;
}
