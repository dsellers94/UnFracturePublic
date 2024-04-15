
from sympy import integrate
from sympy import symbols
from sympy import Poly
from sympy import diff
from sympy import expand
from sympy import collect
from sympy import N
from sympy import pi
from sympy import cos, sin, sqrt, log
from sympy import re
#from sympy import *
from matplotlib import pyplot as plt
import numpy as np

#from sympy import *
#expr.subs([(x, 2), (y, 4), (z, 0)])
#poly = Poly(expr,var)
#poly.coeffs()
#K1sub = K1sub.subs([(s3,2),(sb,1),(roe,1),(phi_1,1),(m,1)])
#import pdb; pdb.set_trace()

Ns = 30
Nphi = 150
s_start = (1.0)/(Ns)
del_s = (1.0-s_start)/(Ns)
del_phi = (3.14159/2)/Nphi

c = 1
d = 0.5

roe_sub = 1
c_0_sub = 1
c_1_sub = 0
omega_sub = 2

prec1 = 4 #build_element() node values
prec2 = 4 #kel_generate() substitution values

s, s3, sb, phi, phi_1, phi_2, roe, A = symbols("s s3 sb phi phi_1 phi_2 roe A", positive = True)
psi_1, psi_2, psi_3, psi_4, psi_5, psi_6 = symbols("psi_1 psi_2 psi_3 psi_4 psi_5 psi_6", positive = True)
#c_0, c_1, omega = symbols("c_0, c_1, omega")

L1 = (1/A)*(s3*phi_2 - s3*phi + sb*phi - sb*phi_2)
L2 = (1/A)*(s3*phi - s3*phi_1 + sb*phi_2 - sb*phi + s*phi_1 - s*phi_2)
L3 = (1/A)*(sb*phi_1 - sb*phi_2 + s*phi_2 - s*phi_1)

N1 = L1*(2*L1 - 1)
N2 = L2*(2*L2 - 1)
N3 = L3*(2*L3 - 1)
N4 = 4*L1*L2
N5 = 4*L2*L3
N6 = 4*L1*L3

def set_s_range(l,a,b):
    '''defines range to iterate over in (s) for each radial index (l)'''
    s_range = (a*b)/sqrt((b*cos(l*del_phi))**2 + (a*sin(l*del_phi))**2) - s_start
    return s_range

def set_roe_el():
    roe = np.empty([Ns,Nphi])
    for j in range(Ns):
        for l in range(Nphi):
            if s_start + j*(del_s) < s_start + set_s_range(l,c,d):
                roe[j,l] = 1
            else:
                roe[j,l] = 0

    return roe

def get_area(nodes):
    '''
    nodes -> list of lists, nodal coordinates in format [[sb,sb*phi_1],[sb,sb*phi_2],[s3,sb*phi_2]]
    '''
    area = N( nodes[0][1]*(nodes[1][0] - nodes[2][0]) + nodes[1][1]*(nodes[2][0] - nodes[0][0]) + nodes[2][1]*(nodes[0][0] - nodes[1][0]) )

    return area

def K_gen(R,sb_sub,phi_1_sub,phi_2_sub,m,b):

    K_s_ind = integrate(R*s,s)
    K_s_def = K_s_ind.subs(s,m*phi+b) - K_s_ind.subs(s,sb_sub)
    K_s_ex = expand(K_s_def)
    K_s_dict = K_s_ex.as_coefficients_dict()
    #print(K_s_dict)
    K_ind = 0
    for i in K_s_dict:
        #print(i)
        #print(K_s_dict[i])
        int_sub = integrate(i,phi)
        #print(int_sub)
        int_coeff = K_s_dict[i]
        K_ind += int_sub*int_coeff
    K = K_ind.subs(phi,phi_2_sub) - K_ind.subs(phi,phi_1_sub)
    
    return K

def L_K_gen(R,sb_sub,phi_1_sub,phi_2_sub,m,b):

    K_s_ind = integrate(R*s,s)
    K_s_def = K_s_ind.subs(s,sb_sub) - K_s_ind.subs(s,m*phi+b)
    K_s_ex = expand(K_s_def)
    K_s_dict = K_s_ex.as_coefficients_dict()
    #print(K_s_dict)
    K_ind = 0
    for i in K_s_dict:
        #print(i)
        int_sub = integrate(i,phi)
        #print(int_sub)
        int_coeff = K_s_dict[i]
        K_ind += int_sub*int_coeff
    K = K_ind.subs(phi,phi_1_sub) - K_ind.subs(phi,phi_2_sub)
    
    return K

