import numpy as np
import csv

class Node:

    def __init__(self):
        self.Index = 0
        self.Position = []
        self.NeighborIndices = []
        self.XIndex = -1
        self.YIndex = -1
        self.BoundEdgeLineIndices = []
        self.NeighborSeparationVectors = []
        self.IsEdgeNode = False
        self.IsCornerNode = False
        self.IsOffGridCorner = False
        self.IsOffGridEdge = False
        

class EdgeLine:
     
    def __init__(self):
        self.m = 0
        self.b = 0
        self.StartPoint = []
        self.EndPoint = []
        self.IsHorizontal = False
        self.IsVertical = False
        self.IsLeftBoundary = False
        self.IsRightBoundary = False
        self.IsLowerBoundary = False
        self.IsUpperBoundary = False

    def Calculate(self):

        #Check if line is vertical
        if self.EndPoint[1] == self.StartPoint[1]:
            self.IsVertical = True

        #Check if line is horiizontal
        if self.EndPoint[2] == self.StartPoint[2]:
            self.IsHorizontal = True

        #If not vertical, calculate slope and y-intercept
        if not self.IsVertical:
            self.m = (self.EndPoint[2] - self.StartPoint[2]) / (self.EndPoint[1] - self.StartPoint[1])
            self.b = self.StartPoint[2] - self.StartPoint[1] * self.m

        #Determine if line is an upper or lower boundary, a left or right boundary.
        if self.IsVertical:
            if self.EndPoint[2] < self.StartPoint[2]:
                self.IsLeftBoundary = True
            else:
                self.IsRightBoundary = True
        elif self.IsHorizontal:
            if self.EndPoint[1] > self.StartPoint[1]:
                self.IsLowerBoundary = True
            else:
                self.IsUpperBoundary = True
        else:
            if self.EndPoint[2] < self.StartPoint[2]:
                self.IsLeftBoundary = True
            else:
                self.IsRightBoundary = True
            if self.EndPoint[1] > self.StartPoint[1]:
                self.IsLowerBoundary = True
            else:
                self.IsUpperBoundary = True

    def print(self):
        print("m: " + str(self.m) + " b: " + str(self.b))
        print("Vertical: " + str(self.IsVertical) + " Horizontal: " + str(self.IsHorizontal))
        print("Left: " + str(self.IsLeftBoundary) + " Lower: " + str(self.IsLowerBoundary))
        print("Right: " + str(self.IsRightBoundary) + " Upper: " + str(self.IsUpperBoundary))

class Triangle:
    def __init__(self, In0, In1, In2):
        self.Index0 = In0
        self.Index1 = In1
        self.Index2 = In2

    def print(self):
        print(str(self.Index0) + "," + str(self.Index1) + "," + str(self.Index2))

# Generate the full global stiffness matrix for a given set of nodes with elastic modulus E and cross-section A
def GenerateGlobalMatrix(Nodes, E, A):
    print("Clits and Tits")
    K = np.zeros(shape = (2 * len(Nodes), 2 * len(Nodes)), dtype=np.float64)

    for i in range(len(Nodes)):
        for j in range(len(Nodes[i].NeighborIndices)):
            separationVector = Nodes[i].NeighborSeparationVectors[j]
            separationMagnitude = np.sqrt( separationVector[1]**2 + separationVector[2]**2 )

            # X component of Kii
            K[2 * i][2 * i] += E * A * abs(separationVector[1] / separationMagnitude**2)
            # Y componenent of Kii
            K[2 * i + 1][2 * i + 1] += E * A * abs(separationVector[2] / separationMagnitude**2)
            
            # X component of Kij
            K[2 * i][2 * Nodes[i].NeighborIndices[j]] += - E * A * abs(separationVector[1] / separationMagnitude**2)
            # Y copmonent of Kij
            K[2 * i + 1][2 * Nodes[i].NeighborIndices[j] + 1] += - E * A * abs(separationVector[2] / separationMagnitude**2)
    
    return K


