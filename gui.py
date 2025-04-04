import socket
import tkinter as tk

def send_command():
    """Send command to C++ server and update UI"""
    command = entry.get()
    client.sendall(command.encode())  # Send command to C++
    entry.delete(0, tk.END)

    # Get database name and response
    database = client.recv(1024).decode()
    response = client.recv(1024).decode()

    # Update database label
    db_label.config(text=f"{database} >> ")

    # Display response
    text.insert(tk.END, response + "\n")
    text.see(tk.END)

# Connect to C++ server
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(("127.0.0.1", 9999))

# GUI setup
root = tk.Tk()
root.title("Database GUI")

# Entry box for SQL commands
entry = tk.Entry(root, width=100)
entry.grid(row=0, column=1, padx=5, pady=5)

# Run button
button = tk.Button(root, text="Run", command=send_command)
button.grid(row=0, column=2, padx=5, pady=5)

# Database label (initially "Database: ?")
db_label = tk.Label(root, text="? >>", fg="blue", font=("Arial", 10, "bold"))
db_label.grid(row=0, column=0, padx=10, pady=5, sticky="w")

# Output box for displaying results
text = tk.Text(root, height=50, width=100)
text.grid(row=1, column=0, columnspan=3, padx=5, pady=5)

root.mainloop()
client.close()
