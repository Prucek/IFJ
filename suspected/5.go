/////
///// INVALID ASSEMBLY ERROR
///// LOWEST PRIORITY
/////


package main
func main () {
	a := 30
	print(a)
	for a:=10;a>0;a=a-1 {
		print(a)
		a := 3.14 // @Rucko toto je chyba, iteracna premenna sa moze prekryt
		print(a)
		if a>5.0 {
			print(a)
			a:="hello" //toto ukazuje spravne chybu
			print(a)
		} else {
			print(a)
			a:="world" // aj toto
			print(a)
		}
	}
	print(a)
}
// toto neprejde
// test sa vola exe/basic/shadowing.go - pravdepodobne problem s definovanim lokalnych vars s id rovnakym ako v globalnom/nadradenom scope