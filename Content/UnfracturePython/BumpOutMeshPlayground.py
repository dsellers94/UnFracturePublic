'''
In this script we're attempting to develop a dynamic grid meshing algorithm for arbitrary convex shapes with
flat edges.
'''

import csv
import numpy as np
from scipy import linalg
from UnfractureClasses import Node
from UnfractureClasses import Triangle
from UnfractureClasses import EdgeLine
import UnfractureClasses as uc
        
# Set X and Y coursness 
Nx = 40
Ny = 40

#global DeltaX
#global DeltaY

#Initialize Arrays. This is something like the format we'll use for holding actual game meshes
Nodes = []
EdgeNodes = []
EdgeNodesWithCenter = []
CornerNodes = []
EdgeLines = []
Triangles = []
NodeGridMap = []
FixedNodeIndices = []
EdgeNodeIndices = []
EdgeNodePositions = []
Positions = []

'''

Mesh Generation Procedure:

1.Run this script
2.Run GenerateAllFromCornerNodes() with valid corner node posistions for a convex shape
3.Run InvokeCSVWriter() with a string mesh name argument
4.Copy output files to Content/Files
5.Create an instance of CompressableBase (or child class) and change it's MeshName variable to the new mesh name
6.Add an appropriate box collision to the mesh (hoping to automate this in near future)

GenerateAllFromCornerNodes([[0.0, 0.0 , 150.0],[0.0, 800.0, 0.0],[0.0, 800.0, 350.0],[0.0, 450.0, 600.0],[0.0, 0.0, 600.0]])
GenerateAllFromCornerNodes([[0.0, 0.0, 200.0],[0.0, 0.0, 0.0],[0.0, 600.0, 0.0],[0.0, 600.0, 200.0]])

'''

#Not for refactoring, use locally. Call in IDLE
def GenerateAllFromCornerNodes(cornerNodePositions):
    
    uc.CreateCornerNodes(cornerNodePositions, CornerNodes)
    GenerateAll()

#Not refactoring this one yet as it's a work in progress and not in use
def AddRemainingCornerNodesToEdgeNodes():

    #Add the corner nodes that haven't already been added by the internal meshing process,
    #also tell any corner nodes you find you that they are corner nodes, cause they don't know
    #yet and it will make them feel special
    for a in range(len(CornerNodes)):

        validCandidate = True

        for b in range(len(EdgeNodes)):
            if CornerNodes[a].Position == EdgeNodes[b].Position:
                validCandidate = False
                EdgeNodes[b].IsCornerNode = True

        if validCandidate:
            CornerNodes[a].IsOffGridCorner = True
            Nodes.append(CornerNodes[a])
            EdgeNodes.append(CornerNodes[a])



def GenerateAll():
    
    DeltaX, DeltaY = uc.DefineEdgesAndGrid(CornerNodes, EdgeLines, Nx, Ny)
    uc.GenerateMesh(Nodes, NodeGridMap, EdgeLines, EdgeNodes, Nx, Ny, DeltaX, DeltaY)
    uc.GenerateMeshConnections(Nodes, NodeGridMap, Nx, Ny)
    uc.GenerateSeparationVectors(Nodes)

    uc.AddSawToothDiagonalsToEdgeNodes(Nodes, NodeGridMap, EdgeNodes)
    
    ExtractEdgeNodeIndices(EdgeNodes)
    SetFixedNodes(EdgeNodeIndices)

    uc.GenerateCentralTriangles(Nodes, EdgeNodes, EdgeNodeIndices, Triangles)

    uc.ExtractPositionsWithCenter(Nodes, Positions, Nx, Ny, DeltaX, DeltaY)

    uc.ExtractEdgeNodePositionsWithCenter(EdgeNodes, EdgeNodePositions, Nx, Ny, DeltaX, DeltaY)

    global KReduxInverse
    global KReduxInversePrime
    KReduxInverse = GenerateKReduxInverse()
    #KReduxInversePrime = uc.ReduceMatrixPrime(KReduxInverse, Nodes, EdgeNodeIndices, FixedNodeIndices)


#Fix only base nodes for now (Toymesh only) FIX THIS
def SetFixedNodes(edgeNodeIndices):
    for i in range(Ny):
        if i in edgeNodeIndices:
            FixedNodeIndices.append(i)

def ExtractEdgeNodeIndices(edgeNodes):
    for i in range(len(edgeNodes)):
        EdgeNodeIndices.append(edgeNodes[i].Index)

def ExtractEdgeNodesWithCenterIndices(edgeNodesWithCenter):
    for i in range(len(edgeNodesWithCenter)):
        EdgeNodesWithCenterIndices.append(edgeNodesWithCenter[i].Index)

def GenerateKReduxInverse():
    K = uc.GenerateGlobalMatrix(Nodes, 50, 1)
    KRedux = uc.ReduceMatrix(K, FixedNodeIndices)

    print("Inverting")
    KReduxInverse = np.linalg.inv(KRedux)
    print("Done")

    return KReduxInverse


def InvokeCSVWriter(MeshNamePrefix):
    uc.WriteToCSV(MeshNamePrefix, Positions, Triangles, KReduxInverse, KReduxInversePrime, FixedNodeIndices, EdgeNodeIndices, EdgeNodePositions)
    