# Reduces the matrix to account for fixed nodes. Fixed Nodes will also need to be passed to C++ and accounted for in all future calculations

KRedux = []

def ReduceMatrix(K, FixedNodeIndices):
    for i in range(int(len(K) / 2)):
        KReduxSubZero = []
        KReduxSubOne = []
        if i not in FixedNodeIndices:
            for j in range(int(len(K) / 2)):
                if j not in FixedNodeIndices:
                    KReduxSubZero.append(K[2 * i][2 * j])
                    KReduxSubZero.append(K[2 * i][2 * j + 1])
                    KReduxSubOne.append(K[2 * i + 1][2 * j])
                    KReduxSubOne.append(K[2 * i + 1][2 * j + 1])
            KRedux.append(KReduxSubZero)
            KRedux.append(KReduxSubOne)

    return KRedux

# Reduces the matrix to only include the edge nodes

KReduxInversePrime = []

def ReduceMatrixPrime(kReduxInverse, nodes, edgeNodeIndices, fixedNodeIndices, edgeNodeKMatrixMap):

    FixedNodesPassedI = 0

    InternalNodesPassed = 0

    for i in range(len(nodes)):
        KReduxInversePrimeSubZero = []
        KReduxInversePrimeSubOne = []

        if i in fixedNodeIndices:
            FixedNodesPassedI += 1

        elif i in edgeNodeIndices:

            edgeNodeKMatrixMap[i] = i - FixedNodesPassedI - InternalNodesPassed

            FixedNodesPassedJ = 0

            for j in range(len(nodes)):

                if j in fixedNodeIndices:
                    FixedNodesPassedJ += 1
                    
                elif j in edgeNodeIndices :
                    KReduxInversePrimeSubZero.append(kReduxInverse[2 * (i - FixedNodesPassedI)][2 * (j - FixedNodesPassedJ)])
                    KReduxInversePrimeSubZero.append(kReduxInverse[2 * (i - FixedNodesPassedI)][2 * (j - FixedNodesPassedJ) + 1])
                    KReduxInversePrimeSubOne.append(kReduxInverse[2 * (i - FixedNodesPassedI) + 1][2 * (j - FixedNodesPassedJ)])
                    KReduxInversePrimeSubOne.append(kReduxInverse[2 * (i - FixedNodesPassedI) + 1][2 * (j - FixedNodesPassedJ) + 1])

            KReduxInversePrime.append(KReduxInversePrimeSubZero)
            KReduxInversePrime.append(KReduxInversePrimeSubOne)

        else:
            
            InternalNodesPassed += 1
            

    return KReduxInversePrime

def GenerateSeparationVectors(Nodes):
    for Node in Nodes:
        for Neighbor in Node.NeighborIndices:
            localPosition = Node.Position
            neighborPosition = Nodes[Neighbor].Position

            separationVector = [neighborPosition[0] - localPosition[0], neighborPosition[1] - localPosition[1], neighborPosition[2] - localPosition[2]]

            Node.NeighborSeparationVectors.append(separationVector)


def ExtractPositions(Nodes):
    Positions = []
    for i in range(len(Nodes)):
        Positions.append(Nodes[i].Position)
    return Positions

def ExtractPositionsWithCenter(nodes, positions, nx, ny, deltaX, deltaY):
    for i in range(len(nodes)):
        positions.append(nodes[i].Position)
    positions.append([0.0, (nx / 2) * deltaX, (ny / 2) * deltaY])

def ExtractEdgeNodePositionsWithCenter(edgeNodes, edgeNodePositions, nx, ny, deltaX, deltaY):
    for i in range(len(edgeNodes)):
        edgeNodePositions.append(edgeNodes[i].Position)
    edgeNodePositions.append([0.0, (nx / 2) * deltaX, (ny / 2) * deltaY])


