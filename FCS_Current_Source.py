print("Importando bibliotecas")
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import configparser

adress = pd.read_csv("FCS.csv",header=None)
local= adress.loc[0][0]

NumberBursts=0
amostras=0
result=pd.DataFrame
dataFrame=list()

config= configparser.ConfigParser()
config.read("config.ini")
FCS_Cycles=config.get('FCS','CYCLES')
amostras=int(config.get('FCS','SAMPLES'))
gain=float(config.get('FCS','CURRENT_GAIN'))
vg_max_criteria=float(config.get('FCS','VG_MAX_CRITERIA'))
vg_min_criteria=float(config.get('FCS','VG_MIN_CRITERIA'))
VG_Config=config.get('FCS','VG_SPAN')
VDS_Config=config.get('FCS','VDS')
Bursts_config=config.get('FCS','BURSTS')
Max_VDS_Config=config.get('FCS','MAX_VDS_EXPECT')
nPontosmedia=int(config.get('FCS','PONTOS_GRUPOS')) 
Freq_Config=config.get('FCS','FREQ')
Flat_Region_width_config=config.get('FCS','FLAT_REGION_WIDHT')




for path, directories, files in os.walk(local):
    for arquivo in files:
        if "results" in arquivo:
            atual= path+ "\\"+"\\" + arquivo
            burst= pd.read_csv(atual, sep=";")
            burst.columns=["time","VDS","ID","VG"]
            burst=burst.astype("Float64")
            burst["ID"]=burst["ID"]*gain
            burst["RES"]=burst["VDS"]/burst["ID"]
            dataFrame.append(burst)   
            NumberBursts=NumberBursts+1  


k=NumberBursts             
result=pd.concat(dataFrame)

############################################
############################################
############################################
############################################

periodos=NumberBursts*int(FCS_Cycles)
aux=amostras/float(FCS_Cycles)

i=np.arange(0,periodos,1)
subida=pd.DataFrame
descida=pd.DataFrame

sublista=list()
desLista=list()
result=result.reset_index()
result=result.drop(columns="index")

for j in i:
    
    min=result.loc[j*aux:(j+1)*aux-1]["VG"].idxmin()
    max=result.loc[j*aux:(j+1)*aux-1]["VG"].idxmax()
    #print(max,min)
    if(max<min):
        delta= ((j+1)*aux -1) - min
        sublista.append(result.loc[j*aux:max-1][["VG","RES","ID"]])
        sublista.append(result.loc[min:min+delta][["VG","RES","ID"]])
        desLista.append(result.loc[max+1:min][["VG","RES","ID"]])
    else:
        delta= ((j+1)*aux -1) - max
        desLista.append(result.loc[j*aux:min-1][["VG","RES","ID"]])
        desLista.append(result.loc[max:max+delta][["VG","RES","ID"]])
        sublista.append(result.loc[min+1:max][["VG","RES","ID"]])

descida=pd.concat(desLista)
subida=pd.concat(sublista)

descida=descida.sort_values(by=["VG"]).reset_index()
subida=subida.sort_values(by=["VG"]).reset_index()
#print(len(descida),len(subida))

fig, ax=plt.subplots()
ax.scatter(subida["VG"],subida["RES"], color= 'red')
ax.scatter(descida["VG"],descida["RES"], color= 'blue')
ax.set_xlabel("Tensão Vg")
ax.set_ylabel("Resistência")
ax.set_title("Resistência vs Vg")

Sub_Final= pd.DataFrame
Des_Final=pd.DataFrame
desLista.clear()
sublista.clear()
#nPontosmedia=int(config.get('FCS','PONTOS_GRUPOS')) #numero de pontos agrupados
Ntotal=amostras*NumberBursts/2
PontosFinais=Ntotal/nPontosmedia

x=np.arange(0,int(PontosFinais),1)


for j in x:
    sublista.append(subida[j*nPontosmedia:(j+1)*nPontosmedia].mean())
    desLista.append(descida[j*nPontosmedia:(j+1)*nPontosmedia].mean())

Sub_Final=pd.concat(sublista)
Des_Final=pd.concat(desLista)

