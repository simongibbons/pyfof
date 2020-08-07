#pragma once
#include <cstdlib>
#include <list>
std::vector< std::vector<size_t> >  friends_of_friends(double* data, size_t npts, size_t ndim, double linking_length);

std::vector< std::vector<size_t> > friends_of_friends_brute(double* data, size_t npts, size_t ndim, double linking_length);

