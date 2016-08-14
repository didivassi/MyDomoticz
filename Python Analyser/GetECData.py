import numpy

import urllib, json
url = "http://192.168.5.100/json.htm?type=graph&sensor=Percentage&idx=30&range=day"
response = urllib.urlopen(url)
data = json.loads(response.read())
resultados = list(data['result'])

valores=[]
for dados in resultados:
    valores.append(dados['v'])
    #print dados['d']
#print valores
#numpy.linalg.lstsq(arrayNumpy['v'], arrayNumpy['d'])
contaResultados=len(valores)
#print contaResultados

xi = numpy.arange(0,contaResultados)
A = numpy.array([ xi, numpy.ones(contaResultados)])
# linearly generated sequence
y = valores
m, c = numpy.linalg.lstsq(A.T,y)[0] # obtaining the parameters
#print m , c

delta5Minutos=m #declive da recta
valorInicialPeriodo=c #inicio em y

delta24Horas=m*contaResultados
deltaPercentual24h=(delta24Horas/valorInicialPeriodo)*100
deltaHora=m*12
valorActualTeorico=valorInicialPeriodo+delta24Horas


if m<0:
    trend='A Descer'
else:
    trend='A subir'

print trend
print 'Valor inicial do periodo',valorInicialPeriodo
print 'Delta em 24h', delta24Horas
print 'Delta percentual em 24h', deltaPercentual24h,'%'
print 'Delta a cada hora',deltaHora
print 'Delta em 5m',delta5Minutos
print 'Valor actual teorico', valorActualTeorico
