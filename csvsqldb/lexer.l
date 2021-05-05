
%top{
  #include <iostream>
  #include "tokens.h"

#include "base/pragma_push.h"
#include "base/pragma_sign.h"
#include "base/pragma_conversion.h"
}

%{
#include "base/pragma_pop.h"
%}


digit             [0-9]
digit_no_zero     [1-9]
digits            {digit}+

boolean_const     (?i)true|(?i)false|(?i)unknown
char_const        '([^'])'
string_const      '([^']*)'
plus              \+
comment           --([^\r\n]*)
sub               -
dot               \.
real_const        (({digit_no_zero}{digit}*)|0)\.{digits}((E|e)(\+|-)?{digits})?
integer_const     0|{digit_no_zero}{digit}*
equal             =
greater_equal     >=
lesser_equal      <=
not_equal         <>
lesser            <
greater           >
comma             ,
semicolon         ;
asterisk          \*
left_paren        \(
right_paren       \)
div               \/
mod               %
concat            \|\|

union             (?i)union
select            (?i)select
from              (?i)from
where             (?i)where
using             (?i)using
intersect         (?i)intersect
except            (?i)except
as                (?i)as
all               (?i)all
cast              (?i)cast
distinct          (?i)distinct
on                (?i)on
natural           (?i)natural
left              (?i)left
right             (?i)right
inner             (?i)inner
outer             (?i)outer
cross             (?i)cross
full              (?i)full
join              (?i)join
like              (?i)like
and               (?i)and
or                (?i)or
not               (?i)not
is                (?i)is
null              (?i)null
between           (?i)between
in                (?i)in
exists            (?i)exists
group             (?i)group
order             (?i)order
by                (?i)by
collate           (?i)collate
asc               (?i)asc
desc              (?i)desc
having            (?i)having
limit             (?i)limit
offset            (?i)offset
create            (?i)create
table             (?i)table
if                (?i)if
boolean           (?i)boolean
bool              (?i)bool
int               (?i)int
integer           (?i)integer
real              (?i)real
float             (?i)float
double            (?i)double
varchar           (?i)varchar
char              (?i)char
character         (?i)character
date              (?i)date
time              (?i)time
timestamp         (?i)timestamp
constraint        (?i)constraint
primary           (?i)primary
key               (?i)key
unique            (?i)unique
default           (?i)default
check             (?i)check
alter             (?i)alter
column            (?i)column
drop              (?i)drop
add               (?i)add
sum               (?i)sum
count             (?i)count
avg               (?i)avg
max               (?i)max
min               (?i)min
current_date      (?i)current_date
current_time      (?i)current_time
current_timestamp (?i)current_timestamp
varying           (?i)varying
describe          (?i)describe
ast               (?i)ast
extract           (?i)extract
second            (?i)second
minute            (?i)minute
hour              (?i)hour
year              (?i)year
month             (?i)month
day               (?i)day
explain           (?i)explain
show              (?i)show
mapping           (?i)mapping
exec              (?i)exec
arbitrary         (?i)arbitrary

identifier        [_a-zA-Z][_a-zA-Z0-9]*
quoted_identifer  \"([^\"]*)\"

ws                [ \t\n\r]+ 


