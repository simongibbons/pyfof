import numpy as np
import pytest

import pyfof


@pytest.mark.parametrize("dimensions", range(1, 10))
@pytest.mark.parametrize("dtype", [np.int, np.float64])
def test_two_points_seperated_on_x_axis(dimensions, dtype):
    points = np.array([
        [0] + [0] * (dimensions - 1),
        [1] + [0] * (dimensions - 1),
    ], dtype=dtype)

    assert points.shape == (2, dimensions)

    assert pyfof.friends_of_friends(points, 0.5) == [[0], [1]]
    assert pyfof.friends_of_friends(points, 0.999) == [[0], [1]]
    assert pyfof.friends_of_friends(points, 1.001) == [[0, 1]]
    assert pyfof.friends_of_friends(points, 2.0) == [[0, 1]]


def test_pass_in_points_as_a_list():
    points = [
        [0, 0],
        [0, 0.5],
    ]
    assert pyfof.friends_of_friends(points, 0.4) == [[0], [1]]


def test_use_brute_force_implementation():
    points = [[0, 1], [0, 0]]
    assert pyfof.friends_of_friends(points, 0.5, use_brute=True) == [[0], [1]]

