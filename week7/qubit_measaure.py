import numpy as np

# Example workspace
workspace = np.array([[1.]])

def pushQubit(weights):
    global workspace
    workspace = np.reshape(workspace,(1,-1))
    workspace = np.kron(workspace,weights)

def probQubit():
    global workspace
    workspace = np.reshape(workspace,(-1,2))
    return np.linalg.norm(workspace,axis=0)**2

def measureQubit():
    global workspace
    
    prob = probQubit()                         # probability of 0 or 1
    measurement = np.random.choice(2, p=prob)  # randomly choose result
    
    workspace = workspace[:,[measurement]] / np.sqrt(prob[measurement])
    
    return str(measurement)


# Example run
pushQubit([0.6,0.8])   # qubit with probabilities 0.36 and 0.64

print("Probabilities:", probQubit())

result = measureQubit()

print("Measurement result:", result)
print("New workspace:", workspace)
