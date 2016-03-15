# basic

## Grammar

  <expression>    := <disjunct> [ OR <disjunct> ]*
  <disjunct>      := <conjunct> [ AND <conjunct> ]*
  <conjunct>      := <relation> [ <relop> <relation> ]*
  <relation>      := <term> [ <addop> <term> ]*
  <term>          := <signed factor> [ <mulop> <factor> ]*
  <signed factor> := [ <addop> ] <factor>
  <factor>        := <integer> | <variable> | (<expression>)
