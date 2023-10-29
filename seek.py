with open("msys-2.0.dll", "rb+") as f:  # Open in read+write mode, use 'rb' for read-only
    f.seek(0x1EA800)  # Go to the offset
    data = f.read(100)  # Read 100 bytes, you can change this number based on how much you want to read
    
    print(data)
    # If you want to write data, you can do:
    # f.write(b'your_data_here')
