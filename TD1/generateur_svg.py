import svgwrite
from math import *

#--------------------Forme de Base--------------------#
dessin  = svgwrite.Drawing('TD1/exercice_1.svg', size=(800,600))
triangle=[(0,300),(200,0),(400,300)]
carre=[(-100,-100), (100,-100), (100,100), (-100,100)]

dessin.add(dessin.polygon(carre, fill="#5CF33E", stroke="#000000", opacity=1 ))

dessin.save()

#--------------------Translation de la Forme--------------------#

def translater(input, vectTrans):
  x,y = input
  tx,ty = vectTrans
  return ((x+tx, y+ty))

dessin2 = svgwrite.Drawing('TD1/exercice_carre_trans.svg', size=(800,600))

carre=[(-100,-100), (100,-100), (100,100), (-100,100)]

trans=(150,300)

carre_trans=[translater(sommet,trans) for sommet in carre]

dessin2.add(dessin2.polygon(carre_trans, fill="#5CF33E",stroke="#000000", opacity=1))

dessin2.save()

#--------------------Rotation de la Forme--------------------#

def rotation10(angle):
 return (cos(angle), sin(angle))

def rotation01(angle):
 return (sin(angle), cos(angle))

def rotation(point, angle):
 x, y = point
 v1x, v1y = rotation10(angle)
 v2x, v2y = rotation01(angle)


 out=(x*v1x+y*v1y,-x*v2x+y*v2y)
 return out

dessin3 = svgwrite.Drawing('TD1/exercice_carre_rota.svg', size=(800,600))

carre=[(-100,-100), (100,-100), (100,100), (-100,100)]

angle=pi/12

carre_rota=[translater(rotation(sommet,angle), trans) for sommet in carre]

dessin3.add(dessin3.polygon(carre_rota, fill="#FF87F9",stroke="#000000", opacity=1))

dessin3.save()

#--------------------Matrices 2D--------------------#

def prodMatVect(Mat, Vect ):
       x, y = Vect
       ( (a11,a12), (a21, a22) ) = Mat

       x2 = a11 * x + a12 * y
       y2 = a21 * x + a22 * y

       return (x2,y2)

def Matrotation(angle):
     return ((cos(angle), sin(angle)), (-sin(angle), cos(angle)))

dessin4 = svgwrite.Drawing('TD1/exercice_carre_matrice.svg', size=(800,600))

carre=[(-100,-100), (100,-100), 
       (100,100), (-100,100)]

angle=pi/12

matRot=Matrotation(angle)

carre_out=[translater(prodMatVect(matRot,sommet),trans) for sommet in carre]

dessin4.add(dessin4.polygon(carre_out, fill='#FF0000', stroke="#000000", opacity=1))

dessin4.save()


#--------------------Dilatation Question 3--------------------#

def Matdilatation(coefDilatation):
  return ((coefDilatation,0), (0, coefDilatation))  

dessin5 = svgwrite.Drawing('TD1/question3.svg', size=(800,600))

carre=[(-100,-100), (100,-100), (100,100), (-100,100)]

angle=0

coefDilatation = 1

matRot=Matrotation(angle)

x = (150,300)

opac = 0.2

i = 0
while (i < 6):
       carre_out=[translater(prodMatVect(Matdilatation(coefDilatation),sommet), x) for sommet in carre]
       dessin5.add(dessin5.polygon(carre_out, fill="#15BCDA", stroke="#000000", opacity=opac))
       opac += 0.2
       x = tuple(map(sum, zip(x, (150, 0))))
       coefDilatation += -0.2
       i += 1
dessin5.save()  

#--------------------Dilatation Question 4--------------------#

dessin5 = svgwrite.Drawing('TD1/question4.svg', size=(800,600))

carre=[(-100,-100), (100,-100), (100,100), (-100,100)]

angle=0

coefDilatation = 1

matRot=Matrotation(angle)

x = (150,300)

opac = 0.2

i = 0
while (i < 6):
       carre_out=[translater(prodMatVect(Matdilatation(coefDilatation),sommet), x) for sommet in carre]
       dessin5.add(dessin5.polygon(carre_out, fill="#15BCDA", stroke="#000000", opacity=opac))
       opac += 0.2
       coefDilatation += -0.2
       i += 1
dessin5.save()  

