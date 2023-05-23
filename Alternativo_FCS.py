import pandas as pd
import os
import matplotlib.pyplot as plt
import numpy as np

adress = pd.read_csv("C:\\Users\Mediconchip\\Desktop\\ALTERNATIVOFCS.csv",header=None)
local= adress.loc[0][0]
dataFrame=pd.DataFrame()
info=list()
FCS=""
i=0

for path, directories, files in os.walk(local):
    if(i==0):
        FCS=path
    for arquivo in files:
        if "resistencia_VG"in arquivo:
            item= pd.read_csv(path+"\\"+arquivo,sep=';')
            info.append(item)
    i=i+1

dataFrame=pd.concat(info)
dataFrame=dataFrame.reset_index()
dataFrame=dataFrame.drop(columns='index')
print(dataFrame)

fig, ax=plt.subplots()
ax.scatter(dataFrame["VG"],dataFrame["Resistencia"],color='red')
ax.set_xlabel("VG [V]")
ax.set_ylabel("Resistência [OHM]")
ax.set_title("Resistência Canal vs VG ")
plt.savefig(FCS+"\\FCS.png")
plt.show()
dataFrame.to_csv(FCS+"\\FCS.csv", index=False,sep=';')