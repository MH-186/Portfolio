import tkinter as tk # Import the tkinter library
# This block of code creates the main interface
import math
import threading




class Calculator: # Create a class Calculator
    def __init__(self, root):
        self.root = root # Assign the root window to the variable self.root
        self.root.title("Main Menu") # Set the title of the root windows to "Main Menu"
        self.root.config(padx=100, pady=50, bg="#f18930") # Set the size and background color of the root window
        self.createwidgets() # Call the createwidgets method to create widgets for the main interface

    def createwidgets(self):
        
          # Create the widgets (label and button) for the main interface
        tk.Label(self.root, text="Welcome Fellows!", bg="#f18930").pack()# Create a label with welcome message
        tk.Button(self.root, text="CLICK ME!", pady=25, padx=55,bd=30, command=self.menu2).pack()# Create a button that calls menu2 method when clicked

    def menu2(self): # creating the second window for the calculator
        self.menu = tk.Toplevel(self.root) # Create a new Toplevel window as a child of the root window
        self.menu.title("Calculator") # Set the title of the second window to "Calculator"
        self.menu.config(padx=100, pady=100, bg="#1c1c1c")# Set the size of the second window
        self.createcalculator()# set the createcalculator method to create the calculator interface

    def createcalculator(self):# Create the calculator interface
        self.operator = ""
        self.inputvalue = tk.StringVar()# Initialize the input values of the calculator to a StringVar
                # Create the entry widget for the calculator display
        displaytext = tk.Entry(self.menu, font=("arial", 20, "bold"), textvariable=self.inputvalue, bd=30, insertwidth=4, bg="#1c1c1c", justify=tk.RIGHT, fg="white")
        displaytext.grid(columnspan=5)# Set the widget to span across 5 columns
    
         # List of tuples representing each button and its position (row, column)
        buttons = [
            ("sin", 1, 0), ("cos", 1, 2), ("tan", 1, 3), ("sqrt", 1, 4),
            ("7", 2, 0), ("8", 2, 2), ("9", 2, 3), ("+", 2, 4),
            ("4", 3, 0), ("5", 3, 2), ("6", 3, 3), ("-", 3, 4),
            ("1", 4, 0), ("2", 4, 2), ("3", 4, 3), ("C", 4, 4),
            ("0", 5, 0), ("/", 5, 2), ("*", 5, 3), ("x³", 5, 4),
            ("x²", 6, 0), ("(", 6, 2), (")", 6, 3), ("=",6, 4)
        ]
