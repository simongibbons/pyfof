import numpy as np
import pytest

import pyfof


@pytest.mark.parametrize("dimensions", range(1, 10))
def test_two_points_seperated_on_x_axis(dimensions):
    points = np.array([
        [0] + [0] * (dimensions - 1),
        [1] + [0] * (dimensions - 1),
    ], dtype=np.float64)

    assert pyfof.friends_of_friends(points, 0.5) == [[0], [1]]
    assert pyfof.friends_of_friends(points, 0.999) == [[0], [1]]
    assert pyfof.friends_of_friends(points, 1.001) == [[0, 1]]
    assert pyfof.friends_of_friends(points, 2.0) == [[0, 1]]