#--------------------Dilatation Question 5--------------------#
def prodMatMat(MatA, MatB ):

   ( (a11,a12), (a21, a22) ) = MatA
   ( (B11,B12), (B21, B22) ) = MatB

   return ( (a11*B11+a12* B21 ,a11*B12+a12*B22 ), (  a21*B11+a22*B21 , a21*B12 +a22*B22  ) )


dessin5q = svgwrite.Drawing('TD1/question5.svg', size=(1000,800))

x_base, y_base = 400, 300
espacement = 60  
angle_increment = -pi/12

for i in range(12):  
    angle = i * angle_increment 
    coefDilatation = 0.8 
    

    matModelisation = prodMatMat(Matdilatation(coefDilatation), Matrotation(angle))
    

    carre_transforme = [prodMatVect(matModelisation, sommet) for sommet in carre]
    

    x_offset = x_base + i * espacement - 300
    y_offset = y_base
    

    carre_final = [translater(sommet, (x_offset, y_offset)) for sommet in carre_transforme]
    
    dessin5q.add(dessin5q.polygon(carre_final, fill='#FF0000', stroke="#000000", opacity=0.7))

dessin5q.save()

#--------------------Triange Question 1--------------------#

aux=100
points=[(aux, aux),(-aux, aux),(-aux, -aux),(aux, -aux)]
triangles=[0,1,2,0,2,3]

dessinT1 = svgwrite.Drawing('TD1/TriangeQ1.svg', size=(1000,800))

i=0
dessinT1.add(dessinT1.polygon((points[triangles[3*i]],points[triangles[3*i+1]],points[triangles[3*i+2]]), fill='blue',  opacity=0.5,stroke='black'))
i=1
dessinT1.add(dessinT1.polygon((points[triangles[3*i]],points[triangles[3*i+1]],points[triangles[3*i+2]]), fill='blue',  opacity=0.5,stroke='black'))

dessinT1.save()

#--------------------Triange Question 2--------------------#

aux=100
points=[(aux, aux),(-aux, aux),(-aux, -aux),(aux, -aux)]
triangles=[0,1,2,0,2,3]

dessinT2 = svgwrite.Drawing('TD1/TriangeQ2.svg', size=(1000,800))

i=0
dessinT2.add(dessinT2.polygon((translater(rotation(points[triangles[3*i]],angle), trans),translater(rotation(points[triangles[3*i+1]],angle), trans),translater(rotation(points[triangles[3*i+2]],angle), trans)), fill='blue',  opacity=0.5,stroke='black'))
i=1
dessinT2.add(dessinT2.polygon((translater(rotation(points[triangles[3*i]],angle), trans),translater(rotation(points[triangles[3*i+1]],angle), trans),translater(rotation(points[triangles[3*i+2]],angle), trans)), fill='blue',  opacity=0.5,stroke='black'))

dessinT2.save()

#--------------------Triange Question 3--------------------#

