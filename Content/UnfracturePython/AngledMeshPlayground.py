'''
This script is just to generate a toy SQUARE ELEMENT rectangular mesh and associated data.
It will be used to generate a sample stiffness matrix and trianlge set for
importing into Unreal/Eigen/RMC
Since our first toy trianlgular DSM method didn't work like AT ALL we're going
try some other mesh shapes and then critically examine the DSM formulation
'''

import csv
import numpy as np
from scipy import linalg
from UnfractureClasses import Node
from UnfractureClasses import Triangle
import UnfractureClasses as uc

# Set Mesh values (this is only for Toy Mesh, not production meshing
Nx = 60
Ny = 20

Width = 600
Length = 200

#Initialize Arrays. This is something like the format we'll use for holding actual game meshes
Nodes = []
Triangles = []
#Positions = [] Now initialized within GenerateAll() as a global

#Generates a rectangular toy mesh with the dimensions and number of nodes specified above by filling Nodes (Class defined in UnfractureClasses.py)
def GenerateMesh():
    for i in range(Nx*Ny):

        NewNode = Node()
        NewNode.NeighborIndices = []
        
        NewNode.Index = i
        
        NewNode.Position = [0.0 ,(i % Nx) * Width / (Nx - 1), (i // Nx) * Length / (Ny - 1)]

        Nodes.append(NewNode)

    for i in range(Nx*Ny):

        ConnectingNode = Nodes[i]
        
        #Don't give right-edge nodes neighbors to the right
        if (i + 1) % Nx != 0:

            ConnectingNode.NeighborIndices.append(i + 1)
            Nodes[i + 1].NeighborIndices.append(i)

            #Don't give top-row nodes neighbors above them to the right
            if (i + Nx) <= (Nx * Ny - 1):

                ConnectingNode.NeighborIndices.append(i + Nx + 1)
                Nodes[i + Nx + 1].NeighborIndices.append(i)

        #Don't give top-row nodes neighbors above them
        if (i + Nx) <= (Nx * Ny - 1):

            ConnectingNode.NeighborIndices.append(i + Nx)
            Nodes[i + Nx].NeighborIndices.append(i)

            #Don't give left-edge nodes neighbors above them to the left
            if i % Nx != 0:

                ConnectingNode.NeighborIndices.append(i + Nx - 1)
                Nodes[i + Nx - 1].NeighborIndices.append(i)

#Calcualtes separation vecotrs and stores them on the Node objects themselves. MOVE TO UnfactrureClasses.py


#Gathers the indexes for each triangle to passed to RMC, need a more general algorithm in UnfractureClasses, this one only works for the toy mesh
def GenerateTriangles():
    for i in range(Nx * (Ny - 1)):

        #As long as this node is not on the left edge:
        if i % Nx != 0:

            RightTriangle = Triangle(i, i + Nx, i + Nx - 1)
            Triangles.append(RightTriangle)

        #As long as this node is not on the right edge:
        if (i + 1) % Nx != 0:

            LeftTriangle = Triangle(i, i + 1, i + Nx)
            Triangles.append(LeftTriangle)

#MOVE TO UnfractureClasses.py    


def GenerateAll():
    GenerateMesh()
    GenerateTriangles()
    global Positions
    Positions = uc.ExtractPositions(Nodes)
    uc.GenerateSeparationVectors(Nodes)

GenerateAll()

#Fix only base nodes for now (Toymesh only)
FixedNodes = []
for i in range(Nx):
    FixedNodes.append(i)

K = uc.GenerateGlobalMatrix(Nodes, 50, 1)
KRedux = uc.ReduceMatrix(K, FixedNodes)

print("Inverting")
KReduxInverse = np.linalg.inv(KRedux)
print("Done")

def InvokeCSVWriter(MeshNamePrefix):
    uc.WriteToCSV(MeshNamePrefix, Nodes, Triangles, KReduxInverse, FixedNodes)
    

