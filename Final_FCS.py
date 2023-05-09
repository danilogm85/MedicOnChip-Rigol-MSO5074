import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

adress = pd.read_csv("C:\\Users\\Mediconchip\\Desktop\\FCS.csv",header=None)
local= adress.loc[0][0]


i=0
result=pd.DataFrame
dataFrame=list()
print("cavalo")
for path, directories, files in os.walk(local):

    for arquivo in files:
        atual= path+ "\\"+"\\" + arquivo
        burst= pd.read_csv(atual, sep=";")
        burst.columns=["time","VDS","ID","VG"]
        burst=burst.astype("Float64")
        burst["ID"]=burst["ID"]/5000 
        burst["RES"]=burst["VDS"]/burst["ID"]
        dataFrame.append(burst)     
              
result=pd.concat(dataFrame)
os.remove("C:\\Users\\Mediconchip\\Desktop\\FCS.csv")

############################################
############################################
############################################
############################################

periodos=40
aux=100
#print(aux)

i=np.arange(0,periodos,1)
subida=pd.DataFrame
descida=pd.DataFrame
print(i)
sublista=list()
desLista=list()
result=result.reset_index()
print(result.index)

for j in i:
    min=result.loc[j*100:(j+1)*aux-1]["VG"].idxmin()
    max=result.loc[j*100:(j+1)*aux-1]["VG"].idxmax()
    delta= ((j+1)*aux -1) - min
    sublista.append(result.loc[j*100:max-1][["VG","RES","ID"]])
    sublista.append(result.loc[min:min+delta][["VG","RES","ID"]])
    desLista.append(result.loc[max+1:min][["VG","RES","ID"]])



descida=pd.concat(desLista)
subida=pd.concat(sublista)

descida=descida.sort_values(by=["VG"])
subida=subida.sort_values(by=["VG"])

print(len(descida),len(subida))
#print(descida[0:3].mean())


fig, ax=plt.subplots()
ax.scatter(subida["VG"],subida["RES"], color= 'red')
ax.scatter(descida["VG"],descida["RES"], color= 'blue')
ax.set_xlabel("Tensão Vg")
ax.set_ylabel("Resistência")
ax.set_title("Resistência vs Vg")
plt.show()


Sub_Final= pd.DataFrame
Des_Final=pd.DataFrame
desLista.clear()
sublista.clear()

i=np.arange(0,20,1)
print(i)

for j in i:
    sublista.append(subida[j*100:(j+1)*100].mean())
    desLista.append(descida[j*100:(j+1)*100].mean())

Sub_Final=pd.concat(sublista)
Des_Final=pd.concat(desLista)


fig, ax=plt.subplots()
ax.plot(Sub_Final["VG"],Sub_Final["RES"], color= 'red', label="Subida")
ax.plot(Des_Final["VG"],Des_Final["RES"], color= 'blue',label="Descida")
ax.set_xlabel("Tensão Vg")
ax.set_ylabel("Resistência")
ax.set_title("Resistência vs Vg")
plt.legend()
plt.show()
