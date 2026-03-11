import numpy as np

# Example workspace
workspace = np.array([[1.]])

def pushQubit(weights):
    global workspace
    workspace = np.reshape(workspace,(1,-1))
    workspace = np.kron(workspace,weights)

def tosQubit(k):
    global workspace
    if k > 1:                                  # only needed if not already top
        workspace = np.reshape(workspace,(-1,2,2**(k-1)))
        workspace = np.swapaxes(workspace,-2,-1)


# Example run
pushQubit([-1,1])       # Qubit 1
pushQubit([0.6,0.8])   # Qubit 2

print("Before moving qubit:")
print(workspace)

tosQubit(2)            # move 2nd qubit to top

print("After moving qubit:")
print(np.reshape(workspace,(1,-1)))
