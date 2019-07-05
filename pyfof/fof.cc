#include <iostream>
#include <vector>
#include <list>
#include <cmath>

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>

namespace bg = boost::geometry;
namespace bmpl = boost::mpl;
namespace bgi = bg::index;

// Create a D dimensional point from an array of coordinates
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

// Calculate the square of the euclidian distance between two points
template <size_t D>
struct d2_calc {
    typedef bg::model::point<double, D, bg::cs::cartesian> point_t;

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

// Add a scaler to all the coordinates of a point
template <size_t D>
struct add_scalar_to_point {
    typedef bg::model::point<double, D, bg::cs::cartesian> point_t;

    point_t &p;
    double c;

    add_scalar_to_point(point_t &p, double c) : p(p), c(c)
    {}

    template< typename U > void operator()(U i)
    {
        double new_coord = bg::get<i>(p) + c;
        bg::set<i>(p, new_coord);
    }

};

template <size_t D>
std::list< std::list<size_t> >
friends_of_friends_rtree(double *data, size_t npts, double linking_length)
{
    typedef bg::model::point<double, D, bg::cs::cartesian> point_t;
    typedef std::pair<point_t, size_t> value_t;
    using tree_t =  bgi::rtree< value_t, bgi::linear<16> >;
    typedef bmpl::range_c<size_t, 0, D> dim_range;

    std::vector< std::pair<point_t, size_t> > points;
    points.reserve(npts);

    for(size_t i = 0 ; i<npts ; ++i) {
        point_t point;
        bmpl::for_each< dim_range >( point_setter<D>(point, data + i*D) );
        points.push_back(std::make_pair(point, i));
    }

    tree_t tree(points.begin(), points.end());

    std::list< std::list< size_t > > groups;

    while( !tree.empty() ) {
        std::list< value_t > to_add;

        // Grab a point from the tree.
        to_add.push_back( *tree.qbegin( bgi::satisfies([](value_t const &){return true;})) );
        tree.remove( to_add.begin(), to_add.end() );

        for( auto it = to_add.begin() ; it != to_add.end() ; ++it ) {
            std::list< value_t >  added;

            // Build box to query
            point_t lower = it->first;
            bmpl::for_each< dim_range >( add_scalar_to_point<D>(lower, -linking_length) );
            point_t upper = it->first;
            bmpl::for_each< dim_range >( add_scalar_to_point<D>(upper, +linking_length));

            bg::model::box< point_t > box( lower, upper );

            auto within_ball = [&it, linking_length](value_t const &v) {
                double d2 = 0.;
                bmpl::for_each< dim_range >( d2_calc<D>(it->first, v.first, d2) );
                return sqrt(d2) < linking_length;
            };

            // Find all points within a linking length of the current point.
            tree.query( bgi::within(box) && bgi::satisfies(within_ball), std::back_inserter(added) );

            // Remove any points we find from the tree as they have been assigned.
            tree.remove( added.begin(), added.end() );

            // Add the found points to the list so we can find their "friends" as well
            to_add.splice(to_add.end(), added);
        }

        std::list< size_t > group;
        for( auto p : to_add ) {
            group.push_back(p.second);
        }
        groups.push_back(group);
    }

    return groups;
}

inline double
dist(double *p1, double *p2, size_t ndim)
{
    double d2 = 0.;
    for(size_t i = 0 ; i < ndim ; ++i) {
        d2 += pow(p1[i] - p2[i], 2);
    }
    return sqrt(d2);
}

// A brute force friends of friends finder without the Rtree accelerator.
std::list< std::list<size_t> >
friends_of_friends_brute(double *data, size_t npts, size_t ndim, double linking_length)
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

std::list< std::list<size_t> >
friends_of_friends(double *data, size_t npts, size_t ndim, double linking_length)
{
    switch(ndim) {
        case 1:
            return friends_of_friends_rtree<1>(data, npts, linking_length);
            break;
        case 2:
            return friends_of_friends_rtree<2>(data, npts, linking_length);
            break;
        case 3:
            return friends_of_friends_rtree<3>(data, npts, linking_length);
            break;
        case 4:
            return friends_of_friends_rtree<4>(data, npts, linking_length);
            break;
        default:
            return friends_of_friends_brute(data, npts, ndim, linking_length);
            break;
    }
}

