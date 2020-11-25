package main

func foo(){

	for i := 0; i < 5; i = i+1{
		if (i > 0 && i < 5) {
			print(":)")
		} else{
			print("Chyba jak svina")
		}
	}
}

func main(){

	foo()
}