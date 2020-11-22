// PRIDAT testy function_call SEMANTIKY


        package

        main

func main(){
        a := 3
        if a == 1 {
                for ;; {

                }
        } else {
                for i:= ((())) + "toj" + "tak"; i<10; i = i +1{

                }
        }
        _ = 3
        _ = a
        _ = "abc"
        a = 3
        a = 3 + 3

        a, a = 3, 3 // OTHER ERROR ????

        // a = a + 3

        {}
        // i = 3

        // a = i + 3

        // i = a + 3

        a = 1 +5 * (1 / 5)
        main()  // moze byt volany rekurzivne ????
}
func main(){

}

func maine (i int, b string)(){

        b := "Kurna"
        b = "nyeeee"
        b, i = "precooo", 44
        b = maine(i, b)
        ahoj := a
        id := 5
        ahoj, id = a, b
        return ahoj
        if a {
                j :=1
                if b {
                        ahoj = a
                        return ahoj
                } else{
                }
                return ahoj
        }else{
                //id1 , id2 , id3,_ = a, b,c,d
        }
        //ahoj := b
        ahoj(24)
        ahoj()
        id = 25.3
        return masci,aj
}

func ahoj(){
    //ahoj = a
    //i = 5
    od := (5*8)/0
    id := (od*2)/4+5 != 1 +9
    ny := 5
    yn := 10
    f := 5.5
    s := "Letim jak apac"
    monsignour := "Misko Kmet"
    //ny = len(5)
    //id, od = maine(9,"hnusna skola")
    ny, yn = test_missing_expr_return()
    print(monsignour)
    inputf(25.4)
    ny, f = inputf()
    f, ny = inputf()
    int2float(25, 45)
    f = int2float(77.5)
    ny = float2int(448)
    ny = float2int(xx, f)
    ny = float2int(f)
    monsignour, ny = substr(s, 2, ny)
    ny, yn = ord(s, 4)



        for ;a ;  {
                if a{
                        id = id*id
                        for a:=0; a!= 3;a=a +1{
                                return 7
                        }
                } else {
                        //ahje = a
                        b := 3
                        //b := a
                }
                ahoj()
                dobry_den("ahoj", 24)
                preco(25, 4, 8.5)
                main()
                blablabla()
        }
    bebi := "hello"+"its"-"me"
    id = 5
}

func dobry_den (s string, i int) (int,string){

        dobry_den()
        print("STRNIG =" ,s, "error =", code, "\n")
        print(25, ":", "Ahoj")
        print()         //je to korektne ???
        a := 5
        b := 10
        preco(a, b, 75.42)
        a, s = dobry_den(s, b)
        s, a = dobry_den(s, 25)
        a, s, b = dobry_den(s, 25)
        maine(k, "ahoj")
}

func preco(i int, j int, k float64){

}

func achoj(i int) (string) {

        return 123
}

func achj(a int, b int) (string, float64, int) {

        // Assignment semantics
        a, b = 1, 1
        a, b = "s", 6.5

        a = "neno nepo to kurna"

        // Repete with local defined vars
        c:= 6.5
        d:= "str"

        c, d = 1, 1
        d = 1
        c, d = "s", 6.5

        c = 123

        return "a", 63.5, 5
}

func achjxD() (string, float64, int) {

        return 63.5, 5, "a"
}

func nemas_tu_mat_return_hajzel() {
        // Sprosty had aha
        return 123, 150, "SBS"
}

func nevracias_dobry_typ() (int) {
        return "string"
}

func nevracias_dobry_pocet() (int, string) {
        return 6, "string", "aaaa uz si si myslel xd"
}

func test() (int) {
        i:= 1
        return i
}

func no_nist__malo() (int) {
        // return -
        // return , // Secko syntax error
        return ;
}

func test_missing_expr_return() (int, int) {
        i:= 1
        return , i
}

func test_void_expr_return_morexD() (int, int, string, float64) {
        // return 1, 1, "tak", 6.5
        // return 1, , "", 1.5,
        // //       ^
        // return 1 // insufficient
        // return 1,1,1,1,1,1 // too many
        return 1, 1, 1, // zabudeu posledny
}

func test_for_expressions() {
        test_brace_bug()
        tichobujkkt()

        for ;d < 4; {

        }

        for d:=c; d == 3; d = d+ 1 {

        }

        for d:=1; d<3; d = d + nil { // valid

        }
}

func test_expression_eval(str string, i int, j int, f float64, d float64) {
        i = i +i + 123 + 3

        i = f + f
        i = 1 + f

        f = 6.2 + 3.1
        f = f + d
        f = str + f // COMPATIBILITY ERROR (expression types do not match) before TYPE ERRROR (assignment)
        f = f + str

        str = str + str + str // ok
        str = str + str / str - str // isto nie ok - momentalne nefunguje
}

func test_brace_bug() {
        // POKIAL TO NEVYZERA TAKTO, TAK TO UZ JE V P

        // ****  Syntax error: token type 'BRACKET_LEFT', line 144!  ****
        // ****  Syntax error: token type 'KEYWORD', line 146!  ****
        // ****  Syntax error: token type 'KEYWORD', line 147!  ****
        // ****  Syntax error: token type 'KEYWORD', line 148!  ****
        // ****  Syntax error: token type 'KEYWORD', line 150!  ****
        // ****  Syntax error: token type 'KEYWORD', line 151!  ****
        // ****  Syntax error: token type 'KEYWORD', line 152!  ****
        // ****  Syntax error: token type 'KEYWORD', line 153!  ****

        {}

        i:= 1
        f:= 1.1
        s:= "s"

        i = i+1
        f = f+1.1
        s = s+"s"

        s, s, s = funcafds()
}