#This currently only works for nodes that conform to the grid. Diagonal line support upcoming
def GenerateTriangles(nodes, triangles):
    for i in range(len(nodes)):

        TriangulationNode = nodes[i]
        RightNeighborIndex = None
        LeftNeighborIndex = None
        UpperNeighborIndex = None
        LowerNeighborIndex = None
        LocalXIndex = TriangulationNode.XIndex
        LocalYIndex = TriangulationNode.YIndex
        
        #Gather Neighbors needed for triangles to the right and lower left if available
        for j in range(len(TriangulationNode.NeighborIndices)):
            CandidateNode = nodes[TriangulationNode.NeighborIndices[j]]
            CandidateNodeIndex = TriangulationNode.NeighborIndices[j]
            if CandidateNode.YIndex == LocalYIndex:
                if CandidateNode.XIndex == LocalXIndex + 1:
                    RightNeighborIndex = CandidateNodeIndex
                elif CandidateNode.XIndex == LocalXIndex - 1:
                    LeftNeighborIndex = CandidateNodeIndex
            elif CandidateNode.XIndex == LocalXIndex:
                if CandidateNode.YIndex == LocalYIndex + 1:
                    UpperNeighborIndex = CandidateNodeIndex
                elif CandidateNode.YIndex == LocalYIndex - 1:
                    LowerNeighborIndex = CandidateNodeIndex
            

        #Create Lower Left and Upper Right Triangles if possible

        if LeftNeighborIndex != None and LowerNeighborIndex != None:
            NewTriangle = Triangle(i, LeftNeighborIndex, LowerNeighborIndex)
            triangles.append(NewTriangle)
        if RightNeighborIndex != None and UpperNeighborIndex != None:
            NewTriangle = Triangle(i, RightNeighborIndex, UpperNeighborIndex)
            triangles.append(NewTriangle)

