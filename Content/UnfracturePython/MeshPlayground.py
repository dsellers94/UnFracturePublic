'''
This script is just to generate a toy rectangular mesh and associated data.
It will be used to generate a sample stiffness matrix and trianlge set for
importing into Unreal/Eigen/RMC
'''

import csv
import numpy as np
from scipy import linalg
from UnfractureClasses import Node
from UnfractureClasses import Triangle


Nx = 7
Ny = 4

Width = 600
Length = 200

Nodes = []
Triangles = []
Positions = []


def GenerateMesh():
    for i in range(Nx*Ny):

        NewNode = Node()
        NewNode.NeighborIndices = []
        
        NewNode.Index = i
        
        NewNode.Position = [0.0 ,(i % Nx) * Width / (Nx - 1), (i // Nx) * Length / (Ny - 1)]

        #Don't give right-edge nodes neighbors to the right
        if (i + 1) % Nx != 0:

            NewNode.NeighborIndices.append(i + 1)

        #Don't give left-edge nodes neighbors to the left
        if i % Nx != 0:

            NewNode.NeighborIndices.append(i - 1)

        #Don't give first-row nodes neighbors below them
        if i - Nx >= 0:
            
            NewNode.NeighborIndices.append(i - Nx)

            #Don't give right-edge nodes neighbors below them to the right
            if (i + 1) % Nx != 0:

                NewNode.NeighborIndices.append(i - Nx + 1)

        #Don't give top-row nodes neighbors above them
        if (i + Nx) <= (Nx * Ny - 1):

            NewNode.NeighborIndices.append(i + Nx)

            #Don't give left-edge nodes neighbors above them to the left
            if i % Nx != 0:

                NewNode.NeighborIndices.append(i + Nx -1)

        Nodes.append(NewNode)

def GenerateSeparationVectors():
    for Node in Nodes:
        for Neighbor in Node.NeighborIndices:
            localPosition = Node.Position
            neighborPosition = Nodes[Neighbor].Position

            separationVector = [neighborPosition[0] - localPosition[0], neighborPosition[1] - localPosition[1], neighborPosition[2] - localPosition[2]]

            Node.NeighborSeparationVectors.append(separationVector)

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
        
def ExtractPositions():
    for i in range(len(Nodes)):
        Positions.append(Nodes[i].Position)


def GenerateGlobalMatrix(Nodes, E, A, FixedNodes):
    print("Clits and Tits")
    K = np.zeros(shape = (2 * len(Nodes), 2 * len(Nodes)), dtype=np.float64)

    for i in range(len(Nodes)):
        for j in range(len(Nodes[i].NeighborIndices)):
            separationVector = Nodes[i].NeighborSeparationVectors[j]
            separationMagnitude = np.sqrt( separationVector[1]**2 + separationVector[2]**2 )

            # X component of Kii
            K[2 * i][2 * i] += - E * A * abs(separationVector[1] / separationMagnitude**2)
            # Y componenent of Kii
            K[2 * i + 1][2 * i + 1] += - E * A * abs(separationVector[2] / separationMagnitude**2)
            
            # X component of Kij
            K[2 * i][2 * Nodes[i].NeighborIndices[j]] += E * A * abs(separationVector[1] / separationMagnitude**2)
            # Y copmonent of Kij
            K[2 * i + 1][2 * Nodes[i].NeighborIndices[j] + 1] += E * A * abs(separationVector[2] / separationMagnitude**2)
    
    KRedux = np.zeros( shape = (len(K) - len(FixedNodes), len(K) - len(FixedNodes)))

    #K Reduction Attempts
    '''                  
    for i in range(len(Nodes)):
        for j in range(len(Nodes)):
            if not (i in FixedNodes) and not (j in FixedNodes):
                KRedux[2 * i][2 * j] = K[i][j]
                KRedux[2 * i + 1][2 * j + 1] = K[i + 1][j + 1]
    '''
    
    for i in range(2, len(K)):
        for j in range(2, len(K)):
            KRedux[i-2][j-2] = K[i][j]
    
    
    return K

def GenerateAll():
    GenerateMesh()
    GenerateTriangles()
    ExtractPositions()
    GenerateSeparationVectors()

GenerateAll()

K = GenerateGlobalMatrix(Nodes, 50, 1, [0])

KInverse = np.linalg.inv(K)


def WriteToCSV():
    with open('toymesh_positions.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        for i in range(len(Nodes)):
            csvwriter.writerow(Nodes[i].Position)

    with open('toymesh_triangles.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        for i in range(len(Triangles)):
            csvwriter.writerow([Triangles[i].Index0])
            csvwriter.writerow([Triangles[i].Index1])
            csvwriter.writerow([Triangles[i].Index2])

#MiniMesh Experiment 

MiniNodes = []
MiniTriangles = []

def GenerateMiniMesh():

    NewNode = Node()
    NewNode.NeighborIndices = [1,2]
    NewNode.Index = 0
    NewNode.Position = [0.0, 0.0, 0.0]
    MiniNodes.append(NewNode)

    NewNode = Node()
    NewNode.NeighborIndices = [0,2]
    NewNode.Index = 1
    NewNode.Position = [0.0, 100.0, 0.0]
    MiniNodes.append(NewNode)

    NewNode = Node()
    NewNode.NeighborIndices = [0,1]
    NewNode.Index = 2
    NewNode.Position = [0.0, 0.0, 100.0]
    MiniNodes.append(NewNode)

def GenerateMiniSeparationVectors():
    for Node in MiniNodes:
        for Neighbor in Node.NeighborIndices:
            localPosition = Node.Position
            neighborPosition = MiniNodes[Neighbor].Position

            separationVector = [neighborPosition[0] - localPosition[0], neighborPosition[1] - localPosition[1], neighborPosition[2] - localPosition[2]]

            Node.NeighborSeparationVectors.append(separationVector)
            
def GenerateMiniTriangles():
    MiniTriangles.append(0)
    MiniTriangles.append(1)
    MiniTriangles.append(2)

def GenerateMiniGloablMatrix(MiniNodes, E, A):
    print("Clits and Tits")
    MiniK = np.zeros(shape = (2 * len(MiniNodes), 2 * len(MiniNodes)), dtype=np.float64)

    for i in range(len(MiniNodes)):
        for j in range(len(MiniNodes[i].NeighborIndices)):
            separationVector = MiniNodes[i].NeighborSeparationVectors[j]
            separationMagnitude = np.sqrt( separationVector[1]**2 + separationVector[2]**2 )

            # X component of Kii
            MiniK[2 * i][2 * i] += - E * A * abs(separationVector[1] / separationMagnitude)
            # Y componenent of Kii
            MiniK[2 * i + 1][2 * i + 1] += - E * A * abs(separationVector[2] / separationMagnitude)
            
            # X component of Kij
            MiniK[2 * i][2 * MiniNodes[i].NeighborIndices[j]] += E * A * abs(separationVector[1] / separationMagnitude)
            # Y copmonent of Kij
            MiniK[2 * i + 1][2 * MiniNodes[i].NeighborIndices[j] + 1] += E * A * abs(separationVector[2] / separationMagnitude)


    for i in range(len(MiniK)):
        MiniK[0][i] = 0
        MiniK[1][i] = 0
        MiniK[i][0] = 0
        MiniK[i][1] = 0

        MiniK[2][i] = 0
        MiniK[3][i] = 0
        MiniK[i][2] = 0
        MiniK[i][3] = 0
    

    return MiniK
'''
GenerateMiniMesh()
GenerateMiniTriangles()
GenerateMiniSeparationVectors()
MiniK = GenerateMiniGloablMatrix(MiniNodes, 50, 1)
'''
