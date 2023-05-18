import struct

with open('raw_data0.bin', 'rb') as arquivo:
    conteudo = arquivo.read()

print(type(conteudo))
print(len(conteudo))
print(conteudo[3543])

time = []
vds = []
current = []
vg = []
data = [time,vds,current,vg]

num_elementos = len(conteudo) // 4

# Converte os bytes em valores float
valores_float = struct.unpack('f' * num_elementos, conteudo)

# Exibe os valores float
for i in range (0,len(valores_float)):
    data[i%4].append(valores_float[i])

nome_arquivo = "lista.txt"

# Abre o arquivo em modo de escrita
with open(nome_arquivo, 'w') as arquivo:
    # Escreve cada elemento da lista em uma linha do arquivo
    for elemento in vds:
        arquivo.write(str(elemento) + '\n')