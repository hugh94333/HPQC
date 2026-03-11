import numpy as np

# workspace example (from Instruction 1)
workspace = np.array([[1.]])

def pushQubit(weights):
    global workspace
    workspace = np.reshape(workspace,(1,-1))
    workspace = np.kron(workspace,weights)

def applyGate(gate):
    global workspace
    workspace = np.reshape(workspace,(-1, gate.shape[0]))
    np.matmul(workspace, gate.T, out=workspace)


# Example gate: Pauli X (NOT gate)
X_gate = np.array([[0,1],
                   [1,0]])

# Example gate: Hadamard
H_gate = np.array([[1,1],
                   [1,-1]]) * np.sqrt(1/2)


# Example run
pushQubit([0.6,0.8])        # push |0>
print("Input:", workspace)

applyGate(X_gate)       # apply NOT gate
print("After X gate:", workspace)

applyGate(H_gate)       # apply Hadamard gate
print("After H gate:", workspace)
