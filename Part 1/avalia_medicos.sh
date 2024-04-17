#92985_tiago_almeida
#!/bin/bash
if [ ! -f medicos.txt ] ;
then
echo "Não há médicos registados no sistema!"
else

if [ -f lista_negra_medicos.txt ]
then
rm lista_negra_medicos.txt
fi
touch lista_negra_medicos.txt

num_linhas_file=$(cat medicos.txt | wc -l)
i=1
while [ $i -le $num_linhas_file ]; do
num_consultas=$(cat medicos.txt | cut -d ';' -f6 | tail +$i | head -1 ) 
rating=$(cat medicos.txt | cut -d ';' -f5 | tail +$i | head -1 )
bad_doctor=$(sed -n "$i"p medicos.txt)

if [ $num_consultas -gt 6 -a $rating -lt 5 ] 
then
echo $bad_doctor >> lista_negra_medicos.txt
fi

i=$(($i+1))
done
echo "---------------------------------"
echo "Lista negra de médicos:"
cat lista_negra_medicos.txt
echo "---------------------------------"
fi 