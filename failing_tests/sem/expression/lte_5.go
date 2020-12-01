/*

Interpret error output:
Error at line: 175
Operand stack is empty

----
ERROR: Unexpected exit code of IFJ interpreter. Actual: 56 Expected: [0].

ASSEMBLY LINES 170 - 180:

   170	# EXPR BEGIN
   171	  PUSHS int@5
   172	  PUSHS int@5
   173	  POPS GF@tmp1
   174	  POPS GF@tmp2
   175	  LTS				<--------
   176	  PUSHS GF@tmp1
   177	  PUSHS GF@tmp2
   178	  EQS
   179	  ORS
   180	  POPS GF@expr_result

*/

package main

func main () {
	if (5<=5) {
	} else {
	}
}
