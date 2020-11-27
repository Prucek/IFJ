package main

func main(){
	foo(0)
}

func foo (i int){
	if i == 0{
		i = 3 // toto nefunguje v generovani syntax chyba, v 3AC sa nachadza LF@i ale mal by sa LF@%param1
	}else{
		i = 2
	}
}