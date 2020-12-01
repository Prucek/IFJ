/////
///// INVALID ASSEMBLY ERROR
///// LOWEST PRIORITY
/////


package main
func main () {
	a := 30
	print(a)
	for a:=10;a>0;a=a-1 {
		print(a) // neprekryje vypise 30
		a := 3.14 // neprekryje
		print(a)
		if a>5.0 {
			print(a)
			a:="hello" // neprekryje
			print(a)
		} else {
			print(a)
			a:="world" // neprekryje
			print(a)
		}
	}
	print(a)
}
// toto neprejde
// test sa vola exe/basic/shadowing.go - pravdepodobne problem s definovanim lokalnych vars s id rovnakym ako v globalnom/nadradenom scope