def GenerateCentralTriangles(nodes, edgeNodes, edgeNodeIndices, triangles):
    CenterNodeIndex = nodes[-1].Index + 1
    TriangulationNode = edgeNodes[0]
    NextTriangulationNode = edgeNodes[-1]
    limitCount = 0
    UpperRightMode = False
    DownMode = False

    #Check each neighbor starting from the Lower Right and moving counterclockwise as a valid candidate for a triangulation partner. Stop when we return
    #to the starting node
    while (NextTriangulationNode is not edgeNodes[0]):
        LowerRightNeighborIndex = None
        RightNeighborIndex = None
        UpperRightNeighborIndex = None
        UpperNeighborIndex = None
        UpperLeftNeighborIndex = None
        LeftNeighborIndex = None
        LowerLeftNeighborIndex = None
        LowerNeighborIndex = None
        LocalXIndex = TriangulationNode.XIndex
        LocalYIndex = TriangulationNode.YIndex

        for j in range(len(TriangulationNode.NeighborIndices)):
            CandidateNode = nodes[TriangulationNode.NeighborIndices[j]]
            CandidateNodeIndex = TriangulationNode.NeighborIndices[j]
            if CandidateNode.XIndex == LocalXIndex + 1:
                if CandidateNode.YIndex == LocalYIndex - 1:
                    LowerRightNeighborIndex = CandidateNodeIndex
                elif CandidateNode.YIndex == LocalYIndex:
                    RightNeighborIndex = CandidateNodeIndex
                elif CandidateNode.YIndex == LocalYIndex + 1:
                    UpperRightNeighborIndex = CandidateNodeIndex
            elif CandidateNode.YIndex == LocalYIndex + 1:
                if CandidateNode.XIndex == LocalXIndex:
                    UpperNeighborIndex = CandidateNodeIndex
                elif CandidateNode.XIndex == LocalXIndex - 1:
                    UpperLeftNeighborIndex = CandidateNodeIndex
            elif CandidateNode.XIndex == LocalXIndex - 1:
                if CandidateNode.YIndex == LocalYIndex:
                    LeftNeighborIndex = CandidateNodeIndex
                elif CandidateNode.YIndex == LocalYIndex - 1:
                    LowerLeftNeighborIndex = CandidateNodeIndex
            elif CandidateNode.YIndex == LocalYIndex - 1 and CandidateNode.XIndex == LocalXIndex:
                LowerNeighborIndex = CandidateNodeIndex

        #This is some of the ugliest code I've ever written but I'm not sure how to clean it up right now
        if LowerRightNeighborIndex != None and LowerRightNeighborIndex in edgeNodeIndices and UpperRightMode == False:
            
            NextTriangulationNode = nodes[LowerRightNeighborIndex]
            NewTriangle = Triangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex)
            triangles.append(NewTriangle)

        elif RightNeighborIndex != None and RightNeighborIndex in edgeNodeIndices and UpperRightMode == False:

            NextTriangulationNode = nodes[RightNeighborIndex]
            NewTriangle = Triangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex)
            triangles.append(NewTriangle)

        elif UpperRightNeighborIndex != None and UpperRightNeighborIndex in edgeNodeIndices and UpperRightMode == False:

            NextTriangulationNode = nodes[UpperRightNeighborIndex]
            NewTriangle = Triangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex)
            triangles.append(NewTriangle)

        elif UpperNeighborIndex != None and UpperNeighborIndex in edgeNodeIndices and DownMode == False:

            NextTriangulationNode = nodes[UpperNeighborIndex]
            NewTriangle = Triangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex)
            triangles.append(NewTriangle)
            UpperRightMode = True

        elif UpperLeftNeighborIndex != None and UpperLeftNeighborIndex in edgeNodeIndices and DownMode == False:

            NextTriangulationNode = nodes[UpperLeftNeighborIndex]
            NewTriangle = Triangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex)
            triangles.append(NewTriangle)

        elif LeftNeighborIndex != None and LeftNeighborIndex in edgeNodeIndices and DownMode == False:

            NextTriangulationNode = nodes[LeftNeighborIndex]
            NewTriangle = Triangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex)
            triangles.append(NewTriangle)

        elif LowerLeftNeighborIndex != None and LowerLeftNeighborIndex in edgeNodeIndices and DownMode == False:

            NextTriangulationNode = nodes[LowerLeftNeighborIndex]
            NewTriangle = Triangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex)
            triangles.append(NewTriangle)

        elif LowerNeighborIndex != None and LowerNeighborIndex in edgeNodeIndices:

            NextTriangulationNode = nodes[LowerNeighborIndex]
            NewTriangle = Triangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex)
            triangles.append(NewTriangle)
            DownMode = True

        limitCount += 1
        if limitCount > len(edgeNodes) + 5:
            break

        TriangulationNode = NextTriangulationNode;

def CreateCornerNodes(cornerNodePositions, cornerNodes):
    for cornerNodePosition in cornerNodePositions:
        NewCornerNode = Node()
        NewCornerNode.IsCornerNode = True
        NewCornerNode.Position = cornerNodePosition
        cornerNodes.append(NewCornerNode)

def DefineEdgesAndGrid(cornerNodes, edgeLines, nx, ny):
    #Start from index -1 to index 0 to avoid boolean check

    global XMin
    XMin = 1000000
    global YMin
    YMin = 1000000
    global XMax
    XMax = -1000000
    global YMax
    YMax = -1000000
        
    for i in range(-1, len(cornerNodes) - 1):
        NewEdgeLine = EdgeLine()
        NewEdgeLine.StartPoint = cornerNodes[i].Position
        NewEdgeLine.EndPoint = cornerNodes[i + 1].Position
        NewEdgeLine.Calculate()
        edgeLines.append(NewEdgeLine)
        
        XPos = cornerNodes[i].Position[1]
        YPos = cornerNodes[i].Position[2]
        if XPos > XMax:
            XMax = XPos
        if XPos < XMin:
            XMin = XPos
        if YPos > YMax:
            YMax = YPos
        if YPos < YMin:
            YMin = YPos

    DeltaX = (XMax - XMin) / nx
    DeltaY = (YMax - YMin) / ny

    return DeltaX, DeltaY

