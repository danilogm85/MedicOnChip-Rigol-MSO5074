import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

samples=100000

#lê o arquivo que contém o endereço atual com dados a se processar
adress = pd.read_csv("FCP.csv",header=None)
local= adress.loc[0][0]
print(type(local))
print(local)
print(os.path.isdir(local))
dataFrame=list()

for path, directories, files in os.walk(local):
    print(path)
    print(directories)
    print(files)
    for arquivo in files:
        atual= path+ "\\"+"\\" + arquivo
        burst= pd.read_csv(atual, sep=";")
        burst.columns=["time","VDS","ID","VG"]
        burst=burst.astype("Float64")
        dataFrame.append(burst)

result=pd.DataFrame(0, index=np.arange(0, samples), columns=np.arange(4))
result.columns=["time","VDS","ID","VG"]
i=0
for aux in dataFrame:
    result["time"]=aux["time"]+result["time"]
    result["VDS"]=aux["VDS"]+result["VDS"]
    result["ID"]=aux["ID"]+result["ID"]
    result["VG"]=aux["VG"]+result["VG"]
    i=i+1

result=result/len(dataFrame)
inf=pd.DataFrame
sup=pd.DataFrame
inf=result.loc[0:(samples/2)-1]
sup=result.loc[(samples/2):samples]
sup.reset_index(drop=True, inplace=True)
tempo=inf["time"]
media=(inf["VDS"]+sup["VDS"])/2

#Printar Gráfico
fig, ax=plt.subplots()
ax.plot(tempo, media, color='blue', label="VDS")
ax.set_xlabel("Time (s)")
ax.set_ylabel("VDS (V)")
ax.set_title("FCP")
plt.legend()
plt.show()

os.remove("FCP.csv")
