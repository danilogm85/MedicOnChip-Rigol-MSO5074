print("Importando bibliotecas")

import pandas as pd
import os
import matplotlib.pyplot as plt
import numpy as np
from sklearn.linear_model import LinearRegression

print("Iniciando calculo de resistências do canal")

#Lê o arquivo que contém o endereço atual de processamento
adress = pd.read_csv("diretorio.csv",header=None)
local= adress.loc[0][0]
print(os.path.isdir(local))
i=0
for path, directories, files in os.walk(local):
    if (i!=0):
        dataFrames=list()
        print(directories)
        for arquivo in files:
            if ".csv" in arquivo:
                atual= path+ "\\"+"\\" + arquivo
                log = "Pegando curva no arquivo: " + atual
                print(log)
                burst= pd.read_csv(atual, sep=";")
                #print(burst.head(5))
                burst.columns=["time","VDS","ID","VG"]
                burst=burst.astype("Float64")
                burst["ID"]=burst["ID"]/5000
                dataFrames.append(burst)
                print("Curvas carregadas")

        result = pd.concat(dataFrames)
        print("Fazendo ajuste linear dos dados")
        model=LinearRegression()
        x= result["ID"].to_numpy().reshape((-1, 1))
        y= result["VDS"].to_numpy()
        model.fit(x,y)
        print("Obtendo coeficiente linear: ")
        a=model.coef_
        print(a)
        b=model.intercept_
        print("Obtendo offset: ")
        print(b)
        print("Plotando curva com o ajuste")
        xx=np.arange(-2e-5,3e-5,1e-6)
        fig, ax=plt.subplots()
        ax.scatter(x,y)
        ax.plot(xx,a*xx+b, color="red")
        ax.set_xlabel("Corrente")
        ax.set_ylabel("Tensão")
        ax.set_title("Resistência do Canal via Regressão Linear: "+str(a)+" Ohms")
        print(path)
        plt.savefig(path)
        print("Mostrando curva")
        plt.show()
        label=["Resistencia","Condutancia","Coef. Linear"]
        values=[a,1/a,b]
        teste={
            'labels':label,
            'valores':values
        }
        data=pd.DataFrame(teste)
        print(path)
        log = "Salvando resultados em: " + path + "\\resistencia" + ".csv"
        print(log)
        data.to_csv(path_or_buf=path+ "\\resistencia" + ".csv", sep=";")           
           
    
    i=i+1

#os.remove("diretorio.csv")
