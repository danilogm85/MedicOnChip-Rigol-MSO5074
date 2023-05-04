import pandas as pd
import os
import matplotlib.pyplot as plt
import numpy as np
from sklearn.linear_model import LinearRegression


#Lê o arquivo que contém o endereço atual de processamento
adress = pd.read_csv("C:\\Users\\Mediconchip\\Desktop\\diretorio.csv",header=None)
local= adress.loc[0][0]
i=0
for path, directories, files in os.walk(local):

    if (i!=0):
        dataFrames=list()
        print(directories)
        for arquivo in files:
            atual= path+ "\\"+"\\" + arquivo
            burst= pd.read_csv(atual, sep=";")
            #print(burst.head(5))
            burst.columns=["time","VDS","ID","VG"]
            burst=burst.astype("Float64")
            burst["ID"]=burst["ID"]/5000
            dataFrames.append(burst)

        result = pd.concat(dataFrames)
        model=LinearRegression()
        x= result["ID"].to_numpy().reshape((-1, 1))
        y= result["VDS"].to_numpy()
        model.fit(x,y)
        a=model.coef_
        print(a)
        b=model.intercept_
        print(b)
        xx=np.arange(-2e-5,3e-5,1e-6)
        fig, ax=plt.subplots()
        ax.scatter(x,y)
        ax.plot(xx,a*xx+b, color="red")
        ax.set_xlabel("Corrente")
        ax.set_ylabel("Tensão")
        ax.set_title("Resistência do Canal via Regressão Linear: "+str(a)+" Ohms")
        plt.savefig(path)
        plt.show()
        label=["Resistencia","Condutancia","Coef. Linear"]
        values=[a,1/a,b]
        teste={
            'labels':label,
            'valores':values
        }
        data=pd.DataFrame(teste)
        print(path)
        data.to_csv(path_or_buf=path+ "\\resistencia" + ".csv", sep=";")           
           
    
    i=i+1

os.remove("C:\\Users\\Mediconchip\\Desktop\\diretorio.csv")