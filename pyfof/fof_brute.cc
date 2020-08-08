#include "fof_brute.hpp"

#include <algorithm>
#include <vector>
#include <cmath>

namespace {

typedef std::pair<size_t, double*> Point;

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
    auto result = std::vector< std::vector< size_t > >();

    //Create list of unused points with indices
    std::vector<Point> unused;
    for(size_t i=0 ; i<npts ; ++i) {
        unused.push_back(std::make_pair(i, data + i*ndim));
    }

    //If there are unused points try to create a new group
    while( !unused.empty() ) {
    	auto group = std::vector<size_t>();
    	auto toadd = std::vector<Point>();
        toadd.push_back(unused.back());
        unused.pop_back();

        while (!toadd.empty()) {
        	auto point = toadd.back();
        	toadd.pop_back();
        	group.push_back(point.first);

            for (auto& unused_point: unused) {
                if(dist(unused_point.second, point.second, ndim) < linking_length) {
                    toadd.push_back(unused_point);
                    unused_point.second = nullptr;  // Mark the unused point to be deleted
                }     
            }

            // Remove any deleted points
        	unused.erase(
                std::remove_if(unused.begin(), unused.end(), [](Point p){return p.second == nullptr;}),
                unused.end()
            );
        }
        std::sort(group.begin(), group.end());
        result.push_back(group);
    }
    return result;
}
