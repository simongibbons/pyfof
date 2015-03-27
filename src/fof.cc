#include "fof.hpp"
#include <list>
#include <iostream>
#include <cmath>

struct Point {
    size_t index;
    double *loc;

    Point(size_t index, double *loc):index(index), loc(loc)
    {}
};

inline double dist(double *p1, double *p2, size_t ndim)
{
    double d2 = 0.;
    for(size_t i = 0 ; i < ndim ; ++i) {
        d2 += pow(p1[i] - p2[i], 2);
    }
    return sqrt(d2);
}

std::list< std::list<size_t> > friends_of_friends(double *data, size_t npts, size_t ndim, double linking_length)
{
    std::list<Point> unused;
    std::list< std::list< size_t > > groups;
    for(size_t i=0 ; i<npts ; ++i) {
        unused.push_back(Point(i, data + i*ndim));
    }

    while( unused.size() > 0 ) {
        std::list<Point> toadd;
        toadd.push_back(unused.front());
        unused.pop_front();

        for(auto toadd_it = toadd.begin() ; toadd_it != toadd.end() ; ++ toadd_it) {
            auto unused_it = unused.begin();
            while(unused_it != unused.end()) {
                if(dist(unused_it->loc, toadd_it->loc, ndim) < linking_length) {
                    toadd.push_back(*unused_it);
                    unused.erase(unused_it++);
                } else {
                    ++unused_it;
                }
            }
        }

        std::list<size_t> group;
        for(auto& p : toadd) {
            group.push_back(p.index);
        }
        groups.push_back(group);
    }

    return groups;
}