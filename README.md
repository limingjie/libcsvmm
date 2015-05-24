[![Build Status](https://travis-ci.org/limingjie/libcsvmm.svg?branch=master)](https://travis-ci.org/limingjie/libcsvmm)

# libcsvmm
Yet another CSV library in C++. (In development...)

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

### Process input stream
- Create an empty new field and an empty new record.
- Switch to [data state](#data-state).

### Data state
Consume the next input character:
- **"," (U+002C)**
  - [Emit the current field](#emit-the-current-field).
- **CR (U+000D)**
  - [Emit the current field](#emit-the-current-field).
  - [Emit the current record](#emit-the-current-record).
  - Switch to [data CR state](#data-cr-state).
- **LF (U+000A) - Unix/Linux newline**
  - [Emit the current field](#emit-the-current-field).
  - [Emit the current record](#emit-the-current-record).
- **U+0022 QUOTATION MARK (")**
  - Switch to [quoted field state](#quoted-field-state).
- **EOF**
  - If the last input character is **"," (U+002C)**, [emit the current field](#emit-the-current-field).
  - If the current record is not null, [emit the current record](#emit-the-current-record).
  - End of tokenization.
- **Anything else**
  - Switch to [field state](#field-state).
  - Reconsume the current input character.

### Data CR state
(Old Mac newline compatibility.)

Consume the next input character:
- **LF (U+000A) - Windows newline**
  - Switch to [data state](#data-state).
- **EOF**
  - End of tokenization.
- **Anything else - Old Mac newline**
  - Switch to [data state](#data-state).
  - Reconsume the current input character.

### Field state
Consume the next input character:
- **"," (U+002C)**
- **CR (U+000D)**
- **LF (U+000A)**
  - Switch to [data state](#data-state).
  - Reconsume the current input character.
- **EOF**
  - [Emit the current field](#emit-the-current-field).
  - [Emit the current record](#emit-the-current-record).
  - End of tokenization.
- **Anything else**
  - Append the current input character to the current field.

### Quoted field state
Consume the next input character:
- **U+0022 QUOTATION MARK (")**
  - Switch to [quoted field quote state](#quoted-field-quote-state).
- **CR (U+000D)**
  - Append CRLF to the current field.
  - Switch to [quoted field CR state](#quoted-field-cr-state).
- **LF (U+000A) - Unix/Linux newline**
  - Append CRLF to the current field.
- **EOF**
  - [Emit the current field](#emit-the-current-field).
  - [Emit the current record](#emit-the-current-record).
  - End of tokenization.
- **Anything else**
  - Append the current input character to the current field.

### Quoted field quote state
Consume the next input character:
- **"," (U+002C)**
- **CR (U+000D)**
- **LF (U+000A)**
  - Reconsume the current input character.
  - Switch to [data state](#data-state).
- **U+0022 QUOTATION MARK (")**
  - Append the current input character to the current field.
  - Switch to [quoted field state](#quoted-field-state).
- **EOF**
  - [Emit the current field](#emit-the-current-field).
  - [Emit the current record](#emit-the-current-record).
  - End of tokenization.
- **Anything else**
  - Switch to [field state](#field-state).

### Quoted field CR state
(Old Mac newline compatibility.)

Consume the next input character:
- **LF (U+000A)**
  - Windows newline.
  - Switch to [quoted field state](#quoted-field-state).
- **EOF**
  - If the current field is not null, [emit the current field](#emit-the-current-field).
  - If the current record is not null, [emit the current record](#emit-the-current-record).
  - End of tokenization.
- **Anything else**
  - Old Mac newline.
  - Switch to [quoted field state](#quoted-field-state).
  - Reconsume the current input character.

### Emit the current field
- Add the current field to the current record.
- Create an empty new field.

### Emit the current record
- Add the current record to CSV file.
- Create an empty new record.
