/* 
 Exercise: Safely parse a binary header in Rust
 _a game asset begins with this 12-byte little-endian header_

 * Tasks:
 * 1. identify at least four problems or missing guarantees in `parse_header`
 * 2. replace it with a completely safe function
 * 3. validate the magic value, header length and declared payload length
 * 4. explain whether `ptr::read_unaligned` would make the original parser correct
 * 
 * Hint:
 * parse the wire format by field. treat byte order and validation as properties
 * of the file format, not of the host machine.
 * 
 * Problems identified: (before checking my work with LLM)
 * 1. we are not explicit about endianness when parsing the bytes
 * 2. we are not explicit about the length of the data we're passing to `parse_header`
 * 3. we have no error handling, and are assuming we'll only hit the happy path
 * 4. we assume that the order of fields as declared in the struct aligns with how the raw data will be parsed, but we are not explicit about this
 * 
 * Claude's analysis of the problems, after checking my work
 * 1. no minimum length check
 * 2. possible pointer misalignment
 * 3. host endianness is used instead of explicit little-endian
 * 4. in-memory struct layout is coupled to the wire format
 * 5. magic and payload length are not validated
 * 6. there is no error-return mechanism
 * 
 * "your instincts are right; i'd mainly sharpen 'not explicit about length'
 * into 'does not validate the available length,' and the last point into 
 * 'incorrectly equates `repr(C)` memory layout with the serialized format"
 */

 #[derive(Debug, PartialEq)]
 enum ParseError {
    TruncatedHeader,
    BadMagic,
    LengthOverflow,
    TruncatedPayload,
 }

#[derive(Debug, PartialEq)]
struct Header {
    magic: [u8; 4],
    version: u16,
    flags: u16,
    payload_len: u32,
}

fn parse_packet(bytes: &[u8]) -> Result<(Header, &[u8]), ParseError> {
    const HEADER_LEN: usize = 12;

    // expect 12 bytes
    if bytes.len() < HEADER_LEN {
        return Err(ParseError::TruncatedHeader)
    }

    let magic: [u8; 4] = bytes[0..4].try_into().unwrap();

    if magic != *b"WLF1" {
        return Err(ParseError::BadMagic);
    }

    let version = u16::from_le_bytes(bytes[4..6].try_into().unwrap());
    let flags = u16::from_le_bytes(bytes[6..8].try_into().unwrap());
    let payload_len = u32::from_le_bytes(bytes[8..12].try_into().unwrap());

    let payload_len_usize = 
        usize::try_from(payload_len)
        .map_err(|_| ParseError::LengthOverflow)?;

    let payload_end = HEADER_LEN   
        .checked_add(payload_len_usize)
        .ok_or(ParseError::LengthOverflow)?;

    if payload_end > bytes.len() {
        return Err(ParseError::TruncatedPayload);
    }

    let header = Header {
        magic,
        version,
        flags,
        payload_len
    };
    let payload = &bytes[HEADER_LEN..payload_end];

    Ok((header, payload))
}

fn main() {
    let packet: &[u8] = &[
        b'W', b'L', b'F', b'1',
        0x02, 0x00,             // version = 2
        0x05, 0x00,             // flags = 5
        0x04, 0x00, 0x00, 0x00, // payload length = 4
        0xde, 0xad, 0xbe, 0xef,
    ];

    let (header, payload) = parse_packet(packet).unwrap();

    assert_eq!(header.version, 2);
    assert_eq!(header.flags, 5);
    assert_eq!(header.payload_len, 4);
    assert_eq!(payload, &[0xde, 0xad, 0xbe, 0xef]);

    println!(
        "magic: {:#?}\nversion: {}\nflags: {}\npayload_len: {}\n",
        header.magic,
        header.version,
        header.flags,
        header.payload_len,
    );
}