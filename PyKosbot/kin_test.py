import numpy as np
from enum import Enum
import pandas as pd
np.set_printoptions(suppress=True)
class Axis(Enum):
    X = 0
    Y = 1
    Z = 2
    I = 3

def Rotation_Matrix(axis, theta):
    matrix = np.identity(4)
    theta_rad = np.deg2rad(theta)
    ct = np.cos(theta_rad)
    st = np.sin(theta_rad)
    
    if axis == Axis.X:
        matrix[1][1] = ct
        matrix[2][1] = st
        
        matrix[1][2] = -st
        matrix[2][2] = ct
        
    elif axis == Axis.Y:
        matrix[0][0] = ct
        matrix[0][2] = st
        
        matrix[2][0] = -st
        matrix[2][2] = ct
    elif axis == Axis.Z:
        matrix[0][0] = ct
        matrix[1][0] = st
        
        matrix[0][1] = -st
        matrix[1][1] = ct
    elif axis == Axis.I:
        return matrix
    
    return matrix 

def Translation_Matrix(vec):
    
    matrix = np.identity(4)
    matrix[0][3] = vec[0]
    matrix[1][3] = vec[1]
    matrix[2][3] = vec[2]
    
    return matrix

r1 = Rotation_Matrix(Axis.Z, 30)
t1 = Translation_Matrix(np.array((0, 0, 20)))
T1 = np.matmul(r1, t1)
print("T1 : ")
print(T1)

r2 = Rotation_Matrix(Axis.X, 90)
t2 = Translation_Matrix(np.array((10, 0, 0)))
T2 = np.matmul(r2, t2)
#T2 = np.matmul(T2, Rotation_Matrix(Axis.Z, -90))
print("T2 : ")
print(T2)

r3 = Rotation_Matrix(Axis.Z, 0)
t3 = Translation_Matrix(np.array((10, 0, 0)))
T3 = np.matmul(r3, t3)
print("T3 : ")
print(T3)

T12 = np.matmul(T1, T2)
T23 = np.matmul(T2, T3)
T123 = np.matmul(T12, T3)
print("T1 : ")
print(T1)

print("T12 : ")
print(T12)

print("T123 : ")
print(T123)
