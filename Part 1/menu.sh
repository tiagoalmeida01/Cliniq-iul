#92985_tiago_almeida
#!/bin/bash
while : 
do
echo " "
echo "-----------Menu-------------"
echo "1. Cria Pacientes"
echo "2. Cria Médico"
echo "3. Stats"
echo "4. Avalia Médicos"
echo "5. Limpar Consola"
echo "0. Sair"
echo " "
echo "Escolha a opção que pretende"
echo -n "--> "
read option

	case $option in 
	1)
	./cria_pacientes.sh
	echo "Done!"
	echo -n
	echo "****************************************************"
	;; 
	2)
	echo "Por favor insira os campos pedidos"
	echo "Insira o nome"
	read nome
	echo "Insira o número de cédula"
	read cedula
	echo "Insira a especialidade"
	read especialidade
	echo "Insira o email"
	read email
	./cria_medico.sh "$nome" "$cedula" "$especialidade" "$email"
	echo -n
	echo "****************************************************"
	;;
	3)
	echo "Por favor insira os campos pedidos"
	echo "Insira a localidade"
	read localidade
	echo "Insira o saldo minimo"
	read saldo_min
	./stats.sh "$localidade" "$saldo_min"
	echo -n
	echo "****************************************************"
	;;	
	4)
	./avalia_medicos.sh
	echo "Done!"
	echo -n
	echo "****************************************************"
	;;
	5)
	clear
	;;
	0)
	echo "Obrigado e volte sempre!"
	echo "****************************************************"
	exit
	;;
	*) #default
	echo "Opção invalida!"
	echo -n
	echo "****************************************************"
	;;
	esac

done