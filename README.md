pyfof
=====

Friends of Friends cluster finding in python.

This implements the friends-of-friends algorithm to find clusters of points,
accelerated with an R*-tree.

## Requirements
 * c++11 complient compiler
 * [Cython](http://cython.org/)
 * [numpy](http://www.numpy.org/)
 * [boost](http://www.boost.org/)

## Examples

### Two Gaussian blobs

```python
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import pyfof

npts = 10000
data = np.vstack((np.random.normal(-1,0.2,(npts/2,2)),\
                  np.random.normal(1,0.2,(npts/2,2))))

groups = pyfof.friends_of_friends(data, 0.4)

colors = cm.rainbow(np.linspace(0, 1, len(groups)))
for g,c in zip(groups, colors):
    plt.scatter(data[g,0], data[g,1], color=c, s=3)

plt.show()
```

![gaussian_example](/../screenshots/img/gaussian_example.png)

### A more complicated example

![stream_example](/../screenshots/img/stream_example.png)