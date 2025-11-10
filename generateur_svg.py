import svgwrite
dessin  = svgwrite.Drawing('exercice_1.svg', size=(800,600))
triangle=[(0,300),(200,0),(400,300)]

dessin.add(dessin.polygon(triangle, fill="#FF0000",\
                          stroke="#000000", opacity=0.7 ))
dessin.save()