def GenerateMesh(nodes, nodeGridMap, edgeLines, edgeNodes, nx, ny, deltaX, deltaY):

    #initialize each potential node in the grid so that it's position can be used to
    #check against the edgeline. Add it to the list if it is inside the shape. Also
    #add any edge nodes that live on verticals or horizontals and place them in the
    #edge nodes list. Also place the node or a Null value on the grid map.
    #Editing now a few days later because we need to nodes on the diagonal edges to
    #be counted as edge nodes, even though they are not on the true diagonal. With
    #this change I think we'll need to branch the work to implement true diagonal
    #edge nodes going forward.
    indexCount = 0
    
    for i in range(nx + 1):
        
        nodeGridMap.append([])
        
        for j in range(ny + 1):

            validCandidate = True
            
            NewNode = Node()
            NewNode.NeighborInidces = []

            NewNode.Index = indexCount
            NewNode.XIndex = i
            NewNode.YIndex = j

            NewNode.Position = [0.0, i * deltaX, j * deltaY]

            for k in range(len(edgeLines)):

                if not edgeLines[k].IsVertical and not edgeLines[k].IsHorizontal:

                    YBoundary = edgeLines[k].m * (i * deltaX) + edgeLines[k].b

                    if edgeLines[k].IsLowerBoundary:
                        if j * deltaY < YBoundary:
                            validCandidate = False
                            #print("Node Below Lower Boundary: " + str(i) + "," + str(j))
                            break

                    elif edgeLines[k].IsUpperBoundary:
                        if j * deltaY > YBoundary:
                            validCandidate = False
                            #print("Node Above Upper Boundary: " + str(i) + "," + str(j))
                            break

                elif edgeLines[k].IsVertical:
                    if i * deltaX == edgeLines[k].StartPoint[1]:
                        NewNode.IsEdgeNode = True

                elif edgeLines[k].IsHorizontal:
                    if j * deltaY == edgeLines[k].StartPoint[2]:
                        NewNode.IsEdgeNode = True

            if validCandidate:
                nodes.append(NewNode)
                
                nodeGridMap[i].append(NewNode.Index)

                indexCount += 1

                if NewNode.IsEdgeNode:
                    edgeNodes.append(NewNode)

            else:
                nodeGridMap[i].append(None)
        

def GenerateMeshConnections(nodes, nodeGridMap, nx, ny):
    #Diagonal Edges not yet supported
    for i in range(len(nodes)):

        ConnectingNode = nodes[i]
        
        RightNeighborIndex = None
        UpperRightNeighborIndex = None
        UpperNeighborIndex = None
        UpperLeftNeighborIndex = None
        LowerNeighborIndex = None
        LeftNeighborIndex = None

        #Will handle off-grid corner nodes separately...somehow
        if not ConnectingNode.IsOffGridCorner:
            ConnectingNodeXIndex = ConnectingNode.XIndex
            ConnectingNodeYIndex = ConnectingNode.YIndex

            #Find the list index of each neighbor from the NodeGridMap (Lower Left and Lower Right Neighbors not needed,
            #they will be taken care of by the previous nodes and aren't needed for diagonal edge checks
            if ConnectingNodeXIndex != nx:
                RightNeighborIndex = nodeGridMap[ConnectingNodeXIndex + 1][ConnectingNodeYIndex]
                if ConnectingNodeYIndex != ny:
                    UpperRightNeighborIndex = nodeGridMap[ConnectingNodeXIndex + 1][ConnectingNodeYIndex + 1]
            if ConnectingNodeYIndex != ny:
                UpperNeighborIndex = nodeGridMap[ConnectingNodeXIndex][ConnectingNodeYIndex + 1]
                if ConnectingNodeXIndex != 0:
                    UpperLeftNeighborIndex = nodeGridMap[ConnectingNodeXIndex - 1][ConnectingNodeYIndex + 1]
            if ConnectingNodeYIndex != 0:
                LowerNeighborIndex = nodeGridMap[ConnectingNodeXIndex][ConnectingNodeYIndex - 1]
            if ConnectingNodeXIndex != 0:
                LeftNeighborIndex = nodeGridMap[ConnectingNodeXIndex - 1][ConnectingNodeYIndex]

            #Assign neighbor relationships with Right, Upper, UpperRight and UpperLeft Neighbors, if they exist
            if RightNeighborIndex != None:
                ConnectingNode.NeighborIndices.append(RightNeighborIndex)
                nodes[RightNeighborIndex].NeighborIndices.append(i)
            if UpperNeighborIndex != None:
                ConnectingNode.NeighborIndices.append(UpperNeighborIndex)
                nodes[UpperNeighborIndex].NeighborIndices.append(i)
            if UpperRightNeighborIndex != None:
                ConnectingNode.NeighborIndices.append(UpperRightNeighborIndex)
                nodes[UpperRightNeighborIndex].NeighborIndices.append(i)
            if UpperLeftNeighborIndex != None:
                ConnectingNode.NeighborIndices.append(UpperLeftNeighborIndex)
                nodes[UpperLeftNeighborIndex].NeighborIndices.append(i)

        #Loop again to check for cardinal direction neighbors and add diagonal edge lines if needed. Then hookup the
        #neighbor relations for all diagonal edge nodes... somehow

