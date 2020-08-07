# -*- coding: utf-8 -*-
"""
Created on Thu Mar 26 17:59:28 2015

@author: sljg2
"""

__all__ = ["friends_of_friends"]
__version__ = "0.1.3-dev"
__author__ = "Simon Gibbons (sljg2@ast.cam.ac.uk)"
__copyright__ = "Copyright 2015 Simon Gibbons"

cimport numpy as np
import numpy as np
from libcpp.vector cimport vector

cdef extern from "fof.hpp":
    cdef vector[vector[size_t]] _friends_of_friends "friends_of_friends"(double*, size_t, size_t, double) except +

def friends_of_friends(data, double linking_length):
    """ Computes friends-of-friends clustering of data. Distances are computed
    using a euclidian metric.

        :param data: A numpy array with dimensions (npoints x ndim)

        :param linking_length: The linking length between cluster members

        :rtype: A list of lists of indices in each cluster type
    """

    cdef np.ndarray[double, ndim=2, mode='c'] data_array = np.asarray(
        data,
        order='C',
        dtype=np.float,
    )

    if np.any( np.isnan(data) ):
        raise ValueError("NaN detected in pyfof")

    num_points = data_array.shape[0]
    num_dimensions = data_array.shape[1]
    return _friends_of_friends(
        &data_array[0,0],
        num_points,
        num_dimensions,
        linking_length,
    )