%%
{integer_const}   {csvsqldb::Token token{csvsqldb::TOK_CONST_INTEGER, {lineno(),columno()}}; token._value = text(); return token;}
{real_const}      {csvsqldb::Token token{csvsqldb::TOK_CONST_REAL, {lineno(),columno()}}; token._value = text(); return token;}
{boolean_const}   {csvsqldb::Token token{csvsqldb::TOK_CONST_BOOLEAN, {lineno(),columno()}}; token._value = text(); return token;}
{char_const}      {csvsqldb::Token token{csvsqldb::TOK_CONST_CHAR, {lineno(),columno()}}; token.setString(text()); return token;}
{string_const}    {csvsqldb::Token token{csvsqldb::TOK_CONST_STRING, {lineno(),columno()}}; token.setString(text()); return token;}
{plus}            {csvsqldb::Token token{csvsqldb::TOK_ADD, {lineno(),columno()}}; return token;}
{comment}         {csvsqldb::Token token{csvsqldb::TOK_COMMENT, {lineno(),columno()}}; return token;}
{sub}             {csvsqldb::Token token{csvsqldb::TOK_SUB, {lineno(),columno()}}; return token;}
{dot}             {csvsqldb::Token token{csvsqldb::TOK_DOT, {lineno(),columno()}}; return token;}
{equal}           {csvsqldb::Token token{csvsqldb::TOK_EQUAL, {lineno(),columno()}}; return token;}
{not_equal}       {csvsqldb::Token token{csvsqldb::TOK_NOTEQUAL, {lineno(),columno()}}; return token;}
{greater_equal}   {csvsqldb::Token token{csvsqldb::TOK_GREATEREQUAL, {lineno(),columno()}}; return token;}
{lesser_equal}    {csvsqldb::Token token{csvsqldb::TOK_SMALLEREQUAL, {lineno(),columno()}}; return token;}
{lesser}          {csvsqldb::Token token{csvsqldb::TOK_SMALLER, {lineno(),columno()}}; return token;}
{greater}         {csvsqldb::Token token{csvsqldb::TOK_GREATER, {lineno(),columno()}}; return token;}
{comma}           {csvsqldb::Token token{csvsqldb::TOK_COMMA, {lineno(),columno()}}; return token;}
{semicolon}       {csvsqldb::Token token{csvsqldb::TOK_SEMICOLON, {lineno(),columno()}}; return token;}
{asterisk}        {csvsqldb::Token token{csvsqldb::TOK_ASTERISK, {lineno(),columno()}}; return token;}
{left_paren}      {csvsqldb::Token token{csvsqldb::TOK_LEFT_PAREN, {lineno(),columno()}}; return token;}
{right_paren}     {csvsqldb::Token token{csvsqldb::TOK_RIGHT_PAREN, {lineno(),columno()}}; return token;}
{div}             {csvsqldb::Token token{csvsqldb::TOK_DIV, {lineno(),columno()}}; return token;}
{mod}             {csvsqldb::Token token{csvsqldb::TOK_MOD, {lineno(),columno()}}; return token;}
{concat}          {csvsqldb::Token token{csvsqldb::TOK_CONCAT, {lineno(),columno()}}; return token;}


