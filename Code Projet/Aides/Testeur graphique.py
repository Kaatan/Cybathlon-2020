from math import *
from numpy import *
import matplotlib.pyplot as plt


x=15
z=-26
a=35
b=35
X=(b**2-a**2+x**2+z**2)/(2*b)
Y = (a**2-b**2+x**2+z**2)/(2*a)
pi=3.14159



def f(beta):
    return(x*cos(beta)+z*sin(beta)-X)

def g(alpha):
    return(x*cos(alpha)+z*sin(alpha)-Y)

t=[]
res=[]
res2=[]
for i in range(200):
    t.append(-pi + 2*i*pi/200)
    res.append(f(t[i]))
    res2.append(g(t[i]))


gamma = 2.74

alpha0 = arccos((x-a*cos(gamma))/b)
alpha1 = arcsin((z-a*sin(gamma))/b)



beta0 = gamma-alpha0

x_arm = a*cos(alpha0)
x_forearm = b*cos(beta0)

x_whole = x_arm + x_forearm


plt.plot(t, res2, 'r')
#plt.show()

#valeurs : -0.4 et 2.7