# libcsvmm
Yet another CSV library in C++.

## Definition of The CSV Format
[rfc4180](http://tools.ietf.org/rfc/rfc4180.txt)
```
file = [header CRLF] record *(CRLF record) [CRLF]
header = name *(COMMA name)
record = field *(COMMA field)
name = field
field = (escaped / non-escaped)
escaped = DQUOTE *(TEXTDATA / COMMA / CR / LF / 2DQUOTE) DQUOTE
non-escaped = *TEXTDATA
COMMA = %x2C
CR = %x0D ;as per section 6.1 of RFC 2234
DQUOTE =  %x22 ;as per section 6.1 of RFC 2234
LF = %x0A ;as per section 6.1 of RFC 2234
CRLF = CR LF ;as per section 6.1 of RFC 2234
TEXTDATA =  %x20-21 / %x23-2B / %x2D-7E
```

## Tokenization
Create an empty record.

### Data state
Consume the next input character:
- **"," (U+002C)**
  - Emit an empty field.
- **CR (U+000D)**
  - Ignore the character.
- **LF (U+000A)**
  - Emit an empty field. Emit current record.
- **U+0022 QUOTATION MARK (")**
  - Create a new field. Switch to [quoted field state](#quoted-field-state).
- **Anything else**
  - Create a new field. Switch to [field state](#field-state). Reconsume the current input character.

### Field state
Consume the next input character:
- **"," (U+002C)**
  - Emit current field. Switch to [data state](#data-state).
- **CR (U+000D)**
  - Ignore the character.
- **LF (U+000A)**
  - Emit current field. Emit current record. Switch to [data state](#data-state).
- **Anything else**
  - Append the current input character to the current field.

### Quoted field state
Consume the next input character:
- **U+0022 QUOTATION MARK (")**
  - Switch to [quoted field quote state](#quoted-field-quote-state).
- **CR (U+000D)**
  - Ignore the character.
- **LF (U+000A)**
  - Append CRLF to the current field.
- **Anything else**
  - Append the current input character to the current field.

### Quoted field quote state
Consume the next input character:
- **U+0022 QUOTATION MARK (")**
  - Append U+0022 QUOTATION MARK (") to the current field. Switch to [quoted field state](#quoted-field-state).
- **"," (U+002C)**
  - Emit current field. Switch to [data state](#data-state).
- **CR (U+000D)**
  - Ignore the character.
- **LF (U+000A)**
  - Emit current field. Emit current record. Switch to [data state](#data-state).
- **Anything else**
  - Parse error. Treat it as text, append to the current field. Switch to [field state](#field-state).