def Kel_generate(nodes,roe,c_0,c_1,omega,left=False):
    
    area = get_area(nodes)

    sb_sub = N(nodes[0][0],prec2)
    s3_sub = N(nodes[2][0],prec2)
    phi_1_sub = N(nodes[0][1],prec2)
    phi_2_sub = N(nodes[1][1],prec2)

    m = N((s3_sub - sb_sub)/(sb_sub*(phi_2_sub - phi_1_sub)),prec2)

    b = N(sb_sub - m*sb_sub*phi_1_sub,prec2)
    L_b = N(s3_sub - m*sb_sub*phi_2_sub,prec2)
    print(f'sb_sub: {sb_sub} s3_sub: {s3_sub} phi_1_sub: {phi_1_sub} phi_2_sub: {phi_2_sub}, m: {m} b: {b} L_b: {L_b}')
    NS1 = N1.subs([(sb,sb_sub),(s3,s3_sub),(phi_1,phi_1_sub),(phi_2,phi_2_sub),(A,area)])
    NS2 = N2.subs([(sb,sb_sub),(s3,s3_sub),(phi_1,phi_1_sub),(phi_2,phi_2_sub),(A,area)])
    NS3 = N3.subs([(sb,sb_sub),(s3,s3_sub),(phi_1,phi_1_sub),(phi_2,phi_2_sub),(A,area)])
    NS4 = N4.subs([(sb,sb_sub),(s3,s3_sub),(phi_1,phi_1_sub),(phi_2,phi_2_sub),(A,area)])
    NS5 = N5.subs([(sb,sb_sub),(s3,s3_sub),(phi_1,phi_1_sub),(phi_2,phi_2_sub),(A,area)])
    NS6 = N6.subs([(sb,sb_sub),(s3,s3_sub),(phi_1,phi_1_sub),(phi_2,phi_2_sub),(A,area)])

    #Precision control may need to go here
    trial_psi = NS1*psi_1 + NS2*psi_2 + NS3*psi_3 + NS4*psi_4 + NS5*psi_5 + NS6*psi_6
    #s_diff = diff(trial_psi,s)
    s_diff = diff(NS1*psi_1,s) + diff(NS2*psi_2,s) + diff(NS3*psi_3,s) + diff(NS4*psi_4,s) + diff(NS5*psi_5,s) + diff(NS6*psi_6,s)
    phi_diff = diff(trial_psi,phi)
    Rel = (1/s)*diff((s/roe)*s_diff, s) + (1/s)*diff((1/(s*roe))*phi_diff, phi) + roe*c_1*trial_psi + roe*c_0 - 2*omega

    R1 = NS1*Rel
    R2 = NS2*Rel
    R3 = NS3*Rel
    R4 = NS4*Rel
    R5 = NS5*Rel
    R6 = NS6*Rel

    if left:
        K1 = L_K_gen(R1,sb_sub,phi_1_sub,phi_2_sub,m,L_b)
        K2 = L_K_gen(R2,sb_sub,phi_1_sub,phi_2_sub,m,L_b)
        K3 = L_K_gen(R3,sb_sub,phi_1_sub,phi_2_sub,m,L_b)
        K4 = L_K_gen(R4,sb_sub,phi_1_sub,phi_2_sub,m,L_b)
        K5 = L_K_gen(R5,sb_sub,phi_1_sub,phi_2_sub,m,L_b)
        K6 = L_K_gen(R6,sb_sub,phi_1_sub,phi_2_sub,m,L_b)
    else:
        K1 = K_gen(R1,sb_sub,phi_1_sub,phi_2_sub,m,b)
        K2 = K_gen(R2,sb_sub,phi_1_sub,phi_2_sub,m,b)
        K3 = K_gen(R3,sb_sub,phi_1_sub,phi_2_sub,m,b)
        K4 = K_gen(R4,sb_sub,phi_1_sub,phi_2_sub,m,b)
        K5 = K_gen(R5,sb_sub,phi_1_sub,phi_2_sub,m,b)
        K6 = K_gen(R6,sb_sub,phi_1_sub,phi_2_sub,m,b)

    Ks = [K1,K2,K3,K4,K5,K6]
    psis = [psi_1,psi_2,psi_3,psi_4,psi_5,psi_6]
    Kel = []
    
    for i in range(6):
        Ks[i] = expand(Ks[i])
    for i in range(6):
        poly = []
        for j in range(6):
            subpoly = Poly(Ks[i],psis[j])
            co = subpoly.coeffs()[0]
            poly.append(co)
        Kel.append(poly)
    for i in range(6):
        for j in range(6):
            Kel[i][j] = re(Kel[i][j])
    
    return Kel

