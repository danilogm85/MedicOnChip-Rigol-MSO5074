import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

adress = pd.read_csv("FCS.csv",header=None)
local= adress.loc[0][0]

i=0
result=pd.DataFrame
dataFrame=list()

for path, directories, files in os.walk(local):

    for arquivo in files:
        if "vg_values" not in arquivo:
            atual= path+ "\\"+"\\" + arquivo
            burst= pd.read_csv(atual, sep=";")
            burst.columns=["time","VDS","ID","VG"]
            burst=burst.astype("Float64")
            burst["ID"]=burst["ID"]/5000 
            burst["RES"]=burst["VDS"]/burst["ID"]
            dataFrame.append(burst)   
            i=i+1  


k=i             
result=pd.concat(dataFrame)
os.remove("FCS.csv")

############################################
############################################
############################################
############################################

periodos=i*20
aux=100
#print(aux)

i=np.arange(0,periodos,1)
subida=pd.DataFrame
descida=pd.DataFrame

sublista=list()
desLista=list()
result=result.reset_index()


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

#print(len(descida),len(subida))

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
nPontosmedia=100 #numero de pontos agrupados
Ntotal=2000*k/2
PontosFinais=Ntotal/nPontosmedia
#print(PontosFinais)
x=np.arange(0,int(PontosFinais),1)
#print(x)

for j in x:
    sublista.append(subida[j*nPontosmedia:(j+1)*nPontosmedia].mean())
    desLista.append(descida[j*nPontosmedia:(j+1)*nPontosmedia].mean())

Sub_Final=pd.concat(sublista)
Des_Final=pd.concat(desLista)

index = Des_Final["RES"].values.argmax()
vg_max = Des_Final["VG"][index]*0.80
vg_min = vg_max - 0.2

path = local + "vg_values.ini"
content = "[VG]\nMIN=" + str(vg_min) + "\nMAX=" + str(vg_max)

with open(path, 'w') as f:
    f.write(content)

fig, ax=plt.subplots()
ax.plot(Sub_Final["VG"],Sub_Final["RES"], color= 'red', label="Subida")
ax.plot(Des_Final["VG"],Des_Final["RES"], color= 'blue',label="Descida")
ax.plot(vg_max, 0,  color= 'blue')
ax.axvline(x = vg_max, color = 'green', linestyle=':')
ax.axvline(x = vg_min, color = 'green', linestyle=':')
ax.set_xlabel("Tensão Vg")
ax.set_ylabel("Resistência")
ax.set_title("Resistência vs Vg")
fig_path = local + "ResxVg"
plt.savefig(local)
plt.legend()
plt.show()