{union}           {csvsqldb::Token token{csvsqldb::TOK_UNION, {lineno(),columno()}}; return token;}
{select}          {csvsqldb::Token token{csvsqldb::TOK_SELECT, {lineno(),columno()}}; return token;}
{from}            {csvsqldb::Token token{csvsqldb::TOK_FROM, {lineno(),columno()}}; return token;}
{where}           {csvsqldb::Token token{csvsqldb::TOK_WHERE, {lineno(),columno()}}; return token;}
{using}           {csvsqldb::Token token{csvsqldb::TOK_USING, {lineno(),columno()}}; return token;}
{intersect}       {csvsqldb::Token token{csvsqldb::TOK_INTERSECT, {lineno(),columno()}}; return token;}
{except}          {csvsqldb::Token token{csvsqldb::TOK_EXCEPT, {lineno(),columno()}}; return token;}
{as}              {csvsqldb::Token token{csvsqldb::TOK_AS, {lineno(),columno()}}; return token;}
{all}             {csvsqldb::Token token{csvsqldb::TOK_ALL, {lineno(),columno()}}; return token;}
{cast}            {csvsqldb::Token token{csvsqldb::TOK_CAST, {lineno(),columno()}}; return token;}
{distinct}        {csvsqldb::Token token{csvsqldb::TOK_DISTINCT, {lineno(),columno()}}; return token;}
{on}              {csvsqldb::Token token{csvsqldb::TOK_ON, {lineno(),columno()}}; return token;}
{natural}         {csvsqldb::Token token{csvsqldb::TOK_NATURAL, {lineno(),columno()}}; return token;}
{left}            {csvsqldb::Token token{csvsqldb::TOK_LEFT, {lineno(),columno()}}; return token;}
{right}           {csvsqldb::Token token{csvsqldb::TOK_RIGHT, {lineno(),columno()}}; return token;}
{inner}           {csvsqldb::Token token{csvsqldb::TOK_INNER, {lineno(),columno()}}; return token;}
{outer}           {csvsqldb::Token token{csvsqldb::TOK_OUTER, {lineno(),columno()}}; return token;}
{cross}           {csvsqldb::Token token{csvsqldb::TOK_CROSS, {lineno(),columno()}}; return token;}
{full}            {csvsqldb::Token token{csvsqldb::TOK_FULL, {lineno(),columno()}}; return token;}
{join}            {csvsqldb::Token token{csvsqldb::TOK_JOIN, {lineno(),columno()}}; return token;}
{like}            {csvsqldb::Token token{csvsqldb::TOK_LIKE, {lineno(),columno()}}; return token;}
{and}             {csvsqldb::Token token{csvsqldb::TOK_AND, {lineno(),columno()}}; return token;}
{or}              {csvsqldb::Token token{csvsqldb::TOK_OR, {lineno(),columno()}}; return token;}
{not}             {csvsqldb::Token token{csvsqldb::TOK_NOT, {lineno(),columno()}}; return token;}
{is}              {csvsqldb::Token token{csvsqldb::TOK_IS, {lineno(),columno()}}; return token;}
{null}            {csvsqldb::Token token{csvsqldb::TOK_NULL, {lineno(),columno()}}; return token;}
{between}         {csvsqldb::Token token{csvsqldb::TOK_BETWEEN, {lineno(),columno()}}; return token;}
{in}              {csvsqldb::Token token{csvsqldb::TOK_IN, {lineno(),columno()}}; return token;}
{exists}          {csvsqldb::Token token{csvsqldb::TOK_EXISTS, {lineno(),columno()}}; return token;}
{group}           {csvsqldb::Token token{csvsqldb::TOK_GROUP, {lineno(),columno()}}; return token;}
{order}           {csvsqldb::Token token{csvsqldb::TOK_ORDER, {lineno(),columno()}}; return token;}
{by}              {csvsqldb::Token token{csvsqldb::TOK_BY, {lineno(),columno()}}; return token;}
{collate}         {csvsqldb::Token token{csvsqldb::TOK_COLLATE, {lineno(),columno()}}; return token;}
{asc}             {csvsqldb::Token token{csvsqldb::TOK_ASC, {lineno(),columno()}}; return token;}
{desc}            {csvsqldb::Token token{csvsqldb::TOK_DESC, {lineno(),columno()}}; return token;}
{having}          {csvsqldb::Token token{csvsqldb::TOK_HAVING, {lineno(),columno()}}; return token;}
{limit}           {csvsqldb::Token token{csvsqldb::TOK_LIMIT, {lineno(),columno()}}; return token;}
{offset}          {csvsqldb::Token token{csvsqldb::TOK_OFFSET, {lineno(),columno()}}; return token;}
{create}          {csvsqldb::Token token{csvsqldb::TOK_CREATE, {lineno(),columno()}}; return token;}
{table}           {csvsqldb::Token token{csvsqldb::TOK_TABLE, {lineno(),columno()}}; return token;}
{if}              {csvsqldb::Token token{csvsqldb::TOK_IF, {lineno(),columno()}}; return token;}
{boolean}         {csvsqldb::Token token{csvsqldb::TOK_BOOL, {lineno(),columno()}}; return token;}
{bool}            {csvsqldb::Token token{csvsqldb::TOK_BOOL, {lineno(),columno()}}; return token;}
{int}             {csvsqldb::Token token{csvsqldb::TOK_INT, {lineno(),columno()}}; return token;}
{integer}         {csvsqldb::Token token{csvsqldb::TOK_INT, {lineno(),columno()}}; return token;}
{real}            {csvsqldb::Token token{csvsqldb::TOK_REAL, {lineno(),columno()}}; return token;}
{float}           {csvsqldb::Token token{csvsqldb::TOK_REAL, {lineno(),columno()}}; return token;}
{double}          {csvsqldb::Token token{csvsqldb::TOK_REAL, {lineno(),columno()}}; return token;}
{varchar}         {csvsqldb::Token token{csvsqldb::TOK_STRING, {lineno(),columno()}}; return token;}
{char}            {csvsqldb::Token token{csvsqldb::TOK_CHAR, {lineno(),columno()}}; return token;}
{character}       {csvsqldb::Token token{csvsqldb::TOK_CHAR, {lineno(),columno()}}; return token;}
{date}            {csvsqldb::Token token{csvsqldb::TOK_DATE, {lineno(),columno()}}; return token;}
{time}            {csvsqldb::Token token{csvsqldb::TOK_TIME, {lineno(),columno()}}; return token;}
{timestamp}       {csvsqldb::Token token{csvsqldb::TOK_TIMESTAMP, {lineno(),columno()}}; return token;}
{constraint}      {csvsqldb::Token token{csvsqldb::TOK_CONSTRAINT, {lineno(),columno()}}; return token;}
{primary}         {csvsqldb::Token token{csvsqldb::TOK_PRIMARY, {lineno(),columno()}}; return token;}
{key}             {csvsqldb::Token token{csvsqldb::TOK_KEY, {lineno(),columno()}}; return token;}
{unique}          {csvsqldb::Token token{csvsqldb::TOK_UNIQUE, {lineno(),columno()}}; return token;}
{default}         {csvsqldb::Token token{csvsqldb::TOK_DEFAULT, {lineno(),columno()}}; return token;}
{check}           {csvsqldb::Token token{csvsqldb::TOK_CHECK, {lineno(),columno()}}; return token;}
{alter}           {csvsqldb::Token token{csvsqldb::TOK_ALTER, {lineno(),columno()}}; return token;}
{column}          {csvsqldb::Token token{csvsqldb::TOK_COLUMN, {lineno(),columno()}}; return token;}
{drop}            {csvsqldb::Token token{csvsqldb::TOK_DROP, {lineno(),columno()}}; return token;}
{add}             {csvsqldb::Token token{csvsqldb::TOK_ADD_KEYWORD, {lineno(),columno()}}; return token;}
{sum}             {csvsqldb::Token token{csvsqldb::TOK_SUM, {lineno(),columno()}}; return token;}
{count}           {csvsqldb::Token token{csvsqldb::TOK_COUNT, {lineno(),columno()}}; return token;}
{avg}             {csvsqldb::Token token{csvsqldb::TOK_AVG, {lineno(),columno()}}; return token;}
{max}             {csvsqldb::Token token{csvsqldb::TOK_MAX, {lineno(),columno()}}; return token;}
{min}             {csvsqldb::Token token{csvsqldb::TOK_MIN, {lineno(),columno()}}; return token;}
{current_date}    {csvsqldb::Token token{csvsqldb::TOK_CURRENT_DATE, {lineno(),columno()}}; return token;}
{current_time}    {csvsqldb::Token token{csvsqldb::TOK_CURRENT_TIME, {lineno(),columno()}}; return token;}
{current_timestamp} {csvsqldb::Token token{csvsqldb::TOK_CURRENT_TIMESTAMP, {lineno(),columno()}}; return token;}
{varying}         {csvsqldb::Token token{csvsqldb::TOK_VARYING, {lineno(),columno()}}; return token;}
{describe}        {csvsqldb::Token token{csvsqldb::TOK_DESCRIBE, {lineno(),columno()}}; return token;}
{ast}             {csvsqldb::Token token{csvsqldb::TOK_AST, {lineno(),columno()}}; return token;}
{extract}         {csvsqldb::Token token{csvsqldb::TOK_EXTRACT, {lineno(),columno()}}; return token;}
{second}          {csvsqldb::Token token{csvsqldb::TOK_SECOND, {lineno(),columno()}}; return token;}
{minute}          {csvsqldb::Token token{csvsqldb::TOK_MINUTE, {lineno(),columno()}}; return token;}
{hour}            {csvsqldb::Token token{csvsqldb::TOK_HOUR, {lineno(),columno()}}; return token;}
{year}            {csvsqldb::Token token{csvsqldb::TOK_YEAR, {lineno(),columno()}}; return token;}
{month}           {csvsqldb::Token token{csvsqldb::TOK_MONTH, {lineno(),columno()}}; return token;}
{day}             {csvsqldb::Token token{csvsqldb::TOK_DAY, {lineno(),columno()}}; return token;}
{explain}         {csvsqldb::Token token{csvsqldb::TOK_EXPLAIN, {lineno(),columno()}}; return token;}
{show}            {csvsqldb::Token token{csvsqldb::TOK_SHOW, {lineno(),columno()}}; return token;}
{mapping}         {csvsqldb::Token token{csvsqldb::TOK_MAPPING, {lineno(),columno()}}; return token;}
{exec}            {csvsqldb::Token token{csvsqldb::TOK_EXEC, {lineno(),columno()}}; return token;}
{arbitrary}       {csvsqldb::Token token{csvsqldb::TOK_ARBITRARY, {lineno(),columno()}}; return token;}
{identifier}      {csvsqldb::Token token{csvsqldb::TOK_IDENTIFIER, {lineno(),columno()}}; token.setIdentifier(text()); return token;}
{quoted_identifer} {csvsqldb::Token token{csvsqldb::TOK_QUOTED_IDENTIFIER, {lineno(),columno()}}; token.setQuotedIdentifier(text()); return token;}
{ws}              {continue;}
%%