#Kel = Kel_generate([[1,2],[1,1],[2,1]],1,1,1,2)
#Kel = Kel_generate(right_list[5][5],1,1,1,2)
#Kel = Kel_generate(left_list[0][10],1,1,1,2,left = True)

def fill_nodes(del_s,del_phi):

    all_nodes = []
    
    for i in range(Ns):
        sub_nodes = []
        for j in range(Nphi):
            loc_jl = [round(i*del_s + s_start,4), round(j*del_phi,4)]
            sub_nodes.append(loc_jl)
        all_nodes.append(sub_nodes)

    return all_nodes

def build_right_element(all_nodes,i,j):

    node_1 = [N(all_nodes[i][j+1][0],prec1),N(all_nodes[i][j+1][1],prec1)]
    node_2 = [N(all_nodes[i][j][0],prec1),N(all_nodes[i][j][1],prec1)]
    node_3 = [N(all_nodes[i+1][j][0],prec1),N(all_nodes[i+1][j][1],prec1)]
    element = [node_1,node_2,node_3]

    return element

def build_left_element(all_nodes,i,j):
    
    node_1 = [N(all_nodes[i][j-1][0],prec1),N(all_nodes[i][j-1][1],prec1)]
    node_2 = [N(all_nodes[i][j][0],prec1),N(all_nodes[i][j][1],prec1)]
    node_3 = [N(all_nodes[i-1][j][0],prec1),N(all_nodes[i-1][j][1],prec1)]
    element = [node_1,node_2,node_3]

    return element


def build_right_list(all_nodes):

    right_elements = []
    for i in range(Ns-1):
        sub_right = []
        for j in range(Nphi-1):
            element = build_right_element(all_nodes,i,j)
            sub_right.append(element)
        right_elements.append(sub_right)
        
    return right_elements

def build_left_list(all_nodes):

    left_elements = []
    for i in range(1,Ns):
        sub_left = []
        for j in range(1,Nphi):
            element = build_left_element(all_nodes,i,j)
            sub_left.append(element)
        left_elements.append(sub_left)
        
    return left_elements


def end_node_number(i_index,j_index):
    node_number = 2*i_index*(2*Nphi-1)+j
    return node_number
def mid_node_number(i_index,j_index):
    node_number = (2*i_index + 1)*(2*Nphi - 1)
    return node_number

