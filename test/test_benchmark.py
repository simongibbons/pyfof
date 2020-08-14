import pytest
import numpy as np

import pyfof


@pytest.mark.parametrize("use_brute", [False, True])
def test_two_gaussian_blobs(benchmark, use_brute):
    points_per_blob = 10000
    data = np.vstack([
        np.random.normal(-1, 0.2, (points_per_blob, 2)),
        np.random.normal(1, 0.2, (points_per_blob, 2)),
    ])

    groups = benchmark(pyfof.friends_of_friends, data, 0.4, use_brute)
    assert len(groups) == 2
    assert len(groups[0]) == points_per_blob
    assert len(groups[1]) == points_per_blob


@pytest.mark.parametrize("use_brute", [False, True])
def test_uniform_grid(benchmark, use_brute):
    num_points = 20000
    data = [[0, i] for i in range(num_points)]

    groups = benchmark(pyfof.friends_of_friends, data, 0.4, use_brute)
    assert len(groups) == num_points
    assert all(len(g) == 1 for g in groups)


@pytest.mark.parametrize("use_brute", [False, True])
def test_multiple_regions(benchmark, use_brute):
    points_per_blob = 500
    num_blobs = 50
    data = np.vstack([
        np.random.uniform(i, i + 0.5, (points_per_blob, 2))
        for i in range(num_blobs)
    ])

    groups = benchmark(pyfof.friends_of_friends, data, 0.1, use_brute)
    assert len(groups) == num_blobs
    assert all(len(g) == points_per_blob for g in groups)