def AddSawToothDiagonalsToEdgeNodes(nodes, nodeGridMap, edgeNodes):
    previousValue = nodeGridMap[0][0]
    for i in range(len(nodeGridMap)):
        for j in range(len(nodeGridMap[i])):
            currentValue = nodeGridMap[i][j]
            if currentValue != None and previousValue == None:
                if not nodes[currentValue] in edgeNodes:
                    edgeNodes.append(nodes[currentValue])
            elif currentValue == None and previousValue != None:
                if not nodes[previousValue] in edgeNodes:
                    edgeNodes.append(nodes[previousValue])
            previousValue = currentValue

def GenerateEdgeNodesWithCenter(edgeNodes, edgeNodesWithCenter, nx, ny, deltaX, deltaY):
    for i in range(len(edgeNodes)):
        edgeNodesWithCenter.append(edgeNodes[i])
    CenterNode = Node()
    CenterNode.Position = [0.0, (nx / 2) * deltaX, (ny / 2) * deltaY]
    CenterNode.Index = edgeNodes[-1].Index + 1
    edgeNodesWithCenter.append(CenterNode)

def WriteToCSV(MeshNamePrefix, positions, Triangles, KReduxInverse, KReduxInversePrime, FixedNodeIndices, EdgeNodeIndices, EdgeNodePositions, EdgeNodeKMatrixMap, MetaData):

    with open(MeshNamePrefix + '_positions.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        for pos in positions:
            csvwriter.writerow(pos)

    with open(MeshNamePrefix + '_EdgeNodePositions.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        for pos in EdgeNodePositions:
            csvwriter.writerow(pos)

    with open(MeshNamePrefix + '_triangles.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        for i in range(len(Triangles)):
            csvwriter.writerow([Triangles[i].Index0])
            csvwriter.writerow([Triangles[i].Index1])
            csvwriter.writerow([Triangles[i].Index2])

    with open(MeshNamePrefix + '_KReduxInverse.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        for i in range(len(KReduxInverse)):
            csvwriter.writerow(KReduxInverse[i])

    with open(MeshNamePrefix + '_KReduxInversePrime.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        for i in range(len(KReduxInversePrime)):
            csvwriter.writerow(KReduxInversePrime[i])

    with open(MeshNamePrefix + '_FixedNodes.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(FixedNodeIndices)

    with open(MeshNamePrefix + '_EdgeNodes.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(EdgeNodeIndices)

    with open(MeshNamePrefix + '_EdgeNodeKMatrixMap.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        for key in EdgeNodeKMatrixMap.keys():
            outputlist = [key ,EdgeNodeKMatrixMap[key]]
            csvwriter.writerow(outputlist)

    with open(MeshNamePrefix + '_MetaData.csv', 'w', newline = '') as csvfile:
        csvwriter = csv.writer(csvfile)
        for value in MetaData:
            csvwriter.writerow([value])



    



        
            
