/*


*/package main/*



*/ func main(){
	i := 0
	j := 0
	res := 0
	for ; i < 20; i = i+1 {
		for ; j < 20; j= j+1 {
			res = i*j
			print(i," * ",j," = ",res,"\n")
		}
	}
}