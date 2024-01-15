import base64

def simple_encrypt(input_str, cycles):
    result = input_str
    for cycle in range(1, cycles + 1):
        temp_result = ''
        cycle_key = (cycle * 123) % 256
        for char in result:
            encrypted_char = chr(ord(char) ^ cycle_key)
            temp_result += encrypted_char
        result = temp_result

    # Base64 encode the result
    result_bytes = result.encode('utf-8')  # Convert to bytes
    base64_encoded_result = base64.b64encode(result_bytes).decode('utf-8')  # Base64 encode and convert back to string
    return base64_encoded_result

# Example usage
input_string = "new "
cycles = 4  # Number of cycles for encryption

encrypted = simple_encrypt(input_string,cycles)
print("Encrypted:", encrypted)
