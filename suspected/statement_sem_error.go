package main

func main () {
	a := 0
	b:=0

	a,b = 8, 9, 10 // syn error...Ma byt sem error 
}

func main2 () {
	a := 0
	b:=0

	a,b = 8 // Pokazi error system, syntax error - ma byt sem error
}

func main3 () {
	a := 0
	b:=0

	a,b = "help", 9.4 // error code 4 - spravne ?
}

func alpha (a int) (string) {
	return	// Chyba vylucenia void return - treba retest po oprave
}

func main4 () {
	alpha()
}

func main5 () {
	a := _ + 6 // error code 3 - spravne ? - pravdepodobne ma prednost error "_ nemozes citat"
}


func main6 () {
	alpha(_) // error 6 - spravne ? - pravdepodobne to iste
}