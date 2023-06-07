import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import configparser


adress = pd.read_csv("FCP.csv",header=None,sep=';')
local= adress.loc[0][0]
#DescidaResult=pd.DataFrame(0,index=np.arange(0, 5000), columns=np.arange(2))
#DescidaResult.columns=["index","VDS"]
#SubidaResult=pd.DataFrame(0,index=np.arange(0, 5000), columns=np.arange(2))
#SubidaResult.columns=["index","VDS"]
Dataframe=list()
medicao=pd.DataFrame()
mean=0

config= configparser.ConfigParser()
config.read("C:\\Users\\Mediconchip.DESKTOP-K5I25D1\\Desktop\\Repositório Fabrinni\\MedicOnChip Rigol MSO5074\\config.ini")
gain_CURRENT=float(config.get('FCP','CURRENT_GAIN'))
tensao_CURRENT=float(config.get('FCP','TENSAO_CORRENTE'))
Freq_config=config.get('FCP','FREQ')
NCycles_Config=config.get('FCP','CYCLES')
Max_vds_expect_config=config.get('FCP','MAX_VDS_EXPECT')
Bursts_config=config.get('FCP','BURSTS')
AcquireAVG_Config=config.get('FCP','ACQUIRE_AVG')
corrente=gain_CURRENT*tensao_CURRENT

for path, directories,files  in os.walk(local):
    for arquivo in files:
        if "results" in arquivo:
            atual= path+ "\\"+"\\" + arquivo
            print(atual)
            burst= pd.read_csv(atual, sep=";")
            burst.columns=["time","VDS","ID","VG"]
            burst=burst.astype("Float64")
            samples=len(burst.index)
            burst.columns=["time","VDS","ID","VG"]
            Dataframe.append(burst)
            #deltaVg=burst["VG"].diff()
            ##min=deltaVg[0:int(samples/2)-1].idxmin()
            #if(min<100):
            #    SubidaResult=burst["VDS"][min:min+5000].reset_index()+SubidaResult
            #elif(min>(samples/2)-100):
            #    SubidaResult=burst["VDS"][min-5000:min].reset_index()+SubidaResult
            #else:
            #    SubidaResult=burst["VDS"][min-100:min+4900].reset_index()+SubidaResult
            #if(max<100):
            #    DescidaResult=burst["VDS"][max:max+5000].reset_index()+DescidaResult
            #elif(max>(samples/2)-100):
            #    DescidaResult=burst["VDS"][max-5000:max].reset_index()+DescidaResult
            #else:
            #    DescidaResult=burst["VDS"][max-100:max+4900].reset_index()+DescidaResult
            #mean=mean+1

medicao=pd.concat(Dataframe)
#SubidaResult=SubidaResult/mean
#DescidaResult=DescidaResult/mean
fig, ax=plt.subplots()
#ax.plot(SubidaResult.index, SubidaResult["VDS"], color='blue', label="Subida")
#ax.plot(DescidaResult.index, DescidaResult["VDS"], color='red', label="Descida")
ax.plot(medicao["time"], medicao["VDS"], color='red',label='VDS')
ax.set_xlabel("Time (s)")
ax.set_ylabel("VDS (V)")
ax.set_title("FCP")
plt.legend()
#plt.show()
plt.savefig(path+"\\vds.png")

fig, ax=plt.subplots()
#ax.plot(SubidaResult.index, SubidaResult["VDS"]/corrente, color='blue', label="Subida")
#ax.plot(DescidaResult.index, DescidaResult["VDS"]/corrente, color='red', label="Descida")
ax.plot(medicao["time"], medicao["VDS"]/corrente, color='blue',label='Resistência')
ax.set_xlabel("Time (s)")
ax.set_ylabel("RES (Ohms)")
ax.set_title("FCP-RES")
plt.legend()
plt.savefig(path+"\\res.png")
plt.show()

teste={
    "TEMPO":medicao["time"].values,
    'VDS':medicao["VDS"].values,
    "VG":medicao["VG"].values
}
configuracoes={
    "Ganho_Fonte_corrente":gain_CURRENT,
    "SetPointTensao_Fonte_corrente":tensao_CURRENT,
    "FREQ":Freq_config,
    "Cycles":NCycles_Config,
    "Max_VDS_EXPECT":Max_vds_expect_config,
    "NBursts":Bursts_config,
    "AcquireAVG":AcquireAVG_Config

}
Info_Teste_FCP=pd.DataFrame(configuracoes)
data=pd.DataFrame(teste)
print(path)
log = "Salvando resultados em: " + path + "\\resistencia" + ".csv"
print(log)
data.to_csv(path_or_buf=local+ "\\vds_fcp" + ".csv", sep=";",index=False)
Info_Teste_FCP.to_csv(path_or_buf=local+"INFO_CONFIG_FCP"+".csv",sep=";",index=False)
#remover arquivo com os endereços dos dados FCP
#os.remove("FCP.csv")
