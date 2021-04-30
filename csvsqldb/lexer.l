
%top{
  #include <iostream>
  #include "tokens.h"
}

digit           [0-9]
digit_no_zero   [1-9]
digits          {digit}+

boolean_const       (?i)true|(?i)false|(?i)unknown
char_const          '([^'])'
string_const        '([^']*)'
plus                \+
comment             --([^\r\n]*)
sub                 -
dot                 \.
real_const          (({digit_no_zero}{digit}*)|0)\.{digits}((E|e)(\+|-)?{digits})?
integer_const       0|[1-9]\d*
equal               =
greater_equal       >=
lesser_equal        <=
not_equal           <>
lesser              <
greater             >
comma               ,
semicolon           ;
asterisk            \*
left_paren          \(
right_paren         \)
div                 \/
mod                 %
concat              \|\|

union               (?i)union
select              (?i)select
from                (?i)from
where               (?i)where
using               (?i)using
intersect           (?i)intersect
except              (?i)except
as                  (?i)as
all						(?i)all
cast						(?i)cast
distinct						(?i)distinct
on						(?i)on
natural						(?i)natural
left						(?i)left
right						(?i)right
inner						(?i)inner
outer						(?i)outer
cross						(?i)cross
full						(?i)full
join						(?i)join
like						(?i)like
and						(?i)and
or						(?i)or
not						(?i)not
is						(?i)is
null						(?i)null
between						(?i)between
in						(?i)in
exists						(?i)exists
group						(?i)group
order						(?i)order
by						(?i)by
collate						(?i)collate
asc						(?i)asc
desc						(?i)desc
having						(?i)having
limit						(?i)limit
offset						(?i)offset
create						(?i)create
table						(?i)table
if						(?i)if
boolean						(?i)boolean
bool						(?i)bool
int						(?i)int
integer						(?i)integer
real						(?i)real
float						(?i)float
double						(?i)double
varchar						(?i)varchar
char						(?i)char
character						(?i)character
date						(?i)date
time						(?i)time
timestamp						(?i)timestamp
constraint						(?i)constraint
primary						(?i)primary
key						(?i)key
unique						(?i)unique
default						(?i)default
check						(?i)check
alter						(?i)alter
column						(?i)column
drop						(?i)drop
add						(?i)add
sum						(?i)sum
count						(?i)count
avg						(?i)avg
max						(?i)max
min						(?i)min
current_date						(?i)current_date
current_time						(?i)current_time
current_timestamp						(?i)current_timestamp
varying						(?i)varying
describe						(?i)describe
ast						(?i)ast
extract						(?i)extract
second						(?i)second
minute						(?i)minute
hour						(?i)hour
year						(?i)year
month						(?i)month
day						(?i)day
explain						(?i)explain
show						(?i)show
mapping						(?i)mapping
exec						(?i)exec
arbitrary						(?i)arbitrary

identifier          [_a-zA-Z][_a-zA-Z0-9]*
quoted_identifer    \"([^\"]*)\"


%%
{integer_const}   {csvsqldb::Token token{csvsqldb::TOK_CONST_INTEGER}; token._value = text(); return token;}
{real_const}      {csvsqldb::Token token{csvsqldb::TOK_CONST_REAL}; token._value = text(); return token;}
{boolean_const}   {csvsqldb::Token token{csvsqldb::TOK_CONST_BOOLEAN}; token._value = text(); return token;}
{char_const}      {csvsqldb::Token token{csvsqldb::TOK_CONST_CHAR}; token.setString(text()); return token;}
{string_const}    {csvsqldb::Token token{csvsqldb::TOK_CONST_STRING}; token.setString(text()); return token;}
{plus}       {csvsqldb::Token token{csvsqldb::TOK_ADD}; return token;}
{comment}   {csvsqldb::Token token{csvsqldb::TOK_COMMENT}; return token;}
{sub}       {csvsqldb::Token token{csvsqldb::TOK_SUB}; return token;}
{dot}       {csvsqldb::Token token{csvsqldb::TOK_DOT}; return token;}
{equal}        {csvsqldb::Token token{csvsqldb::TOK_EQUAL}; return token;}
{not_equal}        {csvsqldb::Token token{csvsqldb::TOK_NOTEQUAL}; return token;}
{greater_equal}        {csvsqldb::Token token{csvsqldb::TOK_GREATEREQUAL}; return token;}
{lesser_equal}        {csvsqldb::Token token{csvsqldb::TOK_SMALLEREQUAL}; return token;}
{lesser}        {csvsqldb::Token token{csvsqldb::TOK_SMALLER}; return token;}
{greater}        {csvsqldb::Token token{csvsqldb::TOK_GREATER}; return token;}
{comma}        {csvsqldb::Token token{csvsqldb::TOK_COMMA}; return token;}
{semicolon}        {csvsqldb::Token token{csvsqldb::TOK_SEMICOLON}; return token;}
{asterisk}        {csvsqldb::Token token{csvsqldb::TOK_ASTERISK}; return token;}
{left_paren}        {csvsqldb::Token token{csvsqldb::TOK_LEFT_PAREN}; return token;}
{right_paren}        {csvsqldb::Token token{csvsqldb::TOK_RIGHT_PAREN}; return token;}
{div}        {csvsqldb::Token token{csvsqldb::TOK_DIV}; return token;}
{mod}        {csvsqldb::Token token{csvsqldb::TOK_MOD}; return token;}
{concat}        {csvsqldb::Token token{csvsqldb::TOK_CONCAT}; return token;}


