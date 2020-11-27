package main

func main(){
	foo(0)
}

func foo (i int){
	if i == 0{
		i = 3
	}else{
		i = 2
	}
}