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

trans=(200,200)

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

Matcarre = Matrotation(angle)

carre_matrice=[translater(prodMatVect(Matcarre,sommet), trans) for sommet in carre]

dessin4.add(dessin4.polygon(carre_matrice, fill="#FF0000",stroke="#000000", opacity=1))

dessin4.save()

#--------------------Dilatation--------------------#

dessin5 = svgwrite.Drawing('TD1/exercice_carre_dilatation.svg', size=(800,600))

carre=[(-100,-100), (100,-100), 
       (100,100), (-100,100)]

angle=pi/12

Matcarre = Matrotation(angle)

carre_matrice=[translater(prodMatVect(Matcarre,sommet), trans) for sommet in carre]

dessin5.add(dessin5.polygon(carre_matrice, fill="#FF0000",stroke="#000000", opacity=1))

dessin5.save()