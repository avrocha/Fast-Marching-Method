#!/usr/bin/env python3

import h5py as h5
import numpy as np
from scipy.interpolate import RegularGridInterpolator
import matplotlib.pyplot as plt
import subprocess
import math
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--error_tol', type=float, default=0.1, help='Finish condition')
parser.add_argument('--start', type=float, nargs=2, default=[-1, 1], help='Start coordinates in a domain with [-1,-1]x[1,1] as vertices')
parser.add_argument('--speed', type=float, default=0.1, help = 'Maximum speed of the vehicle (r > 0)')
args = parser.parse_args()

targets = np.empty(0, dtype = float)
start = np.array([args.start[0], args.start[1]])

# Read Value Function

file = h5.File('../data/value_function.h5', "r")
data = file['value_function'][()]
file.close()

npts = data.shape[0]
grid_size = 2 / (npts - 1)

for i in range(npts):
    for j in range(npts):
        if data[i][j] == 0.0:
            targets = np.append(targets, [(2 * i) / (npts - 1) - 1, (2 * j) / (npts - 1) - 1])

# Gradient descent
print('Detected target coordinates : ', targets)

gradient = np.gradient(data, grid_size, edge_order=2)

t = np.linspace(-1, 1, npts)
gt = [0, 1]
grad_interpol = RegularGridInterpolator((gt, t, t), gradient, method='linear', bounds_error=False, fill_value=None)

trajectory = np.array([start], dtype=float)

k = 0
while True : 
    grad = grad_interpol([[0, trajectory[k][0], trajectory[k][1]], [1, trajectory[k][0], trajectory[k][1]]])
    abs_grad = math.sqrt(grad[0]**2 + grad[1]**2)

    curr_i = trajectory[k][0] - args.speed * grad[0] / abs_grad
    curr_j = trajectory[k][1] - args.speed * grad[1] / abs_grad
    
    trajectory = np.append(trajectory, [[curr_i, curr_j]], axis=0)
        
    k = k + 1

    closest_error = math.inf
    i = 0
    while i < targets.size :
        temp = max(abs(trajectory[k][0] - targets[i]), abs(trajectory[k][1] - targets[i+1]))
        if closest_error > temp :
            closest_error = temp
        i = i + 2
    
    print('Discovering trajectory... | Iteration', k, '| Current Deviation', closest_error)

    if closest_error < args.error_tol :
        break
    
# Graphics

x, y = np.meshgrid(t, t, indexing='ij')

plt.pcolormesh(x, y, data, cmap='jet', shading='auto')
plt.colorbar()
plt.contour(x, y, data, levels=15, linestyles='dashed', colors='k')
plt.axis('equal')

plt.plot(trajectory[:, 0], trajectory[:, 1], color='red', linestyle='dashed', linewidth = 2, marker='.', markersize=6)

plt.show()