def K_generate(left_list,right_list,roe_el):

    K = np.empty([(2*Ns-1)*(2*Nphi-1),(2*Ns-1)*(2*Nphi-1)])

    m = 0
    for i in range(Ns):
        for j in range(Nphi):

            if m % 1 == 0:
                print(f'element: {m}')
            m += 1
                
            Kel = Kel_generate(left_list[i][j],roe_el[i][j],c_0_sub,c_1_sub,omega_sub,left=True)
            K[end_node_number(i+1,2*j)][end_node_number(i+1,2*j)] = Kel[0][0]
            K[end_node_number(i+1,2*j)][end_node_number(i+1,2*j+2)] = Kel[0][1]
            K[end_node_number(i+1,2*j)][end_node_number(i,2*j+2)] = Kel[0][2]
            K[end_node_number(i+1,2*j)][end_node_number(i+1,2*j+1)] = Kel[0][3]
            K[end_node_number(i+1,2*j)][mid_node_number(i,2*j+2)] = Kel[0][4]
            K[end_node_number(i+1,2*j)][mid_node_number(i,2*j+1)] = Kel[0][5]
            
            K[end_node_number(i+1,2*j+2)][end_node_number(i+1,2*j)] = Kel[1][0]
            K[end_node_number(i+1,2*j+2)][end_node_number(i+1,2*j+2)] = Kel[1][1]
            K[end_node_number(i+1,2*j+2)][end_node_number(i,2*j+2)] = Kel[1][2]
            K[end_node_number(i+1,2*j+2)][end_node_number(i+1,2*j+1)] = Kel[1][3]
            K[end_node_number(i+1,2*j+2)][mid_node_number(i,2*j+2)] = Kel[1][4]
            K[end_node_number(i+1,2*j+2)][mid_node_number(i,2*j+1)] = Kel[1][5]

            K[end_node_number(i,2*j+2)][end_node_number(i+1,2*j)] = Kel[2][0]
            K[end_node_number(i,2*j+2)][end_node_number(i+1,2*j+2)] = Kel[2][1]
            K[end_node_number(i,2*j+2)][end_node_number(i,2*j+2)] = Kel[2][2]
            K[end_node_number(i,2*j+2)][end_node_number(i+1,2*j+1)] = Kel[2][3]
            K[end_node_number(i,2*j+2)][mid_node_number(i,2*j+2)] = Kel[2][4]
            K[end_node_number(i,2*j+2)][mid_node_number(i,2*j+1)] = Kel[2][5]

            K[end_node_number(i+1,2*j+1)][end_node_number(i+1,2*j)] = Kel[3][0]
            K[end_node_number(i+1,2*j+1)][end_node_number(i+1,2*j+2)] = Kel[3][1]
            K[end_node_number(i+1,2*j+1)][end_node_number(i,2*j+2)] = Kel[3][2]
            K[end_node_number(i+1,2*j+1)][end_node_number(i+1,2*j+1)] = Kel[3][3]
            K[end_node_number(i+1,2*j+1)][mid_node_number(i,2*j+2)] = Kel[3][4]
            K[end_node_number(i+1,2*j+1)][mid_node_number(i,2*j+1)] = Kel[3][5]

            K[mid_node_number(i,2*j+2)][end_node_number(i+1,2*j)] = Kel[4][0]
            K[mid_node_number(i,2*j+2)][end_node_number(i+1,2*j+2)] = Kel[4][1]
            K[mid_node_number(i,2*j+2)][end_node_number(i,2*j+2)] = Kel[4][2]
            K[mid_node_number(i,2*j+2)][end_node_number(i+1,2*j+1)] = Kel[4][3]
            K[mid_node_number(i,2*j+2)][mid_node_number(i,2*j+2)] = Kel[4][4]
            K[mid_node_number(i,2*j+2)][mid_node_number(i,2*j+1)] = Kel[4][5]

            K[mid_node_number(i,2*j+1)][end_node_number(i+1,2*j)] = Kel[5][0]
            K[mid_node_number(i,2*j+1)][end_node_number(i+1,2*j+2)] = Kel[5][1]
            K[mid_node_number(i,2*j+1)][end_node_number(i,2*j+2)] = Kel[5][2]
            K[mid_node_number(i,2*j+1)][end_node_number(i+1,2*j+1)] = Kel[5][3]
            K[mid_node_number(i,2*j+1)][mid_node_number(i,2*j+2)] = Kel[5][4]
            K[mid_node_number(i,2*j+1)][mid_node_number(i,2*j+1)] = Kel[5][5]

            Kel = Kel_generate(right_list[i][j],roe_el[i][j],c_0_sub,c_1_sub,omega_sub,left=False)
            K[end_node_number(i,2*j+2)][end_node_number(i,2*j+2)] = Kel[0][0]
            K[end_node_number(i,2*j+2)][end_node_number(i,2*j)] = Kel[0][1]
            K[end_node_number(i,2*j+2)][end_node_number(i+1,2*j)] = Kel[0][2]
            K[end_node_number(i,2*j+2)][end_node_number(i,2*j+1)] = Kel[0][3]
            K[end_node_number(i,2*j+2)][mid_node_number(i,2*j)] = Kel[0][4]
            K[end_node_number(i,2*j+2)][mid_node_number(i,2*j+1)] = Kel[0][5]

            K[end_node_number(i,2*j)][end_node_number(i,2*j+2)] = Kel[1][0]
            K[end_node_number(i,2*j)][end_node_number(i,2*j)] = Kel[1][1]
            K[end_node_number(i,2*j)][end_node_number(i+1,2*j)] = Kel[1][2]
            K[end_node_number(i,2*j)][end_node_number(i,2*j+1)] = Kel[1][3]
            K[end_node_number(i,2*j)][mid_node_number(i,2*j)] = Kel[1][4]
            K[end_node_number(i,2*j)][mid_node_number(i,2*j+1)] = Kel[1][5]

            K[end_node_number(i+1,2*j)][end_node_number(i,2*j+2)] = Kel[2][0]
            K[end_node_number(i+1,2*j)][end_node_number(i,2*j)] = Kel[2][1]
            K[end_node_number(i+1,2*j)][end_node_number(i+1,2*j)] = Kel[2][2]
            K[end_node_number(i+1,2*j)][end_node_number(i,2*j+1)] = Kel[2][3]
            K[end_node_number(i+1,2*j)][mid_node_number(i,2*j)] = Kel[2][4]
            K[end_node_number(i+1,2*j)][mid_node_number(i,2*j+1)] = Kel[2][5]

            K[end_node_number(i,2*j+1)][end_node_number(i,2*j+2)] = Kel[3][0]
            K[end_node_number(i,2*j+1)][end_node_number(i,2*j)] = Kel[3][1]
            K[end_node_number(i,2*j+1)][end_node_number(i+1,2*j)] = Kel[3][2]
            K[end_node_number(i,2*j+1)][end_node_number(i,2*j+1)] = Kel[3][3]
            K[end_node_number(i,2*j+1)][mid_node_number(i,2*j)] = Kel[3][4]
            K[end_node_number(i,2*j+1)][mid_node_number(i,2*j+1)] = Kel[3][5]

            K[mid_node_number(i,2*j)][end_node_number(i,2*j+2)] = Kel[4][0]
            K[mid_node_number(i,2*j)][end_node_number(i,2*j)] = Kel[4][1]
            K[mid_node_number(i,2*j)][end_node_number(i+1,2*j)] = Kel[4][2]
            K[mid_node_number(i,2*j)][end_node_number(i,2*j+1)] = Kel[4][3]
            K[mid_node_number(i,2*j)][mid_node_number(i,2*j)] = Kel[4][4]
            K[mid_node_number(i,2*j)][mid_node_number(i,2*j+1)] = Kel[4][5]

            K[mid_node_number(i,2*j+1)][end_node_number(i,2*j+2)] = Kel[5][0]
            K[mid_node_number(i,2*j+1)][end_node_number(i,2*j)] = Kel[5][1]
            K[mid_node_number(i,2*j+1)][end_node_number(i+1,2*j)] = Kel[5][2]
            K[mid_node_number(i,2*j+1)][end_node_number(i,2*j+1)] = Kel[5][3]
            K[mid_node_number(i,2*j+1)][mid_node_number(i,2*j)] = Kel[5][4]
            K[mid_node_number(i,2*j+1)][mid_node_number(i,2*j+1)] = Kel[5][5]

    return K