#index = Des_Final["RES"].values.argmax()
#vg_max = Des_Final["VG"][index]*0.80
#vg_min = vg_max - 0.2

deltaR=Sub_Final["RES"].diff()
deltaR[0] = 0
#deltaVG=Sub_Final["VG"].diff()

#Sub_Final["dR/dVg"]=deltaR/deltaVG
#print(Sub_Final["VG"])
#print(Sub_Final["RES"])
#print(deltaR)
#print(deltaVG)
#print(Sub_Final["dR/dVg"])

#media_derivada = deltaR.mean()
#print("media:",media_derivada)
threashold_derivada = deltaR.values.max()*vg_min_criteria
#print("threashold:",threashold_derivada)
index_vg_min = 0
vg_max = 0
vg_min = 0

for i in range(0,len(deltaR)):
    if deltaR[i] > threashold_derivada:
        vg_min = Sub_Final["VG"][i]
        index_vg_min = i
        break

#print("vg_min",vg_min) 
#print("index min",index_vg_min) 
index_max = Sub_Final["RES"].values.argmax()
#print("index max",index_max) 

delta_ref = 100000
delta = 0
vg_max = 0
res_sup = Sub_Final["RES"].values.max()*vg_max_criteria
#print(res_sup)
for i in range(0,len(Sub_Final["RES"])):
    if i < index_max:
        if Sub_Final["RES"][i] > res_sup:
            delta = Sub_Final["RES"][i] - res_sup
        elif res_sup > Sub_Final["RES"][i]:
            delta = res_sup - Sub_Final["RES"][i]
        else:
            vg_max = Sub_Final["VG"][i]
            break
        if delta < delta_ref:
            delta_ref = delta
            vg_max = Sub_Final["VG"][i]

#print(vg_max) 
path = local + "\\vg_values.ini"
content = "[VG]\nMIN=" + str(vg_min) + "\nMAX=" + str(vg_max)

with open(path, 'w') as f:
   f.write(content)

fig, ax=plt.subplots()
ax.plot(Sub_Final["VG"],Sub_Final["RES"], color= 'red', label="Subida")
ax.plot(Des_Final["VG"],Des_Final["RES"], color= 'blue',label="Descida")
#ax.plot(Des_Final["VG"],Sub_Final["dR/dVg"], color= 'black',label="Descida")
#ax.scatter(Sub_Final["VG"],deltaR, color="black",label="dR")
ax.plot(vg_max, 0,  color= 'blue')
ax.axvline(x = vg_max, color = 'green', linestyle=':')
ax.axvline(x = vg_min, color = 'green', linestyle=':')
ax.set_xlabel("Tensão Vg")
ax.set_ylabel("Resistência")
ax.set_title("Resistência vs Vg")
fig_path = local + "/RxVg.jpg"
print(fig_path)
plt.savefig(fig_path)
plt.legend()
plt.show()

teste={
    'Sub_Vg':Sub_Final["VG"].values,
    'Sub_Res':Sub_Final["RES"].values,
    'Des_Vg':Des_Final["VG"].values,
    'Des_Res':Des_Final["RES"]
}
Configuracoes={
    "CYCLES":FCS_Cycles,
    "SAMPLES":amostras,
    "Ganho_Fonte_Corrente":gain,
    "vg_max_criteria":vg_max_criteria,
    "vg_min_criteria":vg_min_criteria,
    "VGs":VG_Config,
    "VDS":VDS_Config,
    "N_BURSTS":Bursts_config,
    "Max_VDS_Expect":Max_VDS_Config,
    "N_Pontos_media":nPontosmedia, 
    "FrequenciaVG":Freq_Config,
    "Flat_Region_width_NAO_USADO":Flat_Region_width_config

}
Info_teste=pd.DataFrame(Configuracoes)
data=pd.DataFrame(teste)
#print(path)
log = "Salvando resultados em: " + path + "\\resistencia" + ".csv"
#print(log)
data.to_csv(path_or_buf=local+ "\\data_fcs" + ".csv", sep=";", index = False)
Info_teste.to_csv(path_or_buf=local+"INFO_TESTE_FCS"+ ".csv",sep=";", index = False)
#os.remove("FCS.csv")