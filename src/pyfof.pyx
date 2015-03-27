# -*- coding: utf-8 -*-
"""
Created on Thu Mar 26 17:59:28 2015

@author: sljg2
"""

cimport numpy as np
from libcpp.list cimport list

cdef extern from "fof.hpp":
    cdef list[list[size_t]] _friends_of_friends "friends_of_friends"(double*, size_t, size_t, double)

def friends_of_friends(np.ndarray[double, ndim=2, mode="c"] data, double linking_length):
    """ Computes friends-of-friends clustering of data. Distances are computed
    using a euclidian metric.

        :param data: A numpy array with dimensions (npoints x ndim)

        :param linking_length: The linking length between cluster members

        :rtype: A list of lists of indices in each cluster type
    """
    return _friends_of_friends(&data[0,0],\
                               data.shape[0],\
                               data.shape[1],\
                               linking_length)


