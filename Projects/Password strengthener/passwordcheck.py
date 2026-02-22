inp = input("Enter your password for checking: ")

if len(inp) < 8:
    print('Password too short, it should be 8+ characters.')
elif not any(char in inp for char in "$#@*_,."):
    print('Password needs at least one special character.')
elif not any(char.isdigit() for char in inp):
    print('Password needs at least one number.')
elif not any(char.isupper() for char in inp):
    print('Password needs at least one uppercase letter.')
else:
    print('Your password is strong.')
