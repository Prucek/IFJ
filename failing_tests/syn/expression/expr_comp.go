/*

Interpret error output:
Error at line: 225
Operand stack is empty

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 56 Expected: [0].

ASSEMBLY LINES 220 - 230
	...
		# EXPR BEGIN
		PUSHS int@1
		PUSHS int@0
		POPS GF@tmp1
		POPS GF@tmp2
		LTS				<-------
		PUSHS GF@tmp1
		PUSHS GF@tmp2
		EQS
		ORS
		POPS GF@expr_result
	...

*/

package main

func main () {
	if 0==0 {
	} else {
	}

	for ;0!=0; {
	}

	if 0<1 {
	} else {
	}

	if 0>1 {
	} else {
	}

	for ;1<=0; {
	}
	for ;0>=1; {
	}
}