{union}			{csvsqldb::Token token{csvsqldb::TOK_UNION}; return token;}
{select}			{csvsqldb::Token token{csvsqldb::TOK_SELECT}; return token;}
{from}			{csvsqldb::Token token{csvsqldb::TOK_FROM}; return token;}
{where}			{csvsqldb::Token token{csvsqldb::TOK_WHERE}; return token;}
{using}			{csvsqldb::Token token{csvsqldb::TOK_USING}; return token;}
{intersect}			{csvsqldb::Token token{csvsqldb::TOK_INTERSECT}; return token;}
{except}			{csvsqldb::Token token{csvsqldb::TOK_EXCEPT}; return token;}
{as}			{csvsqldb::Token token{csvsqldb::TOK_AS}; return token;}
{all}			{csvsqldb::Token token{csvsqldb::TOK_ALL}; return token;}
{cast}			{csvsqldb::Token token{csvsqldb::TOK_CAST}; return token;}
{distinct}			{csvsqldb::Token token{csvsqldb::TOK_DISTINCT}; return token;}
{on}			{csvsqldb::Token token{csvsqldb::TOK_ON}; return token;}
{natural}			{csvsqldb::Token token{csvsqldb::TOK_NATURAL}; return token;}
{left}			{csvsqldb::Token token{csvsqldb::TOK_LEFT}; return token;}
{right}			{csvsqldb::Token token{csvsqldb::TOK_RIGHT}; return token;}
{inner}			{csvsqldb::Token token{csvsqldb::TOK_INNER}; return token;}
{outer}			{csvsqldb::Token token{csvsqldb::TOK_OUTER}; return token;}
{cross}			{csvsqldb::Token token{csvsqldb::TOK_CROSS}; return token;}
{full}			{csvsqldb::Token token{csvsqldb::TOK_FULL}; return token;}
{join}			{csvsqldb::Token token{csvsqldb::TOK_JOIN}; return token;}
{like}			{csvsqldb::Token token{csvsqldb::TOK_LIKE}; return token;}
{and}			{csvsqldb::Token token{csvsqldb::TOK_AND}; return token;}
{or}			{csvsqldb::Token token{csvsqldb::TOK_OR}; return token;}
{not}			{csvsqldb::Token token{csvsqldb::TOK_NOT}; return token;}
{is}			{csvsqldb::Token token{csvsqldb::TOK_IS}; return token;}
{null}			{csvsqldb::Token token{csvsqldb::TOK_NULL}; return token;}
{between}			{csvsqldb::Token token{csvsqldb::TOK_BETWEEN}; return token;}
{in}			{csvsqldb::Token token{csvsqldb::TOK_IN}; return token;}
{exists}			{csvsqldb::Token token{csvsqldb::TOK_EXISTS}; return token;}
{group}			{csvsqldb::Token token{csvsqldb::TOK_GROUP}; return token;}
{order}			{csvsqldb::Token token{csvsqldb::TOK_ORDER}; return token;}
{by}			{csvsqldb::Token token{csvsqldb::TOK_BY}; return token;}
{collate}			{csvsqldb::Token token{csvsqldb::TOK_COLLATE}; return token;}
{asc}			{csvsqldb::Token token{csvsqldb::TOK_ASC}; return token;}
{desc}			{csvsqldb::Token token{csvsqldb::TOK_DESC}; return token;}
{having}			{csvsqldb::Token token{csvsqldb::TOK_HAVING}; return token;}
{limit}			{csvsqldb::Token token{csvsqldb::TOK_LIMIT}; return token;}
{offset}			{csvsqldb::Token token{csvsqldb::TOK_OFFSET}; return token;}
{create}			{csvsqldb::Token token{csvsqldb::TOK_CREATE}; return token;}
{table}			{csvsqldb::Token token{csvsqldb::TOK_TABLE}; return token;}
{if}			{csvsqldb::Token token{csvsqldb::TOK_IF}; return token;}
{boolean}			{csvsqldb::Token token{csvsqldb::TOK_BOOL}; return token;}
{bool}			{csvsqldb::Token token{csvsqldb::TOK_BOOL}; return token;}
{int}			{csvsqldb::Token token{csvsqldb::TOK_INT}; return token;}
{integer}			{csvsqldb::Token token{csvsqldb::TOK_INT}; return token;}
{real}			{csvsqldb::Token token{csvsqldb::TOK_REAL}; return token;}
{float}			{csvsqldb::Token token{csvsqldb::TOK_REAL}; return token;}
{double}			{csvsqldb::Token token{csvsqldb::TOK_REAL}; return token;}
{varchar}			{csvsqldb::Token token{csvsqldb::TOK_STRING}; return token;}
{char}			{csvsqldb::Token token{csvsqldb::TOK_CHAR}; return token;}
{character}			{csvsqldb::Token token{csvsqldb::TOK_CHAR}; return token;}
{date}			{csvsqldb::Token token{csvsqldb::TOK_DATE}; return token;}
{time}			{csvsqldb::Token token{csvsqldb::TOK_TIME}; return token;}
{timestamp}			{csvsqldb::Token token{csvsqldb::TOK_TIMESTAMP}; return token;}
{constraint}			{csvsqldb::Token token{csvsqldb::TOK_CONSTRAINT}; return token;}
{primary}			{csvsqldb::Token token{csvsqldb::TOK_PRIMARY}; return token;}
{key}			{csvsqldb::Token token{csvsqldb::TOK_KEY}; return token;}
{unique}			{csvsqldb::Token token{csvsqldb::TOK_UNIQUE}; return token;}
{default}			{csvsqldb::Token token{csvsqldb::TOK_DEFAULT}; return token;}
{check}			{csvsqldb::Token token{csvsqldb::TOK_CHECK}; return token;}
{alter}			{csvsqldb::Token token{csvsqldb::TOK_ALTER}; return token;}
{column}			{csvsqldb::Token token{csvsqldb::TOK_COLUMN}; return token;}
{drop}			{csvsqldb::Token token{csvsqldb::TOK_DROP}; return token;}
{add}			{csvsqldb::Token token{csvsqldb::TOK_ADD_KEYWORD}; return token;}
{sum}			{csvsqldb::Token token{csvsqldb::TOK_SUM}; return token;}
{count}			{csvsqldb::Token token{csvsqldb::TOK_COUNT}; return token;}
{avg}			{csvsqldb::Token token{csvsqldb::TOK_AVG}; return token;}
{max}			{csvsqldb::Token token{csvsqldb::TOK_MAX}; return token;}
{min}			{csvsqldb::Token token{csvsqldb::TOK_MIN}; return token;}
{current_date}			{csvsqldb::Token token{csvsqldb::TOK_CURRENT_DATE}; return token;}
{current_time}			{csvsqldb::Token token{csvsqldb::TOK_CURRENT_TIME}; return token;}
{current_timestamp}			{csvsqldb::Token token{csvsqldb::TOK_CURRENT_TIMESTAMP}; return token;}
{varying}			{csvsqldb::Token token{csvsqldb::TOK_VARYING}; return token;}
{describe}			{csvsqldb::Token token{csvsqldb::TOK_DESCRIBE}; return token;}
{ast}			{csvsqldb::Token token{csvsqldb::TOK_AST}; return token;}
{extract}			{csvsqldb::Token token{csvsqldb::TOK_EXTRACT}; return token;}
{second}			{csvsqldb::Token token{csvsqldb::TOK_SECOND}; return token;}
{minute}			{csvsqldb::Token token{csvsqldb::TOK_MINUTE}; return token;}
{hour}			{csvsqldb::Token token{csvsqldb::TOK_HOUR}; return token;}
{year}			{csvsqldb::Token token{csvsqldb::TOK_YEAR}; return token;}
{month}			{csvsqldb::Token token{csvsqldb::TOK_MONTH}; return token;}
{day}			{csvsqldb::Token token{csvsqldb::TOK_DAY}; return token;}
{explain}			{csvsqldb::Token token{csvsqldb::TOK_EXPLAIN}; return token;}
{show}			{csvsqldb::Token token{csvsqldb::TOK_SHOW}; return token;}
{mapping}			{csvsqldb::Token token{csvsqldb::TOK_MAPPING}; return token;}
{exec}			{csvsqldb::Token token{csvsqldb::TOK_EXEC}; return token;}
{arbitrary}			{csvsqldb::Token token{csvsqldb::TOK_ARBITRARY}; return token;}
{identifier}        {csvsqldb::Token token{csvsqldb::TOK_IDENTIFIER}; token.setIdentifier(text()); return token;}
{quoted_identifer}  {csvsqldb::Token token{csvsqldb::TOK_QUOTED_IDENTIFIER}; token.setQuotedIdentifier(text()); return token;}
%%
