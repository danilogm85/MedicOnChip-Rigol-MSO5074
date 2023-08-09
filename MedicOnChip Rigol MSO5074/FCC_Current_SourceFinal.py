print("Importando bibliotecas")

import pandas as pd
import os
import matplotlib.pyplot as plt
import numpy as np
from sklearn.linear_model import LinearRegression
import configparser

print("Calculando")

config= configparser.ConfigParser()
config.read("config.ini")
gain_CURRENT=float(config.get('FCC','G_TIA'))
Cycles_config=int(config.get('FCC','CYCLES'))
Freq_config=float(config.get('FCC','FREQ'))
Max_Vds_expect_config=float(config.get('FCC','MAX_VDS_EXPECT'))
Burst_config=int(config.get('FCC','BURSTS'))
VG_config=int(config.get('FCC','BURSTS'))
VDS_config=config.get('FCC','VDS_SPAN')
Rlim=config.get('FCC','RG_LIMITS')


#Lê o arquivo que contém o endereço atual de processamento
adress = pd.read_csv("diretorio.csv",header=None)
local= adress.loc[0][0]
i=0
for path, directories, files in os.walk(local):
    print(path)
    if (i!=0):
        dataFrames=list()
        for arquivo in files:
            if "results" in arquivo:
                atual= path+ "\\"+"\\" + arquivo
                burst= pd.read_csv(atual, sep=";")
                burst.columns=["time","VDS","ID","VG"]
                burst=burst.astype("Float64")
                burst["ID"]=burst["ID"]*gain_CURRENT
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
        xx=np.arange(-6e-6,6e-6,0.1e-6)
        fig, ax=plt.subplots()
        ax.scatter(x,y)
        ax.plot(xx,a*xx+b, color="red")
        ax.set_xlabel("Corrente")
        ax.set_ylabel("Tensão")
        ax.set_title("_VG_"+str(result["VG"].mean())+" V "+" Resistência: "+str(a)+" Ohms")
        plt.savefig(path+"\\Visualização Dados Regressão Linear"+"_VG_"+str(result["VG"].mean())+".png")
        if path == local + "vg_0":
            plt.show()
        parametros={
            'Resistencia':a,
            'Condutancia':1/a,
            "VG":result["VG"].mean(),
            "Coef. Linear RES":b
        }
        configuracoes={
            "gain_CURRENT":gain_CURRENT,
            "Cycles_config":Cycles_config,
            "Freq_config":Freq_config,
            "Max_Vds_expect_config":Max_Vds_expect_config,
            "Burst_config":Burst_config,
            "VG_config":VG_config,
            "VDS_config":VDS_config,
            "Rlim":Rlim
        }
        validacao=pd.DataFrame(configuracoes, index=[0])
        validacao.to_csv(path_or_buf=path+ "\\INFO_TESTE_FCC"+".csv", sep=";",index=False)
        data=pd.DataFrame(parametros)        
        #data.to_csv(path_or_buf=path+ "\\resistencia"+"_VG_"+str(result["VG"].mean()) + ".csv", sep=";",index=False)     
        data.to_csv(path_or_buf=path+ "\\resistencia"+".csv", sep=";",index=False)     
        plt.close()         
    
    i=i+1

#os.remove("C:\\Users\\Mediconchip\\Desktop\\diretorio.csv")
