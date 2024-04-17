#92985_tiago_almeida
#!/bin/bash
if [ ! -f pacientes.txt -a ! -f medicos.txt ] ;
then
echo "Não há pacientes nem médicos no sistema!"
elif [ ! -f medicos.txt ] ;
then
echo "Não há médicos no sistema!"
elif [ ! -f pacientes.txt ] ;
then
echo "Não há pacientes no sistema!"
else

num_pacientes_localidade=$(cat pacientes.txt | cut -d ';' -f3 | grep -i "^$1$" | wc -l)

num_med_saldo_superior=0
num_linhas_file=$(cat medicos.txt | wc -l)
i=1
while [ $i -le $num_linhas_file ]; do
conteudo_linha=$(cat medicos.txt | cut -d ';' -f7 | head -$i | tail -1)

if [ $conteudo_linha -gt $2 ] ;
then
num_med_saldo_superior=$(( $num_med_saldo_superior+1 ))
fi
i=$(($i+1))
done

echo "Numero de pacientes de $1: "$num_pacientes_localidade
echo "Numero de médicos com saldo superior a $2: "$num_med_saldo_superior
fi