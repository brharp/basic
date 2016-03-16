# basic

## Grammar

  <expression>    := <disjunct> [ OR <disjunct> ]*
  <disjunct>      := <conjunct> [ AND <conjunct> ]*
  <conjunct>      := <negation> [ <relop> <negation> ]*
  <negation>      := [ NOT ] <relation>
  <relation>      := <term> [ <addop> <term> ]*
  <term>          := <signed factor> [ <mulop> <factor> ]*
  <signed factor> := [ <addop> ] <factor>
  <factor>        := <integer> | <variable> | (<expression>)
