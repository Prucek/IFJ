
package main


// Neprechadza dalsich par testov s rovnakymi operandmi, no rozdielnymi logickymi operatormi

func main () {
	if (0>=(5==5)) { // testsuite.py neprechadza
	} else {
	}
}

func main1 () {
	if (0<(5==5)) { // testsuite.py neprechadza
	} else {
	}
}


func main2 () {
	if ("help">="pleh") { // error code 5 ? spravne ?
	} else {
	}
}

func main () {
	if ("help"<"pleh") { // error code 5 ? spravne ?
	} else {
	}
}


