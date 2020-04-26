from pymarchingCubes import MarchingSquares
import matplotlib.pyplot as plt
from math import cos, sin, exp
from numpy import linspace


def func(x,y):
    # return sin(5 * x) * cos(5 * y)/5
    # return sin(0.1 * x * x + 0.2 * y * y)
    # return (1-(x**2+y**3))*exp(-(x**2+y**2)/2)
    # return exp(sin(x) + cos(y)) -sin(exp(x+y))
    # return sin(sin(x) + cos(y)) - cos(sin(x*y) + cos(x))
    return sin(x**2 + y**2) - cos(x*y)

ms = MarchingSquares(func, [-10, 10], [-10,10], [250,250])

# levels = linspace(0,1,11)

levels = [0.5]

for lvl in levels:
    result = ms.compute(lvl)
    print("Done computing result")
    
    for edge in result:
        plt.plot([edge[0][0], edge[1][0]], [edge[0][1], edge[1][1]], 'r')

plt.axis('equal')
plt.show()