all_nodes = fill_nodes(del_s,del_phi)
right_list = build_right_list(all_nodes)
left_list = build_left_list(all_nodes)

#roe_el = set_roe_el()
#print('and here')

roe_el = np.empty([Ns,Nphi])
s_range = []
for l in range(Nphi):
    s_range.append(set_s_range(l,c,d))
for j in range(Ns):
    #print('got here')
    for l in range(Nphi):
        if s_start + j*(del_s) < s_start + s_range[l]:
            roe_el[j,l] = 1
        else:
            roe_el[j,l] = 0

#K = K_generate(left_list,right_list,roe_el)


'''
turtle.setup(400,300)
wn = turtle.Screen()
plot = turtle.Turtle()
def plot_point(s,phi):
    plot.penup()
    plot.goto(s*cos(phi),s*sin(phi))
    plot.dot()
    return None

nodes = fill_nodes(del_s,del_phi)

for i in range(len(nodes)):
    for j in range(len(nodes[0])):
        plot_point(nodes[i][j][0],nodes[i][j][1])

nodes = fill_nodes(del_s,del_phi)
xs = []
ys = []
for i in range(len(nodes)):
    for j in range(len(nodes[0])):
        xs.append(nodes[i][j][0]*cos(nodes[i][j][1]))
        ys.append(nodes[i][j][0]*sin(nodes[i][j][1]))
plt.plot(xs,ys,'bo')
plt.show()
'''