# This for loop will be responsible for giving each buttons task, some are individually written some are together.
        for buttontext, row, col in buttons:
            if buttontext in ["cos"]:# Altering size, font, function and color takes place here
                tk.Button(self.menu, padx=3, bd=8,fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#d4d4d2",
                          command=lambda txt=buttontext: self.tigonometryclick(txt)).grid(row=row, column=col)
            elif buttontext in ["sin"]:# I put sin in a different "if" function because I want to alter it's size without changing cos' size
                tk.Button(self.menu, padx=3, bd=8, fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#d4d4d2",
                          command=lambda txt=buttontext: self.tigonometryclick(txt)).grid(row=row, column=col)
            elif buttontext in ["tan"]:# I'm doing the same thing to tan, previously, these 3 were put in the same "if" function, but that way they share the same size, which I don't want
                tk.Button(self.menu, padx=3, bd=8, fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#d4d4d2",
                          command=lambda txt=buttontext: self.tigonometryclick(txt)).grid(row=row, column=col)
            elif buttontext == "sqrt":# specifies the appearance, text, and command to be executed when the button is clicked. The grid layout manager is then used to place the button inside the parent widget.
                tk.Button(self.menu, padx=1, bd=8, fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#ff9500",
                          command=lambda: self.square_root_click()).grid(row=row, column=col)
            elif buttontext == "x²":  # Handling button for raising to the power of 2
                tk.Button(self.menu, padx=10, bd=8, fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#505050",
                          command=lambda: self.power_of_2_click()).grid(row=row, column=col)
            elif buttontext == "x³":  # Handling button for raising to the power of 2
                tk.Button(self.menu, padx=12, bd=8, fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#ff9500",
                          command=lambda: self.power3()).grid(row=row, column=col)
            elif buttontext in ["+", "="]:
                tk.Button(self.menu, padx=16, bd=8, fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#ff9500",
                          command=lambda txt=buttontext: self.buttonclick(txt)).grid(row=row, column=col)
            elif buttontext in ["C"]:
                tk.Button(self.menu, padx=14, bd=8, fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#ff9500",
                          command=lambda txt=buttontext: self.buttonclick(txt)).grid(row=row, column=col)
            elif buttontext in ["-"]:
                tk.Button(self.menu, padx=19, bd=8, fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#ff9500",
                          command=lambda txt=buttontext: self.buttonclick(txt)).grid(row=row, column=col)
            else:
                tk.Button(self.menu, padx=16, bd=8, fg="black", font=("arial", 20, "bold"), text=buttontext, bg="#505050",
                          command=lambda txt=buttontext: self.buttonclick(txt)).grid(row=row, column=col)

 
    def tigonometryclick(self, trigonometry):#addign threading for trigonometry to prevent freezing blocking of functionality of the gui
        threading.Thread(target=self.calculatetrigo, args=(trigonometry,)).start()
        
    def calculatetrigo(self,trigonometry):# Trigonometry function starts here, it is used whenever the user clicks any of the trigonometry functions
        angle = float(self.operator) if self.operator else 0.0
        if trigonometry == "sin":# heres when sin is applied by the user
            result = math.sin(math.radians(angle))
        elif trigonometry == "cos":# heres when cos is used by the user
            result = math.cos(math.radians(angle))
        elif trigonometry == "tan":# heres when tan is clicked by the user
            result = math.tan(math.radians(angle))
 # this is made possible using python's library, "import math", written in the very beginning
        self.inputvalue.set(result)
        self.operator = str(result)
 # Handles the square root button click event and calculates the square root of input value
    def square_root_click(self):# defines a method named square_root_click within a class
        try:# This starts a try block, which means that any problems that arise will be handled and the code within will be run.
            result = math.sqrt(float(self.operator))# This line calculates the square root of the input value stored in self.operator, handling any potential errors.
            self.inputvalue.set(result)# This line updates the display with the calculated square root result.
            self.operator = str(result)# This line converts the square root result to a string and assigns it to the self.operator attribute.
        except ValueError:# This line initiates an exception handling block specifically for catching ValueError.
            self.inputvalue.set("Error")# This line sets the input value to "Error" if a ValueError occurs during the square root calculation.
  # Handles the button click event for raising input value to the power of 2
    def power_of_2_click(self):# This line defines a method power_of_2_click in a class, handling button clicks to compute the square of the input value, taking self as a parameter.
        try:# This starts a try block, which means that any problems that arise will be handled and the code within will be run.
            result = float(self.operator) ** 2 # This line calculates the square of the input value by converting it to a float and raising it to the power of 2.
            self.inputvalue.set(result)# This line updates the UI to display the computed result by setting the value of the inputvalue attribute.
            self.operator = str(result)# This line ensures data type consistency by converting the result to a string and assigning it to self.operator.
        except ValueError:# This line initiates an exception handling block specifically for catching ValueError.
            self.inputvalue.set("Error")# This line sets the input value to "Error" if a ValueError occurs during the square root calculation.

    def power3(self):
        try:
            result=float(self.operator)**3
            self.inputvalue.set(result)
            self.operator=str(result)
        except ValueError:
            self.inputvalue.set("Error")

    def buttonclick(self, number): # Method to handle buttons 
        if number == "=": 
             result = eval(self.operator) #evaluate the expresion stored in the opeartor variable
             self.inputvalue.set(result) #set the input value to the resutl
             self.operator = str(result)
        elif number == "C":
            self.operator = "" #when the button 'C' it will reset the operator 
            self.inputvalue.set("")#clear the input value
        else:
            self.operator += str(number)
            self.inputvalue.set(self.operator)

if __name__ == "__main__":
    root = tk.Tk()  # Create the main Tkinter window
    app = Calculator(root)# Create an instance of the Calculator class
    
    root.mainloop() # Start the Tkinter event loop

