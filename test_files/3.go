/*	
 *	• func int2float(i int) (float64) – Vrací hodnotu celočíselného parametru i převedenou na desetinnou hodnotu.
 *	• func float2int(f float64) (int) – Vrací hodnotu desetinného parametru f převedenou na celočíselnou hodnotu oříznutím desetinné části.
 *	Vestavěné funkce pro práci s řetězci: Případná druhá návratová hodnota je věnována příznaku chyby, kdy 0 znamená bezchybné provedení příkazu.
 *	• func len(𝑠 string) (int) – Vrátí délku (počet znaků) řetězce zadaného jediným
 *	parametrem 𝑠. Např. len("x\nz") vrací 3.
 * 	• func substr(s string, i int, n int) (string, int) – Vrátí podřetězec
 *	zadaného řetězce 𝑠. Druhým parametrem 𝑖 je dán začátek požadovaného podřetězce
 * 	(počítáno od nuly) a třetí parametr 𝑛 určuje délku podřetězce. Je-li index 𝑖 mimo
 *	meze 0 až len(s) nebo 𝑛 < 0, vrací funkce v příznaku chyby hodnotu 1. Je-li 𝑛 >
 *	len(s)−𝑖, jsou jako řetězec vráceny od 𝑖-tého znaku všechny zbývající znaky řetězce
 *	𝑠.
 *	• func ord(s string, i int) (int, int) – Vrátí ordinální hodnotu (ASCII)
 *	znaku na pozici 𝑖 v řetězci 𝑠. Je-li index 𝑖 mimo meze řetězce (0 až len(s) - 1), vrací
 *	funkce v příznaku chyby hodnotu 1.
 *	• func chr(i int) (string, int) – Vrátí jednoznakový řetězec se znakem, jehož
 *	ASCII kód je zadán parametrem 𝑖. Případ, kdy je 𝑖 mimo interval [0; 255], vede na
 *	hodnotu 1 v příznaku chyby
 */
package main

func g(){
	s := "\"Kurna to je parnik\"" //20
	bohus := ": rekl Bohus a hlasite si odfrkl"//32
	s = s + bohus
	i := 0
	i = len(s) //52
	str := ""
	str, _ = substr(s,27,5) 
	i, _ = ord(str,0)
	s = s + str
}

func main(){
	f := 3.14
	i := 0
	i = float2int(f)
	f = int2float(i)
	g()
	return
}