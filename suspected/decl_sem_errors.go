package main

func a (){
}

func main () {
	a := 0
	a() // Akoby nic - ma byt sem error
}


func main2 () {
	a := 0
	if 0==0 {
		a:=1 // a already defined error? isto nie!
	}else {
		a:=2
	}
}

func main3 () {
	a:=3.6
	for a:=0;0!=0;{
		a:="help" // a already defined -- Neviem preco nepreslo
	}
}
