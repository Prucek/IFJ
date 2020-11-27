package main

func foo(){

	for i := 1; i < 5; i = i+1{
		if (i > 0) {
			print(":)\n")
		} else{
			print("Chyba jak svina\n")
		}
	}
}

func main(){

	foo()
}