#include <iostream>
#include <random>
#include <vector>
#include <list>
#include <cmath>

#include <boost/geometry/index/rtree.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>

namespace bg = boost::geometry;
namespace bmpl = boost::mpl;
namespace bgi = bg::index;

// Metafunction to create a D dimensional point from a pointer
template <size_t D>
struct point_setter {
    typedef bg::model::point<double, D, bg::cs::cartesian> point_t;

    point_t& point;
    double *loc;

    point_setter(point_t& point, double *loc) : point(point), loc(loc)
    {}

    template< typename U > void operator()(U i)
    {
        bg::set<i>(point, loc[i]);
    }

};

// Metafunction to calculate the square of the euclidian distance between
// two D dimensional points.
template <size_t D>
struct d2_calc {
    typedef bg::model::point<double, D, bg::cs::cartesian> point_t;
    typedef std::pair<point_t, size_t> value_t;

    const point_t &p1;
    const point_t &p2;
    double &d2;

    d2_calc(const point_t &p1, const point_t &p2, double &d2) : p1(p1), p2(p2), d2(d2)
    {}

    template< typename U > void operator()(U i)
    {
        d2 += pow( bg::get<i>(p1) - bg::get<i>(p2), 2);
    }
};


template <size_t D>
std::list< std::list<size_t> > friends_of_friends_rtree(double *data, size_t npts, double linking_length)
{
    std::cerr << "Running with " << D << " dimensions" << std::endl;

    typedef bg::model::point<double, D, bg::cs::cartesian> point_t;
    typedef std::pair<point_t, size_t> value_t;
    typedef bgi::rtree< value_t, bgi::rstar<16> > tree_t;

    std::vector< std::pair<point_t, size_t> > points;
    points.reserve(npts);

    typedef bmpl::range_c<size_t, 0, D> dim_range;
    for(size_t i = 0 ; i<npts ; ++i) {
        point_t point;
        bmpl::for_each< dim_range >( point_setter<D>(point, data + i*D) );
        points.push_back(std::make_pair(point, i));
    }

    tree_t tree(points.begin(), points.end());

    std::list< std::list< size_t > > groups;

    while( !tree.empty() ) {
        std::list< value_t > to_add;
        tree.query( bgi::nearest(points[0].first, 1), std::back_inserter(to_add) );
        tree.remove( to_add.begin(), to_add.end() );

        for( auto it = to_add.begin() ; it != to_add.end() ; ++it ) {
            std::list< value_t >  added;
            // tree.query( "query for all points in a ball around it", std::back_inserter(added) );
            tree.remove( added.begin(), added.end() );
            to_add.splice(to_add.end(), added);
        }

        std::list< size_t > group;
        for( auto p : to_add ) {
            group.push_back(p.second);
        }
        groups.push_back(group);
    }


    // Psudo code for the algorithm
    // 
    // 1) Pick a random point from the tree to begin a group/
    // 2) Query tree to find neighbouring points
    // 3) For each added point do the same.
    // 4) When no more points to add finish.
    // 5) Repeat until the tree is empty.

    // std::list< value_t > result;


    // point_t center(-1.0, -1.0);
    // bg::model::box< point_t > box( point_t(-1.3, -1.3), point_t(-0.7, -0.7) );

    // auto within_ball = [&center, linking_length](value_t const& v) {
    //     double d2 = 0.;
    //     bmpl::for_each< dim_range >( d2_calc<D>(center, v.first, d2) );
    //     return sqrt(d2) < linking_length;
    // };

    // tree.query( bgi::within(box) && bgi::satisfies(within_ball) , std::back_inserter(result) );
    // tree.remove( result.begin(), result.end() );

    // std::list< value_t > rest;
    // bg::model::box< point_t > box2( point_t(-5, -5), point_t(5, 5) );
    // tree.query( bgi::within(box2), std::back_inserter(rest));

    // for(auto r : rest) {
    //     double x = bg::get<0>(r.first);
    //     double y = bg::get<1>(r.first);
    //     std::cout << x << " " << y << std::endl;
    // }


    return groups;
}

inline double dist(double *p1, double *p2, size_t ndim)
{
    double d2 = 0.;
    for(size_t i = 0 ; i < ndim ; ++i) {
        d2 += pow(p1[i] - p2[i], 2);
    }
    return sqrt(d2);
}

std::list< std::list<size_t> > friends_of_friends_brute(double *data, size_t npts, size_t ndim, double linking_length)
{
    std::cerr << "Using non tree accelerated version" << std::endl;
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

    return groups;
}

std::list< std::list<size_t> > friends_of_friends(double *data, size_t npts, size_t ndim, double linking_length)
{
    switch(ndim) {
        // case 1:
        //     return friends_of_friends_rtree<1>(data, npts, linking_length);
        //     break;
        case 2:
            return friends_of_friends_rtree<2>(data, npts, linking_length);
            break;
        // case 3:
        //     return friends_of_friends_rtree<3>(data, npts, linking_length);
        //     break;
        default:
            return friends_of_friends_brute(data, npts, ndim, linking_length);
            break;
    }
}


int main(int argc, char const *argv[])
{
    const size_t ndim = 2;

    std::mt19937 generator;
    std::normal_distribution<double> dist1(1.0, 0.1);
    std::normal_distribution<double> dist2(-1.0, 0.1);

    std::vector< double > coords;

    for(size_t i=0 ; i<50000 ; ++i) {
        for( size_t j=0 ; j<ndim ; ++j ) {
            coords.push_back( dist1(generator) );
        }
    }
    for(size_t i=0 ; i<50000 ; ++i) {
        for( size_t j=0 ; j<ndim ; ++j ) {
            coords.push_back( dist2(generator) );
        }
    }

    auto groups = friends_of_friends(coords.data(), coords.size() / ndim, ndim, 0.3);

    std::cerr << "Ngroups = " << groups.size() << std::endl;



    return 0;
}