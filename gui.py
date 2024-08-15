import ctypes
import os
from tkinter import Tk, Button, Label, StringVar, filedialog

# Load the shared library
compress_lib = ctypes.CDLL('./compress.dll')
decompress_lib = ctypes.CDLL('./decompress.dll')

# Define the argument and return types of the C++ function
compress_file = compress_lib.compress_file
compress_fileargtypes = [ctypes.c_char_p, ctypes.c_char_p]
compress_file.restype = None
decompress_file = decompress_lib.decompress_file
decompress_fileargtypes = [ctypes.c_char_p, ctypes.c_char_p]
decompress_file.restype = None

# Functions to call the C++ functions compress and decompress
def select_file_compress():
    file_path = filedialog.askopenfilename()
    os.startfile(os.path.dirname(file_path))

    # Define the output file name by replacing the original extension with .bin
    base_name = os.path.splitext(file_path)[0]
    output_file = base_name + ".bin"

    # Call the C++ compression function
    compress_file(file_path.encode('utf-8'), output_file.encode('utf-8'))

    result_var.set("Called C++ function: compress file")
    print(f'File compression initiated for: {file_path}')

def select_file_decompress():
    file_path = filedialog.askopenfilename()
    os.startfile(os.path.dirname(file_path))

    # Define the output file name by replacing the original extension with .bin
    base_name = os.path.splitext(file_path)[0]
    output_file = base_name + "_.txt"

    # Call the C++ decompression function
    decompress_file(file_path.encode('utf-8'), output_file.encode('utf-8'))
    
    result_var.set("Called C++ function: decompress file")
    print(f'File decompression initiated for: {file_path}')

# Set up Tkinter
root = Tk()
root.title("File compression/decompression")

result_var = StringVar()
result_var.set("")
Label(root, textvariable=result_var).pack(padx=10, pady=10)


# Create UI elements
select_button = Button(root, text="Select file to compress", command=select_file_compress)
select_button.pack(pady=20)

select_button = Button(root, text="Select file to decompress", command=select_file_decompress)
select_button.pack(pady=5)


# Set the dimensions of the window (width x height)
window_width = 400
window_height = 300
root.geometry(f"{window_width}x{window_height}")

# Run the Tkinter main loop
root.mainloop()