import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

adress = pd.read_csv("FCP.csv",header=None)
local= adress.loc[0][0]
DescidaResult=pd.DataFrame(0,index=np.arange(0, 10100), columns=np.arange(2))
DescidaResult.columns=["index","VDS"]
SubidaResult=pd.DataFrame(0,index=np.arange(0, 10100), columns=np.arange(2))
SubidaResult.columns=["index","VDS"]
mean=0

for path, directories,files  in os.walk(local):
    for arquivo in files:
        if ".csv" in arquivo:
            atual= path+ "\\"+"\\" + arquivo
            print(atual)
            burst= pd.read_csv(atual, sep=";")
            burst.columns=["time","VDS","ID","VG"]
            burst=burst.astype("Float64")
            samples=len(burst.index)
            burst.columns=["time","VDS","ID","VG"]
            #print(burst.head(5))
            deltaVg=burst["VG"].diff()
            max=deltaVg[0:int(samples/2)-1].idxmax()
            min=deltaVg[0:int(samples/2)-1].idxmin()
            print(max)
            print(min)
            if(min<100):
                SubidaResult=burst["VDS"][min:min+10100].reset_index()+SubidaResult
            else:
                SubidaResult=burst["VDS"][min-100:min+10000].reset_index()+SubidaResult
            if(max<100):
                DescidaResult=burst["VDS"][max:max+10100].reset_index()+DescidaResult
            else:
                DescidaResult=burst["VDS"][max-100:max+1000].reset_index()+DescidaResult
            mean=mean+1

SubidaResult=SubidaResult/mean
DescidaResult=DescidaResult/mean
fig, ax=plt.subplots()
ax.plot(SubidaResult.index, SubidaResult["VDS"], color='blue', label="Subida")
ax.plot(DescidaResult.index, DescidaResult["VDS"], color='red', label="Descida")
ax.set_xlabel("Time (s)")
ax.set_ylabel("VDS (V)")
ax.set_title("FCP")
plt.legend()
plt.show()

teste={
    'subida':SubidaResult["VDS"],
    'descida':DescidaResult["VDS"]
}
data=pd.DataFrame(teste)
print(path)
log = "Salvando resultados em: " + path + "\\resistencia" + ".csv"
print(log)
data.to_csv(path_or_buf=local+ "\\vds_fcp" + ".csv", sep=";")

#remover arquivo com os endereços dos dados FCP
os.remove("FCP.csv")
