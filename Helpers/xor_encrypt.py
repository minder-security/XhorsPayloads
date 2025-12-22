import sys

def xor_encrypt(data, key):
    key_bytes = key.encode()
    return bytes([data[i] ^ key_bytes[i % len(key_bytes)] for i in range(len(data))])

def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <source_file> <xor_key> <output_file>")
        sys.exit(1)

    source_file = sys.argv[1]
    xor_key = sys.argv[2]
    output_file = sys.argv[3]

    with open(source_file, "rb") as f:
        data = f.read()

    encrypted = xor_encrypt(data, xor_key)

    with open(output_file, "wb") as f:
        f.write(encrypted)

    print(f"|+| Encrypted {len(data)} bytes")
    print(f"|+| Key: {xor_key}")
    print(f"|+| Output: {output_file}")

if __name__ == "__main__":
    main()