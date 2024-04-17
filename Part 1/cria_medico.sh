#92985_tiago_almeida
#!/bin/bash
nome=$1
num_cedula=$2
especialidade=$3
mail=$4

touch medicos.txt

if [ $(cat medicos.txt | cut -d ';' -f2 | grep -w $num_cedula | wc -l) -eq 0 ]; 
then
echo "$nome;$num_cedula;$especialidade;$mail;0;0;0" >> medicos.txt
else
echo "*********Médico já inscrito!*********"
fi
echo "--------------------------------------------------"
cat medicos.txt