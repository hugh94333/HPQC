#!/usr/bin/env python3

import numpy as np
import sys

# Initialize empty quantum workspace
workspace = np.array([[1.0]])

def pushQubit(weights):
    global workspace
    workspace = np.reshape(workspace, (1, -1))
    workspace = np.kron(workspace, weights)

def main():
    global workspace

    if len(sys.argv) != 3:
        print("Usage: ./push_qubit.py alpha beta")
        print("Example: ./push_qubit.py 1 0")
        return

    alpha = float(sys.argv[1])
    beta = float(sys.argv[2])

    weights = np.array([alpha, beta])

    # Normalize weights
    weights = weights / np.linalg.norm(weights)

    pushQubit(weights)

    print("Workspace:")
    print(workspace)

if __name__ == "__main__":
    main()
