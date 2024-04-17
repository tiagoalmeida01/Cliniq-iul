#92985_tiago_almeida
#!/bin/bash
if [ -f pacientes.txt ]
then
rm pacientes.txt
fi

i=1
saldo=100
while [ $i -le 10 ]; do
id_paciente=$(cat /etc/passwd | grep ^a[1-9] | head | cut -d ':' -f1 | sed 's/a//' | tail +$i | head -1) 
nome_paciente=$(cat /etc/passwd | grep ^a[1-9] | head | cut -d ':' -f5 | sed 's/,,,//' | tail +$i | head -1)
mail_paciente=$(cat /etc/passwd | grep ^a[1-9] | head | cut -d ':' -f1 | sed 's/a//'| awk ' { print $id_paciente"@iscte-iul.pt" } ' | tail +$i | head -1)

echo $id_paciente";"$nome_paciente";;;"$mail_paciente";"$saldo >> pacientes.txt
i=$(($i+1))
done