def compose(points,triangles):
  liste_point=[]
  colors=("blue","red","green","purple","yellow","white","coral","darkblue")
  for i in range(0,len(triangles)//3):
      print(triangles[3*i],triangles[3*i+1],triangles[3*i+2])
      dessinT3.add(dessinT3.polygon((points[triangles[3*i]],points[triangles[3*i+1]],points[triangles[3*i+2]]), fill=colors[i],  opacity=0.5,stroke='black'))

aux=100
points=[(aux, aux),(-aux, aux),(-aux, -aux),(aux, -aux)]
triangles=[0,1,2,0,2,3]

dessinT3 = svgwrite.Drawing('TD1/TriangeQ3.svg', size=(1000,800))

compose(points, triangles)

dessinT3.save()

#--------------------Triange Question 4/5--------------------#
# Question 4
# def compose(points,triangles):
#   liste_point=[]
#   colors=("blue","red","green","purple","yellow","white","coral","darkblue")
#   for i in range(0,len(triangles)//3):
#       print(triangles[3*i],triangles[3*i+1],triangles[3*i+2])
#       dessin.add(dessin.polygon((translater(points[triangles[3*i]], trans),translater(points[triangles[3*i+1]], trans),translater(points[triangles[3*i+2]], trans)), fill=colors[i],  opacity=0.7,stroke='black'))

# Question 5 
def compose(points,triangles):
  liste_point=[]
  colors=("blue","red","green","purple","yellow","white","coral","darkblue")
  for i in range(0,len(triangles)//3):
      print(triangles[3*i],triangles[3*i+1],triangles[3*i+2])
      dessin.add(dessin.polygon((translater(rotation(points[triangles[3*i]], angle), trans),translater(rotation(points[triangles[3*i+1]], angle), trans),translater(rotation(points[triangles[3*i+2]],angle), trans)), fill=colors[i],  opacity=0.7,stroke='black'))


aux=100
points=[(-100, -70),(-100, 0),(-50, 0),(0, -30),(50, 0),(100, -70),(100, 0),(0, 70)]
triangles=[0,1,2,2,3,4,4,5,6,6,7,1]
trans = (200, 300)
angle = -pi/8
dessin = svgwrite.Drawing('TD1/TriangeQ5.svg', size=(800,600))

compose(points, triangles)

dessin.save()

#--------------------Triange Question 6--------------------#

def compose(points,triangles, angle, trans, coefDilatation):
  liste_point=[]
  colors=("blue","red","green","purple","yellow","white","coral","darkblue")
  for i in range(0,len(triangles)//3):
      print(triangles[3*i],triangles[3*i+1],triangles[3*i+2])
      dessin.add(dessin.polygon((translater(rotation(prodMatVect(Matdilatation(coefDilatation),points[triangles[3*i]]), angle), trans),
                           translater(rotation(prodMatVect(Matdilatation(coefDilatation),points[triangles[3*i+1]]), angle), trans),
                           translater(rotation(prodMatVect(Matdilatation(coefDilatation),points[triangles[3*i+2]]),angle), trans)), fill=colors[i],  opacity=0.7,stroke='black'))


aux=100
points=[(-100, -70),(-100, 0),(-50, 0),(0, -30),(50, 0),(100, -70),(100, 0),(0, 70)]
triangles=[0,1,2,2,3,4,4,5,6,6,7,1]
coefDilatation = 1
dessin = svgwrite.Drawing('TD1/TriangeQ6.svg', size=(800,600))



x_base, y_base = 150, 300
espacement = 100 
angle_increment = -pi/3

for i in range(5):  
    angle = i * angle_increment 

    x_offset = x_base + i * espacement
    y_offset = y_base
    coefDilatation -= 0.15
    compose(points, triangles, angle, (x_offset, y_offset), coefDilatation)

dessin.save()


#--------------------3D--------------------#
def compose(points,triangles):
  liste_point=[]
  colors=("purple","coral","blue","red","green","yellow","white","darkblue")
  for i in range(0,len(triangles)//3):
      print(triangles[3*i],triangles[3*i+1],triangles[3*i+2])
      dessinC1.add(dessinC1.polygon((points[triangles[3*i]],points[triangles[3*i+1]],points[triangles[3*i+2]]), fill=colors[(i%(len(colors)*2))//2],  opacity=0.5,stroke='black'))

aux=100
points=[[-aux, -aux, aux], #point 0 (face devant)
        [-aux, aux, aux],#point 1   (face devant)
        [aux, -aux, aux],#point 2   (face devant)
        [aux, aux, aux],#point 3    (face devant)
        [-aux, -aux, -aux],#point 4 (face arrière)
        [-aux, aux, -aux],#point 5 (face arrière)
        [aux, -aux, -aux],#point 6 (face arrière)
        [aux, aux, -aux]#.point 7 (face arrière)
        ]
cube=[0,1,2,   #triangle 1 face 1 (devant)
    1,2,3,     #triangle 2 face 1
    4,5,6,     #triangle 1 face 2 (arriere)
    5,6,7,     #triangle 2 face 2
    0,1,4,     #triangle 1 face 3 (gauche)
    1,4,5,     #triangle 2 face 3
    2,3,6,     #triangle 1 face 4 (droite)
    3,6,7,     #triangle 2 face 4 
    1,3,7,     #triangle 1 face 5 (haut)
    1,5,7,     #triangle 2 face 5
    0,2,4,     #triangle 1 face 6 (bas)
    2,4,6]     #triangle 2 face 6 

dessinC1 = svgwrite.Drawing('TD1/CubeQ1.svg', size=(800,600))

def projection(point3d):
    return((point3d[0],point3d[1]))

points_proj = [ projection(sommet) for sommet in points ]
compose(points_proj,triangles)

dessinC1.save()