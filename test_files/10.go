package main

func main(){
	for ;;{ // uz syntax error
		a := 0
	}
}

// z dis...trofie
func syn_test(){
	a := 
    b := 5 + +
    c := -6
    d := c ++5
    e := (-5)
    f := 5 * 9 ( 5 + 4)
    g := 5 0.6
    h := 5 / 5 ( + 6)
    i := (5 + ) 8
    j := fn()        // chyba bez FUNEXP
    k := 9 + ahoj()  // chyba bez FUNEXP
    l := -40
    m := 36478 ** 2
    q := -5
    // r := 5e // pokazi errory a riadkovanie ale ukaze dobre lex error
    // s := 5e+
    // t := 5e-
// Mali by prejsť bez chyby:
    a1 := 0.0
    c1 := 5.4e12
    d1 := 5e+4
    e1 := 5.8074e-0 // je to chyba ? float - int ?
    aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa := 5
    f1 := 5.6 + 6.8
    g1 := 6 + 5 > 0 // aj toto je asi chyba
    h1 := 55
    i1 := 5 + ( 5 + (5))
    j1 := (45)
    k1 := "ahojky"
    l1 := "ahoj" + " " + "kamarat"
    m1 := ((5))
    n1 := (6 + (5 * 9) * 5) + 1
    int64bit := 4223372036854775807

}